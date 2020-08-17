package com.example.media;

import android.annotation.SuppressLint;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.nfc.Tag;
import android.view.Surface;

import com.example.utils.MyLog;

import java.io.IOException;
import java.nio.ByteBuffer;

public class MediaCodecHelper {

    private final String TAG = this.getClass().getName();

    private int mVideoWidth = -1;
    private int mVideoHeight = -1;
    // bit rate, in bits per second
    private int mBitRate = -1;
    private static final int FRAME_RATE = 15;               // 15fps
    private static final int IFRAME_INTERVAL = 10;          // 10 seconds between I-frames

    private Surface mEncodeSurface = null;
    private MediaCodec mVideoEncoder = null;
    private MediaMuxer mVideoMuxer;
    private int mTrackIndex;
    private boolean mMuxerStarted;

    // allocate one of these up front so we don't need to do it every time
    private MediaCodec.BufferInfo mBufferInfo;

    public void PrepareEncoder() {

        MyLog.d(TAG, "prepareEncoder");

        if (null == mBufferInfo) {
            mBufferInfo = new MediaCodec.BufferInfo();
        }

        mVideoWidth = 1080;
        mVideoHeight = 1920;
        mBitRate = 2000000;

        MediaFormat videoFormat = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_AVC, mVideoWidth, mVideoHeight);
        videoFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface);
        videoFormat.setInteger(MediaFormat.KEY_BIT_RATE, mBitRate);
        videoFormat.setInteger(MediaFormat.KEY_FRAME_RATE, FRAME_RATE);
        videoFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, IFRAME_INTERVAL);
        MyLog.d(TAG, "prepareEncoder videoFormat = " + videoFormat);
        try {
            mVideoEncoder = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_VIDEO_AVC);
        } catch (IOException ioe) {
            ioe.printStackTrace();
        }
        mVideoEncoder.configure(videoFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
        // 创建一个surface，传给OpenGLES作为输出
        mEncodeSurface = mVideoEncoder.createInputSurface();
    }

    public void DrainEncoder (boolean endOfStream) {
        MyLog.d(TAG, "DrainEncoder (" + endOfStream + ")");
        final int TIMEOUT_USEC = 10000;

        if (endOfStream) {
            MyLog.d(TAG, "sending EOS to encoder");
            mVideoEncoder.signalEndOfInputStream();
        }

        ByteBuffer[] encoderOutputBuffers = mVideoEncoder.getOutputBuffers();
        while (true) {
            int encoderStatus = mVideoEncoder.dequeueOutputBuffer(mBufferInfo, TIMEOUT_USEC);
            if (encoderStatus == MediaCodec.INFO_TRY_AGAIN_LATER) {
                // no output available yet
                if (!endOfStream) {
                    break;      // out of while
                } else {
                    MyLog.d(TAG, "no output available, spinning to await EOS");
                }
            } else if (encoderStatus == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
                // not expected for an encoder
                encoderOutputBuffers = mVideoEncoder.getOutputBuffers();
            } else if (encoderStatus == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                // should happen before receiving buffers, and should only happen once
                if (mMuxerStarted) {
                    throw new RuntimeException("format changed twice");
                }
                MediaFormat newFormat = mVideoEncoder.getOutputFormat();
                MyLog.d(TAG, "encoder output format changed: " + newFormat);

                // now that we have the Magic Goodies, start the muxer
                mTrackIndex = mVideoMuxer.addTrack(newFormat);
                mVideoMuxer.start();
                mMuxerStarted = true;
            } else if (encoderStatus < 0) {
                MyLog.i(TAG, "unexpected result from encoder.dequeueOutputBuffer: " +
                        encoderStatus);
                // let's ignore it
            } else {
                ByteBuffer encodedData = encoderOutputBuffers[encoderStatus];
                if (encodedData == null) {
                    throw new RuntimeException("encoderOutputBuffer " + encoderStatus +
                            " was null");
                }

                if ((mBufferInfo.flags & MediaCodec.BUFFER_FLAG_CODEC_CONFIG) != 0) {
                    // The codec config data was pulled out and fed to the muxer when we got
                    // the INFO_OUTPUT_FORMAT_CHANGED status.  Ignore it.
                    MyLog.d(TAG, "ignoring BUFFER_FLAG_CODEC_CONFIG");
                    mBufferInfo.size = 0;
                }

                if (mBufferInfo.size != 0) {
                    if (!mMuxerStarted) {
                        throw new RuntimeException("muxer hasn't started");
                    }

                    // adjust the ByteBuffer values to match BufferInfo (not needed?)
                    encodedData.position(mBufferInfo.offset);
                    encodedData.limit(mBufferInfo.offset + mBufferInfo.size);

                    mVideoMuxer.writeSampleData(mTrackIndex, encodedData, mBufferInfo);
                    MyLog.d(TAG, "sent " + mBufferInfo.size + " bytes to muxer");
                }

                mVideoEncoder.releaseOutputBuffer(encoderStatus, false);

                if ((mBufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                    if (!endOfStream) {
                        MyLog.i(TAG, "reached end of stream unexpectedly");
                    } else {
                        MyLog.d(TAG, "end of stream reached");
                    }
                    break;      // out of while
                }
            }
        }

    }

    public void ReleaseEncoder() {
        MyLog.d(TAG, "releasing encoder objects");
        if (mVideoEncoder != null) {
            mVideoEncoder.release();
            mVideoEncoder = null;
        }
        if (mEncodeSurface != null) {
            mEncodeSurface.release();
            mEncodeSurface = null;
        }
    }

    public void StartEncode() {
        MyLog.d(TAG, "startEncode");
        mVideoEncoder.start();

        @SuppressLint("SdCardPath") String outputPath = "/sdcard/out.mp4";
        MyLog.d(TAG, "startEncode" + outputPath);

        if (null == mVideoMuxer) {
            try {
                mVideoMuxer = new MediaMuxer(outputPath, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
            } catch (IOException ioe) {
                ioe.printStackTrace();
                throw new RuntimeException("StartEncode MediaMuxer creation failed", ioe);
            }
        }
        mTrackIndex = -1;
        mMuxerStarted = false;
    }

    public void StopEncode () {
        MyLog.d(TAG, "StopEncode");
        if (mVideoMuxer != null) {
            mVideoMuxer.stop();
            mVideoMuxer.release();
            mVideoMuxer = null;
        }
        if (mVideoEncoder != null) {
            mVideoEncoder.stop();
        }
    }

    public Surface GetEncodeSurface() {
        return mEncodeSurface;
    }

    public int GetVideoWidth() {
        return mVideoWidth;
    }

    public int GetVideoHeight() {
        return mVideoHeight;
    }
}
