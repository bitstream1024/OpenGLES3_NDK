package com.cgwang1580.openglessamples;

import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.WindowManager;
import android.widget.Toast;

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
    private NativeFunctionHelper mNativeFunctionHelper = new NativeFunctionHelper();

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
        int retCode = mNativeFunctionHelper.Init();
        if (retCode != CommonDefine.ERROR_OK) {
            MyLog.e(TAG, "onResume mNativeFunctionHelper Init failed");
            Toast.makeText(this, "Init error", Toast.LENGTH_SHORT).show();
            return;
        }
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
            myGLSurfaceView = null;
        }
        mNativeFunctionHelper.DestroyProcessor();
    }

    @Override
    protected void onDestroy() {
        MyLog.d(TAG, "onDestroy");
        super.onDestroy();
    }

    public void InitGLSurfaceView (Context context) {
        MyLog.d(TAG, "InitGLSurfaceView");
        myGLSurfaceView = new MyGLSurfaceView(context, mEffectType, mNativeFunctionHelper);
    }

    private void initUI (int effectType) {
        MyLog.d(TAG, "initUI");
        //setNavigationColor ();
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
