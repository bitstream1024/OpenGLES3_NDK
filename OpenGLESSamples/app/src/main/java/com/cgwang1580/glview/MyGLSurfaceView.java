package com.cgwang1580.glview;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Handler;
import android.os.Message;
import com.cgwang1580.multimotionhelper.MotionStateGL;
import com.cgwang1580.openglessamples.R;
import com.cgwang1580.utils.CommonDefine;
import com.cgwang1580.utils.LogUtils;
import com.cgwang1580.utils.UIMsgManager;

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MyGLSurfaceView implements GLSurfaceView.Renderer{
    private final String TAG = this.getClass().getName();
    private final static long MAX_RENDER_COUNT = 500;

    public final static int MSG_SURFACE_CREATED = 0x10;
    public final static int MSG_FRAME_RENDER_END = 0x11;

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
    private EGLConfig mEGLConfig = null;

    private NativeFunctionHelper mNativeFunctionHelper = null;
    private UIMsgManager mUIMsgManager = null;
    private float m_fTotalRenderTime = 0;

    public MyGLSurfaceView (Context context, int drawType, NativeFunctionHelper nativeFunctionHelper) {
        mNativeFunctionHelper = nativeFunctionHelper;
        initGL(context, drawType);
    }

    protected void initGL(Context context, int drawType) {

        LogUtils.d(TAG, "initGL");
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

    public void setMsgManager(UIMsgManager msgManager) {
        this.mUIMsgManager = msgManager;
    }

    protected void requestRender () {
        LogUtils.d(TAG, "requestRender");
        if (null != mGLSurfaceView ){ //&& mRenderCount < MAX_RENDER_COUNT) {
            mGLSurfaceView.requestRender();
        }
    }

    public void resume () {
        if (null != mGLSurfaceView) {
            mGLSurfaceView.onResume();
        }
    }
    public void pause () {
        if (null != mGLSurfaceView) {
            mGLSurfaceView.onPause();
        }
    }

    public void MyGLSurfacePause () {
        if (null != mGLSurfaceView) {
            mGLSurfaceView.queueEvent(new Runnable() {
                @Override
                public void run() {
                    GLSurfaceDestroyed();
                }
            });
            /*if (mRenderHandler != null) {
                // todo: should not in main thread
                mRenderHandler.waitUntilDestroy();
            }*/
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
        LogUtils.d(TAG, "setMotionState");
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
        LogUtils.d(TAG, "getMotionState");
        motionStateGL.mMotionType =  m_MotionStateGL.mMotionType;
        motionStateGL.translate_x = m_MotionStateGL.translate_x;
        motionStateGL.translate_y = m_MotionStateGL.translate_y;
        motionStateGL.translate_z = m_MotionStateGL.translate_z;
        m_MotionStateLock.unlock();
        return motionStateGL;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        LogUtils.d(TAG, "onSurfaceCreated");
        GLSurfaceCreated ();
        mEGLConfig = config;
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        LogUtils.d(TAG, "onSurfaceChanged");
        //GLES30.glViewport(0, 0, width, height);
        int ret = mNativeFunctionHelper.OnSurfaceChanged(width, height);
        LogUtils.d(TAG, "onSurfaceChangedJNI ret = " + ret);
        setGLViewSize(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        LogUtils.d(TAG, "onDrawFrame");
        if (!bGLStateReady) {
            return;
        }
        MotionStateGL motionStateGL = getMotionState();
        int retCode = mNativeFunctionHelper.SetMotionState(motionStateGL);
        if (CommonDefine.ReturnCode.ERROR_OK != retCode) {
            LogUtils.d(TAG, "SetMotionState ret = " + retCode);
            return;
        }

        long beginTime = System.currentTimeMillis();

        retCode = mNativeFunctionHelper.OnDrawFrame();
        if (CommonDefine.ReturnCode.ERROR_OK != retCode) {
            LogUtils.d(TAG, "onDrawFrameJNI ret = " + retCode);
            return;
        }

        long endTime = System.currentTimeMillis();
        mRenderCount = mRenderCount + 1;
        m_fTotalRenderTime = m_fTotalRenderTime + (endTime - beginTime);
        if (null != mRenderHandler && mRenderCount > 0) {
            float fFrameTime = m_fTotalRenderTime/mRenderCount;
            //LogUtils.d(TAG, "onDrawFrame fFrameTime: " + fFrameTime);
            mRenderHandler.sendMessage(mRenderHandler.obtainMessage(MSG_FRAME_RENDER_END, fFrameTime));
        }

        requestRender();
    }

    public void GLSurfaceCreated () {
        LogUtils.d(TAG, "GLSurfaceCreated");
        int ret = mNativeFunctionHelper.OnSurfaceCreatedByType(mDrawType);
        LogUtils.d(TAG, "onSurfaceCreatedJNI ret = " + ret);
        bGLStateReady = (ret == CommonDefine.ReturnCode.ERROR_OK);
        if (bGLStateReady) {
            mRenderHandler.sendMessage(mRenderHandler.obtainMessage(MSG_SURFACE_CREATED));
            //mRenderHandler.setGLState(bGLStateReady);
        }
    }

    public void GLSurfaceDestroyed() {
        LogUtils.d(TAG, "GLSurfaceDestroyed");
        bGLStateReady = false;
        int ret = mNativeFunctionHelper.OnSurfaceDestroyed();
        LogUtils.d(TAG, "onSurfaceDestroyedJNI ret = " + ret);
        //mRenderHandler.setGLState(false);
    }

    protected void updateRenderTime(float renderTime) {
        if (null != mUIMsgManager) {
            mUIMsgManager.updateRenderTime(renderTime);
        }
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
            LogUtils.d(TAG, "RenderHandler msgType = " + msgType);
            switch (msgType) {
                case MSG_SURFACE_CREATED:
                    requestRender();
                    break;
                case MSG_FRAME_RENDER_END:
                    float renderTime = (float) msg.obj;
                    LogUtils.d(TAG, "handleMessage renderTime: " + renderTime);
                    updateRenderTime(renderTime);
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
