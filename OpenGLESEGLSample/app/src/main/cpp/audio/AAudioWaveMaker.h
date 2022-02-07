//
// Created by chauncy on 2021/2/3.
//

#pragma once


#include <cstdint>
#include <atomic>
#include <aaudio/AAudio.h>
#include <mutex>

class AAudioWaveMaker
{
	class Oscillator;

public:
	static AAudioWaveMaker* getInstance();
	bool start();
	void stop();
	void restart();
	void setToneOn(bool isToneOn);

private:
	AAudioWaveMaker();
	~AAudioWaveMaker();
	static aaudio_data_callback_result_t dataCallback(AAudioStream* stream, void* userData, void* audioData, int32_t numFrames);
	static void errorCallback (AAudioStream *stream, void *userData, aaudio_result_t error);


	Oscillator* 	m_pOscillator;
	AAudioStream* 	m_pPlayingStream;
	std::mutex 		m_RestartLock;

	// inner class
	class Oscillator {
	public:
		void setWaveOn(bool isWaveOn);
		void setSampleRate(int32_t sampleRate);
		void render(float* audioData, int32_t numFrames);

	private:
		std::atomic<bool> m_bWaveOn;
		double m_dPhase;
		double m_dPhaseIncrement;
	};
};



