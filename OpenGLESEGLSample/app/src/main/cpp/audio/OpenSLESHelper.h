//
// Created by chauncy on 2021/1/25.
//

#pragma once

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <mutex>

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
	// recording buffer size
	unsigned long 					lBufferSize;
	// two buffers to save audio data one by one，like double PBO in OpenGL
	unsigned char*					pRecordBufferArray[2];
	unsigned char 					nBufferIndex;
	bool 							bRecording;
	std::string 					filePath;
	FILE 							*pFile;
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
	// OpenSL ES audio engine
	SLObjectItf 							m_EngineObject;
	SLEngineItf 							m_EngineEngine;

	SLObjectItf 							m_OutputMixObject;
	SLEnvironmentalReverbItf				m_OutputMixEnvironmentalReverb;

	FdAudioPlayer 							m_AudioPlayer;
	FdAudioRecorder							m_AudioRecorder;

	SLEnvironmentalReverbSettings			ReverbSettings;
};



