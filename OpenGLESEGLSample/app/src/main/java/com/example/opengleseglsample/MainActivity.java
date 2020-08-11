package com.example.opengleseglsample;

import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.util.Log;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.example.permission.PermissionHelper;
import com.example.permission.PermissionInterface;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("eglutils");
    }

    private final static String TAG = "MainActivity";

    NativeEGLHelper mNativeEGLHelper = new NativeEGLHelper();
    private boolean bRenderResume = true;

    private final static String[]PermissionList = new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.READ_EXTERNAL_STORAGE};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        requestPermission ();
    }

    public void requestPermission () {
        PermissionHelper.MyRequestPermission(this, PermissionList, new PermissionInterface() {
            @Override
            public int doPermissionSucceed() {
                Toast.makeText(MainActivity.this, "onCreate doPermissionSucceed", Toast.LENGTH_SHORT).show();
                return 0;
            }

            @Override
            public int doPermissionFailed() {
                Toast.makeText(MainActivity.this, "onCreate doPermissionFailed",
                        Toast.LENGTH_SHORT).show();
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
        new Thread(new Runnable() {
            @Override
            public void run() {
                Log.d(TAG, "run");
                int retCode = 0;
                retCode = mNativeEGLHelper.Init();
                Log.d(TAG, "mNativeEGLHelper.Init retCode = " + retCode);

                while (bRenderResume) {
                    int retCode2 = mNativeEGLHelper.Draw();
                    Log.d(TAG, "mNativeEGLHelper.Draw retCode = " + retCode2);
                    if (retCode2 != 0)
                        break;

                    Bitmap bitmap = mNativeEGLHelper.CreateBitmapFromGLSurface (0, 0, 1080, 1920);
                    MainActivity.this.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            findViewById(R.id.image_view).setBackground(new BitmapDrawable(getResources(), bitmap));
                        }
                    });

                    try {
                        Thread.sleep(50);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                }

                bRenderResume = false;
                mNativeEGLHelper.UnInit();
            }
        }).start();
    }

    @Override
    protected void onPause () {
        super.onPause();
    }

    @Override
    protected void onDestroy () {
        super.onDestroy();

    }
}


