//
// Created by ChauncyWang on 2020/8/17.
//

#include <LogAndroid.h>
#include "MediaCodecHelper.h"
#include <media/NdkImage.h>
#include <media/NdkImageReader.h>
#include <media/NdkMediaCodec.h>
#include <media/NdkMediaCrypto.h>
#include <media/NdkMediaDrm.h>
#include <media/NdkMediaError.h>
#include <media/NdkMediaExtractor.h>
#include <media/NdkMediaFormat.h>
#include <media/NdkMediaMuxer.h>
#include <common.h>

#include <utility>

#define AMEDIA_FORMAT_MIME	"video/avc"
#define MY_COLOR_FormatSurface	0x7f000789

MediaCodecHelper* MediaCodecHelper::m_pNativeMediaCodecHelper = nullptr;

MediaCodecHelper* MediaCodecHelper::CreateInstance()
{
	if (!MediaCodecHelper::m_pNativeMediaCodecHelper) {
		MediaCodecHelper::m_pNativeMediaCodecHelper = new  MediaCodecHelper();
	}
	return MediaCodecHelper::m_pNativeMediaCodecHelper;
}

MediaCodecHelper::~MediaCodecHelper()
{
	LOGD("MediaCodecHelper::MediaCodecHelper");
}

MediaCodecHelper::MediaCodecHelper():
		m_pMediaCodec(nullptr),
		m_pMediaMuxer(nullptr),
		m_pNativeWindow(nullptr),
		m_VideoWidth(0),
		m_VideoHeight(0),
		m_FPS(0),
		m_bMuxerStarted(false)
{
	LOGD("MediaCodecHelper::MediaCodecHelper");
	m_BitRate = 2000000;
	IFRAME_INTERVAL = 10;
	mTrackIndex = -1;
}


int MediaCodecHelper::PrepareEncoder(int width, int height, int fps, const std::string& path)
{
	LOGD ("MediaCodecHelper::PrepareEncoder");
	LOGD("PrepareEncoder width = %d, height = %d, fps = %d, path = %s", width, height, fps, path.c_str());

	m_VideoWidth = width;
	m_VideoHeight = height;
	m_FPS = fps;

	int ret = ERROR_OK;
	do
	{
		AMediaFormat *format = AMediaFormat_new ();
		if (nullptr == format)
		{
			LOGE("PrepareEncoder format is nullptr");
			ret = ERROR_NATIVE_MEDIA;
			break;
		}
		AMediaFormat_setString(format, AMEDIAFORMAT_KEY_MIME, AMEDIA_FORMAT_MIME);
		AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_WIDTH, m_VideoWidth);
		AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_HEIGHT, m_VideoHeight);

		AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_COLOR_FORMAT, MY_COLOR_FormatSurface);
		AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_BIT_RATE, m_BitRate);
		AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_FRAME_RATE, m_FPS);
		AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_I_FRAME_INTERVAL, IFRAME_INTERVAL);

		m_pMediaCodec = AMediaCodec_createEncoderByType (AMEDIA_FORMAT_MIME);
		media_status_t status = AMediaCodec_configure(m_pMediaCodec, format, NULL, NULL,
													  AMEDIACODEC_CONFIGURE_FLAG_ENCODE);
		if (0 != status)
		{
			LOGE("PrepareEncoder AMediaCodec_configure failed");
		}
		AMediaFormat_delete(format);

		int mfd = -1;
		FILE *fp = fopen(path.c_str(), "wb");
		if (nullptr != fp) {
			mfd = fileno(fp);
			if (nullptr == m_pMediaMuxer)
			{
				m_pMediaMuxer = AMediaMuxer_new (mfd, AMEDIAMUXER_OUTPUT_FORMAT_MPEG_4);
			}
			fclose(fp);
		}
		else {
			LOGE("PrepareEncoder fopen failed");
		}
	} while (false);

	return ret;
}

int MediaCodecHelper::PrepareEncoderWithCtx(int width, int height, int fps, const std::string& path, EGLContext const *eglContext)
{
	LOGD("MediaCodecHelper::PrepareEncoderWithCtx");

	/*if (nullptr == eglContext)
	{
		LOGE("PrepareEncoderWithCtx eglContext is nullptr");
		return ERROR_NATIVE_MEDIA;
	}*/

	int ret = PrepareEncoder(width, height, fps, path);
	LOGD("PrepareEncoderWithCtx ret = %d", ret);

	AMediaCodec_createInputSurface(m_pMediaCodec, &m_pNativeWindow);
	if (nullptr == m_pNativeWindow)
	{
		LOGE("PrepareEncoderWithCtx AMediaCodec_createInputSurface failed");
		return ERROR_NATIVE_MEDIA;
	}
	media_status_t status;
	status = static_cast<media_status_t>(AMediaCodec_start(m_pMediaCodec) != AMEDIA_OK);
	if (status)
	{
		LOGE("PrepareEncoderWithCtx AMediaCodec_start failed");
	}
	else
	{
		LOGD("PrepareEncoderWithCtx AMediaCodec_start succeeded");
	}

	return 0;
}

