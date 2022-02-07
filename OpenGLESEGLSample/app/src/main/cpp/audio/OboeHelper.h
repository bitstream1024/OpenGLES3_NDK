//
// Created by chauncy on 2021/1/25.
//

#pragma once

#include <oboe/Oboe.h>

class OboeHelper
{
private:
	class AudioDataCallback : public oboe::AudioStreamCallback {
	public:
		AudioDataCallback(void *pAudioHelper);
		~AudioDataCallback();
		// override
		oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames);

	private:
		OboeHelper*		m_pNativeAudioHelperHolder;
	};


public:
	static OboeHelper* getInstance();

	void startRecording();
	void stopRecording();
	void release();

private:
	OboeHelper();
	~OboeHelper();

	oboe::AudioStreamBuilder 				*m_pAudioStreamBuilder;
	AudioDataCallback 						*m_pAudioDataCallback;
	oboe::AudioStream* 						m_pAudioStream;
	std::mutex								m_AudioLock;

	// audio parameters
	float 									m_fPhase;
	int 									m_nChannelCount;
	FILE*									m_pFile;
};



