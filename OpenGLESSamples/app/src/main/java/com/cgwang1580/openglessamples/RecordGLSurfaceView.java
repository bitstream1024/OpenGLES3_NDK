package com.cgwang1580.openglessamples;

import android.app.Activity;
import android.content.Context;
import android.opengl.EGL14;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Environment;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.Button;

import com.cgwang1580.encoder.video.TextureMovieEncoder;
import com.cgwang1580.utils.CommonDefine;
import com.cgwang1580.utils.MyLog;
import java.io.File;
import java.nio.IntBuffer;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;
import javax.microedition.khronos.opengles.GL10;

public class RecordGLSurfaceView extends GLSurfaceView {

    private final String TAG = this.getClass().getName();

    private final String VIDEO_NAME = "test_record.mp4";
    private final int GLES_VERSION = 3;
    private final int RENDER_FPS = 30;

    private static final int RECORDING_OFF = 0;
    private static final int RECORDING_ON = 1;
    private static final int RECORDING_RESUMED = 2;

    private int mDrawType = 0;
    private int mOffscreenTexture = -1;
    private boolean bGLStateReady = false;

    private File mOutputFile = null;
    private boolean mRecordingEnabled = false;
    private int mRecordingStatus;
    private int mWindowWith = 0;
    private int mWindowHeight = 0;
    private int mFrameCount = 0;
    private Button mBtnRecording = null;

    private EGLConfig mEGLConfig = null;
    private EGLSet mEGLSet = null;

    // this is static so it survives activity restarts
    private static TextureMovieEncoder mVideoEncoder = new TextureMovieEncoder();

    public static NativeFunctionHelper mNativeFunctionHelper = null;
    public static void SetNativeFunctionHelper (NativeFunctionHelper nativeFunctionHelper) {
        mNativeFunctionHelper = nativeFunctionHelper;
    }

    public class EGLSet {
        private EGL10 mEGL = null;
        private EGLContext mEGLContext = null;
        private EGLDisplay mEGLDisplay = null;
        private EGLSurface mWindowSurface = null;
        public EGLSet (EGLConfig eglConfig) {
            mEGL = (EGL10) EGLContext.getEGL();
            if (mEGL != null) {
                mEGLContext = mEGL.eglGetCurrentContext();
                mEGLDisplay = mEGL.eglGetCurrentDisplay();
                mWindowSurface = mEGL.eglGetCurrentSurface(EGL10.EGL_DRAW);
            }
        }

        public EGLSurface getEGLSurface () {
            return mWindowSurface;
        }

        public void makeWindowSurfaceCurrent () {
            mEGL.eglMakeCurrent(mEGLDisplay, mWindowSurface, mWindowSurface, mEGLContext);
        }
    }

    public RecordGLSurfaceView(Context context) {
        super(context);
    }

    public RecordGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    public void init(Context context) {
        MyLog.d(TAG, "init");
        setEGLContextClientVersion(GLES_VERSION);
        setRenderer(mRenderer);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        mOutputFile = new File(Environment.getExternalStorageDirectory() + "/" + "test_record.mp4");
        MyLog.d(TAG, "init path = " + mOutputFile.getPath());
        mBtnRecording = ((Activity)context).findViewById(R.id.btn_recording);
    }

    public void RecordGLSurfacePause() {
        this.queueEvent(this::GLSurfaceDestroyed);
    }

    public void GLSurfaceDestroyed() {
        MyLog.d(TAG, "GLSurfaceDestroyed");
        int ret = mNativeFunctionHelper.OnSurfaceDestroyed();
        MyLog.d(TAG, "onSurfaceDestroyedJNI ret = " + ret);
        bGLStateReady = false;
    }

