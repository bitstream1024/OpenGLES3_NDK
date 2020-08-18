package com.cgwang1580.multimotionhelper;

import com.cgwang1580.utils.MyLog;

public class MotionStateGL {
    private static final String TAG = "MotionStateGL";
    public final int eMOTION_UNKNOWN = 0x00;
    public final int eMOTION_ROTATE = 0x01;
    public final int eMOTION_SCALE = 0x02;
    public final int eMOTION_TRANSLATE = 0x03;
    public int mMotionType;
    public float rotate_x, rotate_y, rotate_z;
    public float scale_x, scale_y, scale_z;
    public float translate_x, translate_y, translate_z;
    public MotionStateGL () {
        setZero ();
    }
    public MotionStateGL (int motionType, float transform_x, float transform_y, float transform_z) {
        mMotionType = 0;
        rotate_x = rotate_y = rotate_z = 0;
        scale_x = scale_y = scale_z = 0;
        translate_x = translate_y = translate_z = 0;
        switch (motionType) {
            case eMOTION_ROTATE:
                rotate_x = transform_x; rotate_y = transform_y; rotate_z = transform_z;
                break;
            case eMOTION_SCALE:
                scale_x = transform_x; scale_x = transform_y; scale_x = transform_z;
                break;
            case eMOTION_TRANSLATE:
                translate_x = transform_x; translate_y = transform_y; translate_z = transform_z;
                break;
            default:
                break;
        }
    }
    public void setZero() {
        mMotionType = eMOTION_UNKNOWN;
        rotate_x = rotate_y = rotate_z = 0.f;
        scale_x = scale_y = scale_z = 0;
        translate_x = translate_y = translate_z = 0.f;
    }

    public void logoutTransform() {
        MyLog.d(TAG, "logoutTransform");
        float transform_x = 0, transform_y = 0, transform_z = 0;
        boolean bLog = true;
        switch (mMotionType) {
            case eMOTION_ROTATE:
                transform_x = rotate_x; transform_y = rotate_y; transform_z = rotate_z;
                break;
            case eMOTION_SCALE:
                transform_x = scale_x; transform_y = scale_x; transform_z = scale_x;
                break;
            case eMOTION_TRANSLATE:
                transform_x = translate_x; transform_y = translate_y; transform_z = translate_z;
                break;
            default:
                bLog = false;
                MyLog.e(TAG, "logoutTransform mMotionType not supported");
                break;
        }
        if (bLog) {
            MyLog.d(TAG, "logoutTransform mMotionType = " + mMotionType + " transform("
                    + transform_x + " " + transform_y + " " + transform_z + ")");
        }
    }
}
