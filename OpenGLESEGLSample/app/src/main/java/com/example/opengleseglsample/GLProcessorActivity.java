package com.example.opengleseglsample;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;

import com.example.media.MediaCodecHelper;

import java.io.IOException;

public class GLProcessorActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("eglutils");
    }

    private final String TAG = this.getClass().getName();

    private NativeEGLHelper mNativeEGLHelper = null;
    private boolean bRenderResume = true;
    private MediaCodecHelper mMediaCodecHelper = null;
    private final static int EACH_FRAME_TIME = 30;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_gl_processor);
        if (mNativeEGLHelper == null) {
            mNativeEGLHelper = new NativeEGLHelper(this);
        }
        if (null == mMediaCodecHelper) {
            mMediaCodecHelper = new MediaCodecHelper();
        }
    }

    @Override
    protected void onResume () {
        super.onResume();
        new Thread(new Runnable() {
            @Override
            public void run() {
                Log.d(TAG, "run");
                int retCode = 0;

                mMediaCodecHelper.PrepareEncoder();
                mNativeEGLHelper.SetWindow(mMediaCodecHelper.GetEncodeSurface(), mMediaCodecHelper.GetVideoWidth(),
                        mMediaCodecHelper.GetVideoHeight());
                mNativeEGLHelper.setSurfaceReady (true);

                while (!mNativeEGLHelper.isbSurfaceReady()) {
                    try {
                        Thread.sleep(200);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                };

                retCode = mNativeEGLHelper.Init();
                Log.d(TAG, "mNativeEGLHelper.Init retCode = " + retCode);

                mMediaCodecHelper.StartEncode();
                if (bRenderResume) {
                    for (int i = 0; i < 30; ++i) {
                        mMediaCodecHelper.DrainEncoder(false);
                        int retCode2 = mNativeEGLHelper.Draw();
                        Log.d(TAG, "mNativeEGLHelper.Draw retCode = " + retCode2);
                        if (retCode2 != 0)
                            break;
                    }
                    mMediaCodecHelper.DrainEncoder(true);
                }
                mMediaCodecHelper.ReleaseEncoder();
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
                        Thread.sleep(EACH_FRAME_TIME);
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


