package com.example.gles;

import android.opengl.GLES11;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;

public class Texture2dProgram {

    public final static String TAG = "Texture2dProgram";

    public enum ProgramType {
        TEXTURE_2D, TEXTURE_EXT, TEXTURE_EXT_BW, TEXTURE_EXT_FILT
    }

    private ProgramType mProgramType;
    private int mTextureTarget;

    public Texture2dProgram (ProgramType programType) {
        mProgramType = programType;
        switch (programType) {
            case TEXTURE_2D:
                mTextureTarget = GLES20.GL_TEXTURE_2D;
                break;

            case TEXTURE_EXT:
                mTextureTarget = GLES11Ext.GL_TEXTURE_EXTERNAL_OES;
                break;

            default:
                break;
        }
    }
}
