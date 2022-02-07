package com.example.opengleseglsample;

import android.annotation.SuppressLint;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.view.View;

import com.example.utils.DebugLog;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

public class OpenSLESActivity  extends AppCompatActivity {

    private final String TAG = this.getClass().getName();

    private boolean bReady = false;
    // audio player state
    private boolean bPlayerCreated = false;
    private boolean bPlaying = false;

    // audio recorder state
    private boolean bRecorderCreated = false;
    private boolean bRecording = false;

    // pcm player state
    private boolean bPcmPlayerCreated = false;
    private boolean bPcmPlaying = false;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_opensles);
    }

    @Override
    protected void onResume() {
        super.onResume();
        int nRet = nativeCreateSLEngine();
        DebugLog.d(TAG, "createSLEngine nRet = " + nRet);
    }

    @Override
    protected void onPause() {
        super.onPause();
        // stop play
        nativeSetPlayingAssetAudioPlayerState(false);
    }

    @Override
    protected void onDestroy() {
        nativeDestroySLEngine();
        super.onDestroy();
    }

    @SuppressLint("NonConstantResourceId")
    public void OnClick (View view) {
        switch (view.getId()) {
            case R.id.btn_play_background_music:
                setAudioPlayerState();
                break;
            case R.id.btn_record_audio:
                setAudioRecordState();
                break;
            case R.id.btn_play_pcm:
                setPcmPlayerState();
                break;
            default:
                break;
        }
    }

    private void setAudioPlayerState() {
        bPlaying = !bPlaying;
        if (!bPlayerCreated) {
            String musicName = "background.mp3";
            nativeCreateAssetAudioPlayer(this.getAssets(), musicName);
            bPlayerCreated = true;
        }
        nativeSetPlayingAssetAudioPlayerState(bPlaying);
    }

    private void setAudioRecordState() {
        bRecording = !bRecording;
        if (!bRecorderCreated) {
            nativeCreateAudioRecorder();
            bPlayerCreated = true;
        }
        if (bRecording) {
            nativeStartRecording();
        } else {
            nativeStopRecording();
        }
    }

    private void setPcmPlayerState() {
        bPcmPlaying = !bPcmPlaying;
        if (!bPcmPlayerCreated) {
            nativeCreatePcmPlayer();
            bPcmPlayerCreated = true;
        }
        if (bPcmPlaying) {
            nativeStartPcmPlayer();
        } else {
            nativeStopPcmPlayer();
        }
    }

    public native int nativeCreateSLEngine();

    public native boolean nativeCreateAssetAudioPlayer(AssetManager assetManager, String fileName);
    public native void nativeSetPlayingAssetAudioPlayerState(boolean bPlay);

    public native void nativeCreateAudioRecorder();
    public native void nativeStartRecording();
    public native void nativeStopRecording();

    public native void nativeCreatePcmPlayer();
    public native void nativeStartPcmPlayer();
    public native void nativeStopPcmPlayer();

    public native void nativeDestroySLEngine();
}