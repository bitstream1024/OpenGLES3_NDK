package com.example.opengleseglsample;

import android.os.Bundle;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

public class GLRecordFboActivity extends AppCompatActivity {

    private final String TAG = this.getClass().getName();
    SimpleGLSurfaceView mGLSurfaceView = null;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_recording);
        mGLSurfaceView = findViewById(R.id.mgl_surface_view);
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }
}
