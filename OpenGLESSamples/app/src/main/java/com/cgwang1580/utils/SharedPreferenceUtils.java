package com.cgwang1580.utils;

import android.app.Activity;
import android.content.SharedPreferences;

import static android.content.Context.MODE_PRIVATE;

public class SharedPreferenceUtils {

    private final String TAG = "SharedPreferenceUtils";
    public final static String PREFERENCE_EFFECT_FILE = "effect_preference";
    private final String KEY_PREFERENCE_EFFECT = "effect_type";
    private Activity mActivity = null;

    public void Init (Activity activity) {
        if (activity != null) {
            SetupSharedPreference (activity);
        }
    }

    private void SetupSharedPreference (Activity activity) {
        mActivity = activity;
        SharedPreferences preferencesInfo = mActivity.getSharedPreferences(PREFERENCE_EFFECT_FILE, MODE_PRIVATE);
        int effectType = preferencesInfo.getInt(KEY_PREFERENCE_EFFECT, 0);
        if (effectType == 0) {
            SharedPreferences.Editor editor = preferencesInfo.edit();
            editor.putInt(KEY_PREFERENCE_EFFECT, 0);
            editor.apply();
        }
    }

    public SharedPreferences getSharedPreference () {
        SharedPreferences preferencesInfo = mActivity.getSharedPreferences(PREFERENCE_EFFECT_FILE, MODE_PRIVATE);
        return preferencesInfo;
    }

    public void setDefaultEffectTypeToSharedPreference(int effectType) {
        SharedPreferences preferencesInfo = mActivity.getSharedPreferences(PREFERENCE_EFFECT_FILE, MODE_PRIVATE);
        if (preferencesInfo == null) {
            return;
        }
        SharedPreferences.Editor editor = preferencesInfo.edit();
        editor.putInt(KEY_PREFERENCE_EFFECT, effectType);
        editor.apply();
    }

    public int getDefaultEffectTypeFromSharedPreference() {
        SharedPreferences preferencesInfo = mActivity.getSharedPreferences(PREFERENCE_EFFECT_FILE, MODE_PRIVATE);
        if (preferencesInfo == null) {
            return 0;
        }
        return preferencesInfo.getInt(KEY_PREFERENCE_EFFECT, 0);
    }
}
