package com.cgwang1580.openglessamples;

import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.WindowManager;
import com.cgwang1580.multimotionhelper.MotionStateGL;
import com.cgwang1580.multimotionhelper.MultiMotionEventHelper;
import com.cgwang1580.utils.CommonDefine;
import com.cgwang1580.utils.MyLog;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;

public class GLViewActivity extends AppCompatActivity {

    private final static String PROCESSOR_NAME = "processor.draw";
    static {
        System.loadLibrary(PROCESSOR_NAME);
    }
    private final String TAG = this.getClass().getName();
    private List<String> mEffectList = null;
    private MultiMotionEventHelper mMultiMotionHelper = null;
    private MyGLSurfaceView myGLSurfaceView;
    private int mEffectType = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        MyLog.d(TAG, "onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_gl_view);
        mMultiMotionHelper = new MultiMotionEventHelper();
        mEffectType = getIntent().getIntExtra(CommonDefine.MESSAGE_EFFECT_TYPE, 0);
        MyLog.d(TAG, "onCreate mEffectType = " + mEffectType);
        initUI (mEffectType);
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
        myGLSurfaceView = null;
        super.onDestroy();
    }

    public void InitGLSurfaceView (Context context) {
        MyLog.d(TAG, "InitGLSurfaceView");
        myGLSurfaceView = new MyGLSurfaceView(context, mEffectType);
    }

    private void initUI (int effectType) {
        MyLog.d(TAG, "initUI");
        setNavigationColor ();
        mEffectList = new ArrayList<String>(Arrays.asList("Triangle", "SimpleTexture", "TextureFBO",
                "HardwareBuffer", "Transform", "Render3D", "TriangleFBO", "Render3DMesh"));
        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            if (effectType < mEffectList.size()) {
                actionBar.setTitle(mEffectList.get(effectType));
            }
        }
    }

    @Override
    public boolean onSupportNavigateUp() {
        finish();
        return true;
    }
    private void setNavigationColor () {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_NAVIGATION);
        }
    }
}
