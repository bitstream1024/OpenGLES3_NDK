package com.example.opengleseglsample;

import android.annotation.SuppressLint;
import android.content.Context;
import android.media.AudioDeviceCallback;
import android.media.AudioDeviceInfo;
import android.media.AudioManager;
import android.os.Bundle;
import android.view.View;

import com.example.utils.DebugLog;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

public class AAudioActivity extends AppCompatActivity {

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
        setContentView(R.layout.activity_aaudio);
    }

    @Override
    protected void onResume() {
        super.onResume();
        AudioManager audioManager = (AudioManager) this.getSystemService(Context.AUDIO_SERVICE);
        AudioDeviceInfo[] deviceInfoList = null;
        if (null != audioManager) {
            deviceInfoList = audioManager.getDevices(AudioManager.GET_DEVICES_INPUTS);
            if (null != deviceInfoList) {
                for (int i = 0; i < deviceInfoList.length; i++) {
                    DebugLog.d(TAG, "deviceInfoList device id = " + deviceInfoList[i].getId() + ", device: " + deviceInfoList[i].getProductName());
                }
            }
            audioManager.registerAudioDeviceCallback(new AudioDeviceCallback() {
                @Override
                public void onAudioDevicesAdded(AudioDeviceInfo[] addedDevices) {
                    super.onAudioDevicesAdded(addedDevices);
                    for (int i = 0; i < addedDevices.length; i++) {
                        DebugLog.d(TAG, "addedDevices device id = " + addedDevices[i].getId() + ", device: " + addedDevices[i].getProductName());
                    }
                }

                @Override
                public void onAudioDevicesRemoved(AudioDeviceInfo[] removedDevices) {
                    super.onAudioDevicesRemoved(removedDevices);
                }
            }, null);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @SuppressLint("NonConstantResourceId")
    public void OnClick (View view) {
        switch (view.getId()) {
            case R.id.btn_aaudio_recording:
                setAudioRecordState();
                break;
            /*case R.id.btn_record_audio:
                setAudioRecordState();
                break;
            case R.id.btn_play_pcm:
                setPcmPlayerState();
                break;*/
            default:
                break;
        }
    }

    private void setAudioRecordState() {
        bPlaying = !bPlaying;
        nativeSetRecordingState(bPlaying);
    }

    public native void nativeSetRecordingState(boolean bRecording);
}