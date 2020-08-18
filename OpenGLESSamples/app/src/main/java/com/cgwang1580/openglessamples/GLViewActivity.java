package com.cgwang1580.openglessamples;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.MotionEvent;
import android.widget.Toast;

import com.cgwang1580.multimotionhelper.MotionStateGL;
import com.cgwang1580.permission.PermissionHelper;
import com.cgwang1580.permission.PermissionInterface;
import com.cgwang1580.multimotionhelper.MultiMotionEventHelper;
import com.cgwang1580.utils.CommonDefine;
import com.cgwang1580.utils.MyLog;

import androidx.appcompat.app.AppCompatActivity;

public class GLViewActivity extends AppCompatActivity {

    private final static String PROCESSOR_NAME = "processor.draw";

    static {
        System.loadLibrary(PROCESSOR_NAME);
    }
    private final String TAG = this.getClass().getName();

    MultiMotionEventHelper mMultiMotionHelper = null;

    private final static String[]PermissionList = new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.READ_EXTERNAL_STORAGE , Manifest.permission.CAMERA};

    MyGLSurfaceView myGLSurfaceView;
    private int mEffectType = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        MyLog.d(TAG, "onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mMultiMotionHelper = new MultiMotionEventHelper();

        mEffectType = getIntent().getIntExtra(CommonDefine.MESSAGE_EFFECT_TYPE, 0);
        MyLog.d(TAG, "onCreate mEffectType = " + mEffectType);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (null != mMultiMotionHelper) mMultiMotionHelper.handleViewEvent(event);
        MotionStateGL motionStateGL = mMultiMotionHelper.getMotionStateGL(myGLSurfaceView.getGLViewWidth(),
                myGLSurfaceView.getGLViewHeight());
        motionStateGL.logoutTransform();

        myGLSurfaceView.setMotionState(motionStateGL);

        return true;
    }

    @Override
    protected void onResume () {
        MyLog.d(TAG, "onResume");
        super.onResume();
        if (null == myGLSurfaceView) {
            InitGLSurfaceView(this);
        }
        //myGLSurfaceView.MyGLSurfaceResume();
        myGLSurfaceView.requestRender();
    }

    @Override
    protected void onPause () {
        MyLog.d(TAG, "onPause");
        super.onPause();
        if (null != myGLSurfaceView) {
            myGLSurfaceView.MyGLSurfacePause ();
        }
    }

    @Override
    protected void onDestroy() {
        MyLog.d(TAG, "onDestroy");
        super.onDestroy();
    }

    public void InitGLSurfaceView (Context context) {
        MyLog.d(TAG, "InitGLSurfaceView");
        myGLSurfaceView = new MyGLSurfaceView();
        myGLSurfaceView.Init(context);
    }
}