ANativeWindow* MediaCodecHelper::GetInputWindow()
{
	LOGD("MediaCodecHelper::GetInputWindow");
	return m_pNativeWindow;
}


void MediaCodecHelper::DrainEncoder(bool eof)
{
	LOGD("MediaCodecHelper::DrainEncoder");
	if (eof) {
		ssize_t ret = AMediaCodec_signalEndOfInputStream(m_pMediaCodec);
		LOGD("DrainEncoder ret = %d", ret);
	}

	while (true) 
	{
		AMediaCodecBufferInfo info;
		//time out usec 10000
		const int TIME_OUT = 10000;
		ssize_t status = AMediaCodec_dequeueOutputBuffer(m_pMediaCodec, &info, TIME_OUT);

		if (status == AMEDIACODEC_INFO_TRY_AGAIN_LATER) {
			if (!eof) {
				LOGD("DrainEncoder video no output available, AMEDIACODEC_INFO_TRY_AGAIN_LATER");
				break;
			}else{
				LOGD("DrainEncoder video no output available, spinning to await EOS");
			}
		}
		else if (status == AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED) {
			// not expected for an encoder
		}
		else if (status == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {
			if (m_bMuxerStarted) {
				LOGD("DrainEncoder format changed twice");
			}

			AMediaFormat *fmt = AMediaCodec_getOutputFormat(m_pMediaCodec);
			const char *s = AMediaFormat_toString(fmt);
			LOGD("DrainEncoder video output format %s", s);

			mTrackIndex = static_cast<int>(AMediaMuxer_addTrack(m_pMediaMuxer, fmt));

			if(mTrackIndex != -1) {
				LOGD("DrainEncoder AMediaMuxer_start");
				AMediaMuxer_start(m_pMediaMuxer);
				m_bMuxerStarted = true;
			}
		} else {
			uint8_t *encodeData = AMediaCodec_getOutputBuffer(m_pMediaCodec, status, NULL/* out_size */);
			if (encodeData == NULL) {
				LOGD("DrainEncoder encoder output buffer was null");
			}

			if ((info.flags & AMEDIACODEC_BUFFER_FLAG_CODEC_CONFIG) != 0) {
				LOGD("DrainEncoder ignoring AMEDIACODEC_BUFFER_FLAG_CODEC_CONFIG");
				info.size = 0;
			}

			int dataSize = static_cast<int>(static_cast<size_t>(info.size));

			if (dataSize != 0) {
				if (!m_bMuxerStarted) {
					LOGD("DrainEncoder muxer has't started");
				}
				LOGD("DrainEncoder AMediaMuxer_writeSampleData video size %d", dataSize);
				AMediaMuxer_writeSampleData(m_pMediaMuxer, static_cast<size_t>(mTrackIndex), encodeData, &info);
			}

			AMediaCodec_releaseOutputBuffer(m_pMediaCodec, static_cast<size_t>(status), false);

			if ((info.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM) != 0) {
				if (!eof) {
					LOGD("DrainEncoder reached end of stream unexpectly");
				} else {
					LOGD("DrainEncoder video end of stream reached");
				}
				break;
			}
		}
	}
}

void MediaCodecHelper::ReleaseEncoder()
{
	LOGD("MediaCodecHelper::ReleaseEncoder");

	if (nullptr != m_pNativeWindow)
	{
		ANativeWindow_release (m_pNativeWindow);
		m_pNativeWindow = nullptr;
	}

	if (nullptr != m_pMediaMuxer)
	{
		AMediaMuxer_stop(m_pMediaMuxer);
		AMediaMuxer_delete(m_pMediaMuxer);
		m_pMediaMuxer = nullptr;
	}

	if (nullptr != m_pMediaCodec)
	{
		AMediaCodec_stop (m_pMediaCodec);
		AMediaCodec_delete (m_pMediaCodec);
		m_pMediaCodec = nullptr;
	}
}
