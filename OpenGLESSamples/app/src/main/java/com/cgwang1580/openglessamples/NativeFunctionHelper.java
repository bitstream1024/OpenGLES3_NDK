package com.cgwang1580.openglessamples;

import com.cgwang1580.multimotionhelper.MotionStateGL;
import com.cgwang1580.utils.MyLog;

public class NativeFunctionHelper {

    private final String TAG = this.getClass().getName();
    private final int ERROR_JNI = -1;

    // 记录C层分配的Processor handler指针地址，在C层分配地址，回传给Java层，避免在jni层使用全局变量
    private long mProcessorHandle = 0;

    public NativeFunctionHelper () {}

    public int Init () {
        mProcessorHandle = CreateProcessorJNI();
        if (0 == mProcessorHandle) {
            MyLog.e(TAG, "Init error");
            return ERROR_JNI;
        }
        return InitProcessorResourceJNI(mProcessorHandle);
    }

    public int DestroyProcessor () {
        if (0 == mProcessorHandle) {
            return ERROR_JNI;
        }
        int retCode = DestroyProcessorJNI(mProcessorHandle);
        mProcessorHandle = 0;
        return retCode;
    }


    // call in GL thread
    public int OnSurfaceCreated () {
        if (0 == mProcessorHandle) {
            return ERROR_JNI;
        }
        return OnSurfaceCreatedJNI(mProcessorHandle);
    }

    public int OnSurfaceCreatedByType (int effectType) {
        if (0 == mProcessorHandle) {
            return ERROR_JNI;
        }
        return OnSurfaceCreatedByTypeJNI(mProcessorHandle, effectType);
    }

    public int OnSurfaceChanged (int width, int height) {
        if (0 == mProcessorHandle) {
            return ERROR_JNI;
        }
        return OnSurfaceChangedJNI(mProcessorHandle, width, height);
    }

    public int OnDrawFrame () {
        if (0 == mProcessorHandle) {
            return ERROR_JNI;
        }
        return OnDrawFrameJNI(mProcessorHandle);
    }

    public int OnSurfaceDestroyed () {
        if (0 == mProcessorHandle) {
            return ERROR_JNI;
        }
        return OnSurfaceDestroyedJNI(mProcessorHandle);
    }

    public int SetMotionState (MotionStateGL motionStateGL) {
        if (0 == mProcessorHandle) {
            return ERROR_JNI;
        }
        return SetMotionStateJNI(mProcessorHandle, motionStateGL);
    }

    public int GetTextureFromFrameBuffer () {
        if (0 == mProcessorHandle) {
            return ERROR_JNI;
        }
        return GetTextureFromFrameBufferJNI(mProcessorHandle);
    }



    // could not called in GL thread
    private native long CreateProcessorJNI ();
    private native int InitProcessorResourceJNI (long handle);
    private native int DestroyProcessorJNI (long handle);

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    private native int OnSurfaceCreatedJNI(long handle);
    private native int OnSurfaceCreatedByTypeJNI(long handle, int effectType);
    private native int OnSurfaceChangedJNI(long handle, int width, int height);
    private native int OnDrawFrameJNI(long handle);
    private native int OnSurfaceDestroyedJNI(long handle);
    private native int SetMotionStateJNI (long handle, MotionStateGL motionStateGL);

    // for GL recording
    private native int GetTextureFromFrameBufferJNI(long handle);
}
