package com.cgwang1580.openglessamples;

import com.cgwang1580.multimotionhelper.MotionStateGL;

public class NativeFunctionSet {

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native int onSurfaceCreatedJNI();
    public native int onSurfaceCreatedByTypeJNI(int effectType);
    public native int onSurfaceChangedJNI (int width, int height);
    public native int onDrawFrameJNI ();
    public native int onSurfaceDestroyedJNI ();
    public native int SetMotionState (MotionStateGL motionStateGL);


    // for GL recording
    public native int getTextureFromFrameBuffer ();
}