    private GLSurfaceView.Renderer mRenderer = new GLSurfaceView.Renderer() {
        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig conVideoEncoderConfig) {
            mDrawType = 6;
            mEGLConfig = conVideoEncoderConfig;
            mEGLSet = new EGLSet(mEGLConfig);
            int ret = mNativeFunctionHelper.OnSurfaceCreatedByType(mDrawType);
            if (ret != CommonDefine.ERROR_OK) {
                MyLog.e(TAG, "onSurfaceCreatedByTypeJNI error");
                return;
            }
            mOffscreenTexture = mNativeFunctionHelper.GetTextureFromFrameBuffer();
            MyLog.d(TAG, "onSurfaceCreated mOffscreenTexture = " + mOffscreenTexture);
            bGLStateReady = true;
            requestRender();
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            int ret = mNativeFunctionHelper.OnSurfaceChanged(width, height);
            if (ret != CommonDefine.ERROR_OK) {
                MyLog.e(TAG, "onSurfaceChangedJNI error");
            }
            mWindowWith = width;
            mWindowHeight = height;
        }

        @Override
        public void onDrawFrame(GL10 gl) {

            long beginTime = System.currentTimeMillis();

            boolean showBox = false;

            if (mRecordingEnabled) {
                switch (mRecordingStatus) {
                    case RECORDING_OFF:
                        Log.d(TAG, "START recording");
                        // start recording
                        mVideoEncoder.startRecording(new TextureMovieEncoder.EncoderConfig(
                                mOutputFile, mWindowWith, mWindowHeight, 4000000, EGL14.eglGetCurrentContext()));
                        mRecordingStatus = RECORDING_ON;
                        break;
                    case RECORDING_RESUMED:
                        Log.d(TAG, "RESUME recording");
                        mVideoEncoder.updateSharedContext(EGL14.eglGetCurrentContext());
                        mRecordingStatus = RECORDING_ON;
                        break;
                    case RECORDING_ON:
                        // yay
                        break;
                    default:
                        throw new RuntimeException("unknown status " + mRecordingStatus);
                }
            } else {
                switch (mRecordingStatus) {
                    case RECORDING_ON:
                    case RECORDING_RESUMED:
                        // stop recording
                        Log.d(TAG, "STOP recording");
                        mVideoEncoder.stopRecording();
                        mRecordingStatus = RECORDING_OFF;
                        break;
                    case RECORDING_OFF:
                        // yay
                        break;
                    default:
                        throw new RuntimeException("unknown status " + mRecordingStatus);
                }
            }

            int ret = mNativeFunctionHelper.OnDrawFrame();
            if (ret != CommonDefine.ERROR_OK) {
                MyLog.e(TAG, "onDrawFrameJNI error");
                return;
            }

            Log.d(TAG, "OnDrawFrame cost time 0 = " + Long.toString(System.currentTimeMillis() - beginTime));

            // Set the video encoder's texture name.  We only need to do this once, but in the
            // current implementation it has to happen after the video encoder is started, so
            // we just do it here.
            //
            // TODO: be less lame.
            mVideoEncoder.setTextureId(mOffscreenTexture);

            // Tell the video encoder thread that a new frame is available.
            // This will be ignored if we're not actually recording.
            mVideoEncoder.frameAvailable(null);

            // Draw a flashing box if we're recording.  This only appears on screen.
            showBox = (mRecordingStatus == RECORDING_ON);
            if (showBox && (++mFrameCount & 0x04) == 0) {
                //drawBox();
            }

            mEGLSet.makeWindowSurfaceCurrent();

            if (bGLStateReady) {
                requestRender();
            }

            Log.d(TAG, "OnDrawFrame cost time = " + Long.toString(System.currentTimeMillis() - beginTime));
        }
    };

    /**
     * Draws a red box in the corner.
     */
    private void drawBox() {
        int []viewport = new int[4];
        GLES20.glGetIntegerv(GLES20.GL_VIEWPORT, IntBuffer.wrap(viewport));
        //GLES20.glViewport(0, 0, );

        GLES20.glEnable(GLES20.GL_SCISSOR_TEST);
        GLES20.glScissor(0, 0, 50, 50);
        GLES20.glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        GLES20.glDisable(GLES20.GL_SCISSOR_TEST);
    }

    public void changeRecordingState(boolean isRecording) {
        MyLog.d(TAG, "changeRecordingState: was " + mRecordingEnabled + " now " + isRecording);
        mRecordingEnabled = isRecording;
    }

    public void setRecordingEnabled(boolean recordingEnabled) {
        mRecordingEnabled = recordingEnabled;
    }

    public boolean isRecordingEnabled() {
        return mRecordingEnabled;
    }
}
