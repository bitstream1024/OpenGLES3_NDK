package com.example.opengleseglsample;

import android.os.Bundle;
import android.view.MotionEvent;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

public class AAudioMakerActivity extends AppCompatActivity {

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_audiomaker);
        nativeStartEngine();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        nativeTouchEvent(event.getAction());
        return super.onTouchEvent(event);
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        nativeStopEngine();
        super.onDestroy();
    }

    public native void nativeTouchEvent(int action);
    public native void nativeStartEngine();
    public native void nativeStopEngine();
}