package com.example.opengleseglsample;

import android.content.Context;
import android.opengl.GLES10;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class SimpleGLSurfaceView extends GLSurfaceView{

    private final String TAG = this.getClass().getName();
    private final int GLES_VERSION = 3;

    private int mTextureTarget = -1;
    private int mOESTexture = -1;

    public SimpleGLSurfaceView(Context context) {
        super(context);
        setEGLContextClientVersion(GLES_VERSION);
        setRenderer(mRenderer);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    private final GLSurfaceView.Renderer mRenderer = new GLSurfaceView.Renderer(){

        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {

        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {

        }

        @Override
        public void onDrawFrame(GL10 gl) {

        }
    };
}
