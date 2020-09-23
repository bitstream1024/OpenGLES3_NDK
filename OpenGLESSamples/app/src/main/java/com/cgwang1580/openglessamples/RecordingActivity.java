package com.cgwang1580.openglessamples;

import android.app.Activity;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Toast;

import com.cgwang1580.glview.NativeFunctionHelper;
import com.cgwang1580.glview.RecordGLSurfaceView;
import com.cgwang1580.utils.CommonDefine;
import com.cgwang1580.utils.LogUtils;

import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;

public class RecordingActivity extends AppCompatActivity {

    private final static String PROCESSOR_NAME = "processor.draw";
    static {
        System.loadLibrary(PROCESSOR_NAME);
    }
    private final String TAG = this.getClass().getName();

    private RecordGLSurfaceView mGLView = null;
    private boolean mRecordingEnabled = false;
    private NativeFunctionHelper mNativeFunctionHelper = new NativeFunctionHelper();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        LogUtils.d(TAG, "onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_recording);
        initUI ();
    }

    @Override
    protected void onResume () {
        LogUtils.d(TAG, "onResume");
        super.onResume();
        int retCode = mNativeFunctionHelper.Init();
        if (retCode != CommonDefine.ERROR_OK) {
            LogUtils.e(TAG, "onResume mNativeFunctionHelper Init failed");
            Toast.makeText(this, "Init error", Toast.LENGTH_SHORT).show();
            return;
        }
        RecordGLSurfaceView.SetNativeFunctionHelper(mNativeFunctionHelper);
        if (null == mGLView) {
            InitGLSurfaceView(this);
        }
    }

    @Override
    protected void onPause () {
        LogUtils.d(TAG, "onPause");
        super.onPause();
        if (null != mGLView) {
            mGLView.RecordGLSurfacePause ();
            mGLView = null;
        }
        mNativeFunctionHelper.DestroyProcessor();
    }

    @Override
    protected void onDestroy() {
        LogUtils.d(TAG, "onDestroy");
        super.onDestroy();
    }

    public void InitGLSurfaceView (Context context) {
        LogUtils.d(TAG, "InitGLSurfaceView");
        mGLView = ((Activity)context).findViewById(R.id.render_view);
    }

    private void initUI () {
        LogUtils.d(TAG, "initUI");
        //setNavigationColor ();
        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setTitle("GLRecording");
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

    public void clickToggleRecording(View view) {
        mRecordingEnabled = !mRecordingEnabled;
        if (mGLView != null) {
            mGLView.queueEvent(() -> {
                // notify the renderer that we want to change the encoder's state
                mGLView.changeRecordingState(mRecordingEnabled);
            });
        }
        updateControls();
    }

    public void updateControls() {
        String text = "";
        if (mRecordingEnabled) {
            text = "stop recording";
        } else {
            text = "start recording";
        }
        ((Button)findViewById(R.id.btn_recording)).setText(text);
    }
}
