package com.example.opengleseglsample;

import android.Manifest;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Toast;

import com.example.permission.PermissionHelper;
import com.example.permission.PermissionInterface;

import androidx.appcompat.app.AppCompatActivity;

public class HomeActivity extends AppCompatActivity {

    private final String TAG  = this.getClass().getName();

    private final static String[]PermissionList = new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.READ_EXTERNAL_STORAGE};

    private boolean bPermissionOK = false;

    // Used to load the library on application startup.
    static {
        System.loadLibrary("mediahelper");
    }

    @Override
    protected void onCreate (Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_home);
        init ();
        requestPermission ();
    }

    public void requestPermission () {
        PermissionHelper.MyRequestPermission(this, PermissionList, new PermissionInterface() {
            @Override
            public int doPermissionSucceed() {
                bPermissionOK = true;
                Toast.makeText(HomeActivity.this, "onCreate doPermissionSucceed", Toast.LENGTH_SHORT).show();
                return 0;
            }

            @Override
            public int doPermissionFailed() {
                Toast.makeText(HomeActivity.this, "onCreate doPermissionFailed", Toast.LENGTH_SHORT).show();
                return 0;
            }
        });
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        PermissionHelper.onMyRequestPermissionsResult(requestCode, permissions, grantResults);
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }

    @Override
    protected void onResume () {
        super.onResume();
    }

    @Override
    protected void onDestroy () {
        super.onDestroy();
    }

    private void init () {

        findViewById(R.id.home_btn_glactivity).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startGLActivity ();
            }
        });
    }

    public void onClick (View view) {
        switch (view.getId()) {
            case R.id.home_btn_glactivity:
                startGLActivity();
                break;
            case R.id.home_btn_openslactivity:
                startOpenSLESActivity();
                break;
            default:
                break;
        }
    }

    private void startGLActivity () {
//        if (bPermissionOK) {
            Intent intent = new Intent(this, GLProcessorActivity.class);
            startActivity(intent);
//        }
        /*else {
            Toast.makeText(HomeActivity.this, "Please ammit permission", Toast.LENGTH_SHORT).show();
        }*/
    }

    private void startOpenSLESActivity() {
        startActivity(new Intent(this, OpenSLESActivity.class));
    }
}
