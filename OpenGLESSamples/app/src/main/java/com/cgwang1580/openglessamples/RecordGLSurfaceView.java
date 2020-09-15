package com.cgwang1580.openglessamples;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

import com.cgwang1580.utils.CommonDefine;
import com.cgwang1580.utils.MyLog;

import java.util.jar.Attributes;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class RecordGLSurfaceView extends GLSurfaceView {

    private final String TAG = this.getClass().getName();

    private final String VIDEO_NAME = "test_record.mp4";
    private final int GLES_VERSION = 3;
    private final int RENDER_FPS = 30;

    private NativeFunctionSet mNativeFunctionSet = new NativeFunctionSet();
    private int mDrawType = 0;
    private int mOffscreenTexture = -1;
    private boolean bGLStateReady = false;

    public RecordGLSurfaceView(Context context) {
        super(context);
    }

    public RecordGLSurfaceView (Context context, AttributeSet attrs) {
        super(context, attrs);
        init ();
    }

    public void init () {
        MyLog.d(TAG, "init");
        setEGLContextClientVersion(GLES_VERSION);
        setRenderer(mRenderer);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    public void RecordGLSurfacePause () {
        this.queueEvent(this::GLSurfaceDestroyed);
    }

    public void GLSurfaceDestroyed() {
        MyLog.d(TAG, "GLSurfaceDestroyed");
        int ret = mNativeFunctionSet.onSurfaceDestroyedJNI ();
        MyLog.d(TAG, "onSurfaceDestroyedJNI ret = " + ret);
        bGLStateReady = false;
    }

    private GLSurfaceView.Renderer mRenderer = new GLSurfaceView.Renderer() {
        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            mDrawType = 6;
            int ret = mNativeFunctionSet.onSurfaceCreatedByTypeJNI(mDrawType);
            if (ret != CommonDefine.ERROR_OK) {
                MyLog.e(TAG, "onSurfaceCreatedByTypeJNI error");
                return;
            }
            mOffscreenTexture = mNativeFunctionSet.getTextureFromFrameBuffer();
            bGLStateReady = true;
            requestRender();
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            int ret = mNativeFunctionSet.onSurfaceChangedJNI(width, height);
            if (ret != CommonDefine.ERROR_OK) {
                MyLog.e(TAG, "onSurfaceChangedJNI error");
            }
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            int ret = mNativeFunctionSet.onDrawFrameJNI();
            if (ret != CommonDefine.ERROR_OK) {
                MyLog.e(TAG, "onDrawFrameJNI error");
                return;
            }
            try {
                Thread.sleep(1000/RENDER_FPS);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            if (bGLStateReady) {
                requestRender();
            }
        }
    };
}
