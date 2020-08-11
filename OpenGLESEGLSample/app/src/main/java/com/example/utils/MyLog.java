package com.example.utils;

import android.util.Log;

public class MyLog {

    static private boolean bShowLog = true;

    static public void e (String tag, String message) {
        if (bShowLog) {
            Log.e(tag + " JAVA", message);
        }
    }

    static public void d (String tag, String message) {
        if (bShowLog) {
            Log.d(tag + " JAVA", message);
        }
    }

    static public void i (String tag, String message) {
        if (bShowLog) {
            Log.i(tag + " JAVA", message);
        }
    }
}
