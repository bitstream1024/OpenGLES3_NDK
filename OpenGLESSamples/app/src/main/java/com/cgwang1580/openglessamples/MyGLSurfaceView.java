package com.cgwang1580.openglessamples;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Handler;
import android.os.Message;

import com.cgwang1580.multimotionhelper.MotionStateGL;
import com.cgwang1580.utils.MyLog;

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import static com.cgwang1580.utils.CommonDefine.ERROR_OK;

public class MyGLSurfaceView implements GLSurfaceView.Renderer{
    private final String TAG = this.getClass().getName();
    private final static long MAX_RENDER_COUNT = 500;

    public final static int MSG_SURFACE_CREATED = 0x10;

    private final static int GLES_VERSION = 3;
    private GLSurfaceView mGLSurfaceView = null;
    private int mDrawType = 0;
    private int mRenderCount = 0;
    private int mGLViewWidth = 0;
    private int mGLViewHeight = 0;
    private MotionStateGL m_MotionStateGL;
    private Lock m_MotionStateLock = null;
    private RenderHandler mRenderHandler = null;
    private boolean bGLStateReady = false;

    private NativeFunctionSet mNativeFunctionSet = new NativeFunctionSet();

    MyGLSurfaceView (Context context, int drawType) {
        initGL(context, drawType);
    }

    protected void initGL(Context context, int drawType) {

        MyLog.d(TAG, "initGL");
        mGLSurfaceView = ((Activity)context).findViewById(R.id.gl_surface_view);
        mGLSurfaceView.setEGLContextClientVersion(GLES_VERSION);
        mDrawType = drawType;

        mGLSurfaceView.setRenderer(this);
        mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        mRenderCount = 0;
        mGLViewWidth = 0;
        mGLViewHeight = 0;
        m_MotionStateLock = new ReentrantLock();
        m_MotionStateGL = new MotionStateGL();

        mRenderHandler = new RenderHandler();
    }

    protected void requestRender () {
        MyLog.d(TAG, "requestRender");
        if (null != mGLSurfaceView && mRenderCount < MAX_RENDER_COUNT) {
            mGLSurfaceView.requestRender();
        }
    }

    protected void MyGLSurfaceResume () {
        if (null != mGLSurfaceView) {
            mGLSurfaceView.queueEvent(new Runnable() {
                @Override
                public void run() {
                    if (!bGLStateReady) {
                        GLSurfaceCreated();
                    }
                }
            });
        }
    }

    protected void MyGLSurfacePause () {
        if (null != mGLSurfaceView) {
            mGLSurfaceView.queueEvent(new Runnable() {
                @Override
                public void run() {
                    GLSurfaceDestroyed();
                }
            });
            if (mRenderHandler != null) {
                // todo: should not in main thread
                mRenderHandler.waitUntilDestroy();
            }
            mRenderCount = 0;
        }
    }

    public int getGLViewWidth() {
        return mGLViewWidth;
    }
    public int getGLViewHeight() {
        return mGLViewHeight;
    }
    public void setGLViewSize(int GLViewWidth, int GLViewHeight) {
        mGLViewWidth = GLViewWidth;
        mGLViewHeight = GLViewHeight;
    }

    public void setMotionState (MotionStateGL motionState)
    {
        m_MotionStateLock.lock();
        MyLog.d(TAG, "setMotionState");
        m_MotionStateGL.mMotionType = motionState.mMotionType;
        m_MotionStateGL.translate_x = motionState.translate_x;
        m_MotionStateGL.translate_y = motionState.translate_y;
        m_MotionStateGL.translate_z = motionState.translate_z;
        m_MotionStateLock.unlock();
    }

    public MotionStateGL getMotionState()
    {
        MotionStateGL motionStateGL = new MotionStateGL();
        m_MotionStateLock.lock();
        MyLog.d(TAG, "getMotionState");
        motionStateGL.mMotionType =  m_MotionStateGL.mMotionType;
        motionStateGL.translate_x = m_MotionStateGL.translate_x;
        motionStateGL.translate_y = m_MotionStateGL.translate_y;
        motionStateGL.translate_z = m_MotionStateGL.translate_z;
        m_MotionStateLock.unlock();
        return motionStateGL;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        MyLog.d(TAG, "onSurfaceCreated");
        GLSurfaceCreated ();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        MyLog.d(TAG, "onSurfaceChanged");
        //GLES30.glViewport(0, 0, width, height);
        int ret = mNativeFunctionSet.onSurfaceChangedJNI (width, height);
        MyLog.d(TAG, "onSurfaceChangedJNI ret = " + ret);
        setGLViewSize(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        MyLog.d(TAG, "onDrawFrame");
        MotionStateGL motionStateGL = getMotionState();
        int ret = mNativeFunctionSet.SetMotionState(motionStateGL);
        MyLog.d(TAG, "SetMotionState ret = " + ret);
        ret = mNativeFunctionSet.onDrawFrameJNI();
        MyLog.d(TAG, "onDrawFrameJNI ret = " + ret);
        mRenderCount = mRenderCount + 1;
        if (bGLStateReady) {
            requestRender();
        }
    }

    public void GLSurfaceCreated () {
        MyLog.d(TAG, "GLSurfaceCreated");
        int ret = mNativeFunctionSet.onSurfaceCreatedByTypeJNI(mDrawType);
        MyLog.d(TAG, "onSurfaceCreatedJNI ret = " + ret);
        bGLStateReady = (ret == ERROR_OK);
        if (bGLStateReady) {
            mRenderHandler.sendMessage(mRenderHandler.obtainMessage(MSG_SURFACE_CREATED));
            mRenderHandler.setGLState(bGLStateReady);
        }
    }

    public void GLSurfaceDestroyed() {
        MyLog.d(TAG, "GLSurfaceDestroyed");
        int ret = mNativeFunctionSet.onSurfaceDestroyedJNI ();
        MyLog.d(TAG, "onSurfaceDestroyedJNI ret = " + ret);
        bGLStateReady = false;
        mRenderHandler.setGLState(false);
        mRenderHandler.notifyAfterDestroy();
    }

    @SuppressLint("HandlerLeak")
    private class RenderHandler extends Handler {

        private final Object mStopLock = new Object();
        private boolean bGLReady = false;

        public RenderHandler () {}

        @Override
        public void handleMessage(Message msg) {
            if (msg == null) return;
            int msgType = msg.what;
            MyLog.d(TAG, "RenderHandler msgType = " + msgType);
            switch (msgType) {
                case MSG_SURFACE_CREATED:
                    requestRender();
                    break;
                default:
                    break;
            }
        }

        public void setGLState(boolean bGLReady) {
            this.bGLReady = bGLReady;
        }

        public void waitUntilDestroy () {
            synchronized (mStopLock) {
                while (bGLReady) {
                    try {
                        mStopLock.wait();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        }

        public void notifyAfterDestroy () {
            synchronized (mStopLock) {
                mStopLock.notifyAll();
            }
        }
    }
}
