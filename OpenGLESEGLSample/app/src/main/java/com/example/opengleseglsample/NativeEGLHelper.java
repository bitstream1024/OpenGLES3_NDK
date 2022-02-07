package com.example.opengleseglsample;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.SurfaceTexture;
import android.opengl.GLES20;
import android.opengl.GLException;
import android.opengl.GLSurfaceView;
import android.view.Surface;
import android.view.TextureView;
import android.view.WindowManager;

import com.example.utils.DebugLog;

import java.nio.IntBuffer;

public class NativeEGLHelper {

    private final String TAG = this.getClass().getName();

    private TextureView mTextureView = null;
    private Surface mWindowSurface = null;

    private boolean bSurfaceReady = false;

    NativeEGLHelper () {}

    NativeEGLHelper (Activity activity) {
        activity.getWindow().setFlags(WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED,
                WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED);
        if (null == mTextureView) {
            mTextureView = activity.findViewById(R.id.texture_view);
            mTextureView.setSurfaceTextureListener(mSurfaceTextureListener);
        }
    }

    private TextureView.SurfaceTextureListener mSurfaceTextureListener = new TextureView.SurfaceTextureListener() {
        @Override
        public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
            DebugLog.d(TAG, "onSurfaceTextureAvailable width = " + width + " height = " + height);
            mWindowSurface = new Surface(surface);
            SetWindowRender(mWindowSurface, width, height);
            bSurfaceReady = true;
        }

        @Override
        public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {

        }

        @Override
        public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
            return false;
        }

        @Override
        public void onSurfaceTextureUpdated(SurfaceTexture surface) {

        }
    };

    public boolean isbSurfaceReady() {
        return bSurfaceReady;
    }

    public void setSurfaceReady(boolean bSetSurface) {
        this.bSurfaceReady = bSetSurface;
    }

    public native int Init ();

    public native int SetWindow (Surface surface, int width, int height);

    public native int SetWindowRender (Surface surface, int width, int height);

    public native int UnInit ();

    public native int Draw ();

    public native int SetImageData (byte[] data, int imgWidth, int imgHeight, int format);

    public Bitmap CreateBitmapFromGLSurface (int x, int y, int w, int h) {
        int bitmapBuffer[] = new int[w * h];
        int bitmapSource[] = new int[w * h];
        IntBuffer intBuffer = IntBuffer.wrap(bitmapBuffer);
        intBuffer.position(0);
        try {
            GLES20.glReadPixels(x, y, w, h, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE,
                    intBuffer);
            int offset1, offset2;
            for (int i = 0; i < h; i++) {
                offset1 = i * w;
                offset2 = (h - i - 1) * w;
                for (int j = 0; j < w; j++) {
                    int texturePixel = bitmapBuffer[offset1 + j];
                    int blue = (texturePixel >> 16) & 0xff;
                    int red = (texturePixel << 16) & 0x00ff0000;
                    int pixel = (texturePixel & 0xff00ff00) | red | blue;
                    bitmapSource[offset2 + j] = pixel;
                }
            }
        } catch (GLException e) {
            return null;
        }
        return Bitmap.createBitmap(bitmapSource, w, h, Bitmap.Config.ARGB_8888);
    }

}
