//
// Created by chauncy on 2021/1/25.
//

#pragma once

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

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
} FdAudioRecorder, *LPFdAudioRecorder;

class OpenSLESHelper
{
public:
	OpenSLESHelper();
	~OpenSLESHelper();
	int createSLEngine();
	int createSLPlayerWithAssets(JNIEnv* env, jobject asset_manager, const char* path);
	void setSLPlayerState(bool bPlay);
	int createSLRecorder();
	void destroySLEngine();

private:
	SLObjectItf 							m_EngineObject;
	SLEngineItf 							m_EngineEngine;

	SLObjectItf 							m_OutputMixObject;
	SLEnvironmentalReverbItf				m_OutputMixEnvironmentalReverb;

	FdAudioPlayer 							m_AudioPlayer;
	FdAudioRecorder							m_AudioRecorder;

	SLEnvironmentalReverbSettings			ReverbSettings;
};



