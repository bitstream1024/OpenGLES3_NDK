package com.example.opengleseglsample;

import android.content.res.AssetManager;
import android.os.Bundle;
import android.view.View;

import com.example.utils.MyLog;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

public class OpenSLESActivity  extends AppCompatActivity {

    private final String TAG = this.getClass().getName();

    private boolean bReady = false;
    private boolean bPlayerCreated = false;
    private boolean bPlay = false;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_opensles);
    }

    @Override
    protected void onResume() {
        super.onResume();
        int nRet = createSLEngine();
        MyLog.d(TAG, "createSLEngine nRet = " + nRet);
    }

    @Override
    protected void onPause() {
        super.onPause();
        // stop play
        setPlayingAssetAudioPlayerState(false);
    }

    @Override
    protected void onDestroy() {
        destroySLEngine();
        super.onDestroy();
    }

    public void OnClick (View view) {
        switch (view.getId()) {
            case R.id.btn_play_background_music:
                setAudioPlayerState();
                break;
            case R.id.btn_record_audio:
                setAudioRecordState();
                break;
            default:
                break;
        }
    }

    private void setAudioPlayerState() {
        bPlay = !bPlay;
        if (!bPlayerCreated) {
            String musicName = "background.mp3";
            createAssetAudioPlayer(this.getAssets(), musicName);
            bPlayerCreated = true;
        }
        setPlayingAssetAudioPlayerState(bPlay);
    }

    private void setAudioRecordState() {
        bPlay = !bPlay;
        if (!bPlayerCreated) {
            String musicName = "background.mp3";
            createAssetAudioPlayer(this.getAssets(), musicName);
            bPlayerCreated = true;
        }
        setPlayingAssetAudioPlayerState(bPlay);
    }

    public native int createSLEngine();
    public native boolean createAssetAudioPlayer(AssetManager assetManager, String fileName);
    public native void setPlayingAssetAudioPlayerState(boolean bPlay);
    public native void destroySLEngine();
}