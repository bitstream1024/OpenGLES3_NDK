package com.cgwang1580.openglessamples;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;

import com.cgwang1580.multimotionhelper.MotionStateGL;
import com.cgwang1580.utils.LogUtils;

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class MyGLSurfaceView{
    private final static String TAG = "MyGLSurfaceView";
    private final static long MAX_RENDER_COUNT = 500;

    private final static int GLES_VERSION = 3;
    private GLSurfaceView mGLSurfaceView = null;
    private MyGLRenderer mGLRenderer = null;
    private int mRenderCount = 0;
    private int mGLViewWidth = 0;
    private int mGLViewHeight = 0;
    private MotionStateGL m_MotionStateGL;
    private Lock m_MotionStateLock;

    MyGLSurfaceView () {}

    protected void Init (Context context) {

        LogUtils.d(TAG, "Init");
        mGLSurfaceView = ((Activity)context).findViewById(R.id.gl_surface_view);
        mGLSurfaceView.setEGLContextClientVersion(GLES_VERSION);
        mGLRenderer = new MyGLRenderer(this);
        mGLSurfaceView.setRenderer(mGLRenderer);
        mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        mRenderCount = 0;
        mGLViewWidth = 0;
        mGLViewHeight = 0;
        m_MotionStateLock = new ReentrantLock();
        m_MotionStateGL = new MotionStateGL();
    }

    protected void requestRender () {
        LogUtils.d(TAG, "requestRender");
        if (null != mGLSurfaceView && mRenderCount < MAX_RENDER_COUNT) {
            mGLSurfaceView.requestRender();
        }
    }

    protected void MyGLSurfaceResume () {
        if (null != mGLSurfaceView) {
            mGLSurfaceView.queueEvent(new Runnable() {
                @Override
                public void run() {
                    if (!mGLRenderer.getCreateState()) {
                        mGLRenderer.GLSurfaceCreated();
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
                    mGLRenderer.GLSurfaceDestroyed();
                }
            });
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
    public int getRenderCount() {
        return mRenderCount;
    }
    public void setRenderCount(int renderCount) {
        mRenderCount = renderCount;
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
}
