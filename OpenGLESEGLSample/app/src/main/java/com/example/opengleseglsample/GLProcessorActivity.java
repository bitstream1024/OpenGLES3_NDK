package com.example.opengleseglsample;

import androidx.appcompat.app.AppCompatActivity;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;

public class GLProcessorActivity extends AppCompatActivity {

    private final static String TAG = "MainActivity";

    NativeEGLHelper mNativeEGLHelper = new NativeEGLHelper();
    private boolean bRenderResume = true;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
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
                if (0 != retCode) {
                    Log.d(TAG, "mNativeEGLHelper.Init retCode = " + retCode);
                    return;
                }

                while (bRenderResume) {
                    int retCode2 = mNativeEGLHelper.Draw();
                    Log.d(TAG, "mNativeEGLHelper.Draw retCode = " + retCode2);
                    if (retCode2 != 0)
                        break;

                    /*Bitmap bitmap = mNativeEGLHelper.CreateBitmapFromGLSurface (0, 0, 1080, 1920);
                    GLProcessorActivity.this.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            findViewById(R.id.image_view).setBackground(new BitmapDrawable(getResources(), bitmap));
                        }
                    });*/

                    try {
                        Thread.sleep(10);
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


