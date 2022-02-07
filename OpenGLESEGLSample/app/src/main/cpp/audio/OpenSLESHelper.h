//
// Created by chauncy on 2021/1/25.
//

#pragma once

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <mutex>

typedef struct _tag_sl_audio_player
{
	SLObjectItf 					fdPlayerObject;
	SLPlayItf 						fdPlayerPlay;
	SLSeekItf 						fdPlayerSeek;
	SLMuteSoloItf 					fdPlayerMuteSolo;
	SLVolumeItf 					fdPlayerVolume;

	// for pcm data audio player
	SLAndroidSimpleBufferQueueItf	fdSimpleBufferQueue;
	unsigned long 					lBufferSize;
	unsigned char*					pPlayerBufferArray[2];
	unsigned char 					nBufferIndex;
	bool 							bPlaying;
	FILE*							pFile;
} FdAudioPlayer, *LPFdAudioPlayer;

typedef struct _tag_sl_audio_recorder
{
	SLObjectItf 					fdRecorderObject;
	SLRecordItf 					fdRecorderRecord;
	SLAndroidSimpleBufferQueueItf 	fdRecorderBufferQueue;
	// recording buffer size
	unsigned long 					lBufferSize;
	// two buffers to save audio data one by one，like double PBO in OpenGL
	unsigned char*					pRecordBufferArray[2];
	unsigned char 					nBufferIndex;
	bool 							bRecording;
	FILE* 							pFile;
} FdAudioRecorder, *LPFdAudioRecorder;

class OpenSLESHelper
{
public:
	OpenSLESHelper();
	~OpenSLESHelper();

	int createSLEngine();
	void destroySLEngine();

	int createSLPlayerWithAssets(JNIEnv* env, jobject asset_manager, const char* path);
	void setSLPlayerState(bool bPlay);
	void destroySLPlayer ();

	int createSLRecorder();
	FdAudioRecorder* getFdAudioRecorder();
	void startRecording();
	void stopRecording();
	void destroySLRecorder();

	void createPcmPlayer();
	FdAudioPlayer* getFdPcmPlayer();
	void startPlayPcmData(std::string pcmPath);
	void stopPcmPlayer();
	void destroyPcmPlayer();

	std::mutex								m_SLAudioRecorderLock;
	std::mutex								m_SLAudioPlayerLock;

private:
	// OpenSL ES audio object and engine
	SLObjectItf 							m_EngineObject;
	SLEngineItf 							m_EngineEngine;

	SLObjectItf 							m_OutputMixObject;
	SLEnvironmentalReverbItf				m_OutputMixEnvironmentalReverb;

	FdAudioPlayer 							m_AssetsAudioPlayer;
	FdAudioPlayer 							m_PcmAudioPlayer;
	FdAudioRecorder							m_AudioRecorder;

	SLEnvironmentalReverbSettings			ReverbSettings;

	// define audio format
	SLDataFormat_PCM						m_PcmFormat;
};



