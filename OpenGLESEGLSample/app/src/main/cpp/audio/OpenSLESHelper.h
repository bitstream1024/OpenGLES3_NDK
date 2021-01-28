//
// Created by chauncy on 2021/1/25.
//

#pragma once

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <mutex>

#define RECORDING_SAMPLE_RATE		44100
#define RECORDING_CHANNELS			1
#define RECORDING_DATA_BIT			16
#define RECORDING_EACH_SAMPLE_TIME	0.1f

typedef struct _tag_sl_audio_player
{
	SLObjectItf 	fdPlayerObject;
	SLPlayItf 		fdPlayerPlay;
	SLSeekItf 		fdPlayerSeek;
	SLMuteSoloItf 	fdPlayerMuteSolo;
	SLVolumeItf 	fdPlayerVolume;
} FdAudioPlayer, *LPFdAudioPlayer;

typedef struct _tag_sl_audio_recorder
{
	SLObjectItf 					fdRecorderObject;
	SLRecordItf 					fdRecorderRecord;
	SLAndroidSimpleBufferQueueItf 	fdRecorderBufferQueue;
	unsigned long 					lBufferSize;
	unsigned char*					pRecordBuffer;
	bool 							bRecording;
	std::string 					filePath;
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
	std::mutex								m_SLAudioRecordLock;

private:
	SLObjectItf 							m_EngineObject;
	SLEngineItf 							m_EngineEngine;

	SLObjectItf 							m_OutputMixObject;
	SLEnvironmentalReverbItf				m_OutputMixEnvironmentalReverb;

	FdAudioPlayer 							m_AudioPlayer;
	FdAudioRecorder							m_AudioRecorder;

	SLEnvironmentalReverbSettings			ReverbSettings;
};



