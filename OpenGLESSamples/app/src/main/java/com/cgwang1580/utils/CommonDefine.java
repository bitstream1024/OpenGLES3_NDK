package com.cgwang1580.utils;

public class CommonDefine {

    public static class ReturnCode {
        public final static int ERROR_OK = 0;
    }

    public final static String MESSAGE_EFFECT_TYPE = "effect_type";

    public static class SampleType {
        public static final int eDraw_Triangle = 0x00;
        public static final int eDraw_SimpleTexture = eDraw_Triangle + 1;
        public static final int eDraw_TextureFBO = eDraw_Triangle + 2;
        public static final int eDraw_HardwareBuffer = eDraw_Triangle + 3;
        public static final int eDraw_Transform = eDraw_Triangle + 4;
        public static final int eDraw_Render3D = eDraw_Triangle + 5;
        public static final int eDraw_TriangleFBO = eDraw_Triangle + 6;
        public static final int eDraw_Render3DMesh = eDraw_Triangle + 7;
        public static final int eDraw_Texture = eDraw_Triangle + 8;
    }
}
