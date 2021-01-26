//
// Created by chauncy on 2021/1/25.
//

#pragma once

#include <oboe/Oboe.h>
#include <oboe/AudioStreamCallback.h>

class NativeAudioHelper
{
private:
	class AudioDataCallback : public oboe::AudioStreamCallback {
	public:
		AudioDataCallback(void *pAudioHelper);
		~AudioDataCallback();
		// override
		oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames);

	private:
		NativeAudioHelper*		m_pNativeAudioHelperHolder;
	};


public:
	NativeAudioHelper();
	~NativeAudioHelper();

	void startAudio();
	void StopAudio();

private:
	oboe::AudioStreamBuilder 				*m_pAudioStreamBuilder;
	AudioDataCallback 						*m_pAudioDataCallback;
	std::shared_ptr<oboe::AudioStream> 		m_pAudioStream;
	std::mutex								m_AudioLock;

	// audio parameters
	float 									m_fPhase;
	int 									m_nChannelCount;
};



