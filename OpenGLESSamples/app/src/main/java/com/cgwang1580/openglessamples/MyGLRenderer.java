package com.cgwang1580.openglessamples;

import android.opengl.GLSurfaceView;

import com.cgwang1580.multimotionhelper.MotionStateGL;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import static com.cgwang1580.utils.CommonDefine.ERROR_OK;

public class MyGLRenderer implements GLSurfaceView.Renderer {

    private final static String TAG = "MyGLRenderer";
    ///**** test for jni begin ***///
    /*private final static int TEST_STATIC_NUM = 10;
    private final int TEST_NUM = 20;
    private final String TEST_STRING = "test_string";*/
    ///**** test for jni end *****///
    private MyGLSurfaceView mMyGLSurfaceView;

    public boolean getCreateState() {
        return bCreateState;
    }

    private boolean bCreateState = false;

    MyGLRenderer (MyGLSurfaceView glSurfaceView) {
        mMyGLSurfaceView = glSurfaceView;
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
        int ret = onSurfaceChangedJNI (width, height);
        MyLog.d(TAG, "onSurfaceChangedJNI ret = " + ret);
        mMyGLSurfaceView.setGLViewSize(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        MyLog.d(TAG, "onDrawFrame");
        MotionStateGL motionStateGL = new MotionStateGL();
        motionStateGL = mMyGLSurfaceView.getMotionState();
        int ret = SetMotionState(motionStateGL);
        ret = onDrawFrameJNI ();
        MyLog.d(TAG, "onDrawFrameJNI ret = " + ret);
        mMyGLSurfaceView.setRenderCount(mMyGLSurfaceView.getRenderCount() + 1);
        mMyGLSurfaceView.requestRender();
    }

    public void GLSurfaceCreated () {
        MyLog.d(TAG, "GLSurfaceCreated");
        int ret = onSurfaceCreatedJNI();
        MyLog.d(TAG, "onSurfaceCreatedJNI ret = " + ret);
        bCreateState = (ret == ERROR_OK);
    }

    public void GLSurfaceDestroyed() {
        MyLog.d(TAG, "GLSurfaceDestroyed");
        int ret = onSurfaceDestroyedJNI ();
        bCreateState = false;
        MyLog.d(TAG, "onSurfaceDestroyedJNI ret = " + ret);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native int onSurfaceCreatedJNI();
    public native int onSurfaceChangedJNI (int width, int height);
    public native int onDrawFrameJNI ();
    public native int onSurfaceDestroyedJNI ();
    public native int SetMotionState (MotionStateGL motionStateGL);
}
