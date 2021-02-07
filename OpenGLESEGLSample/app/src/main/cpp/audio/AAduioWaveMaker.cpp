//
// Created by chauncy on 2021/2/3.
//

#include <cmath>
#include <LogAndroid.h>
#include <thread>
#include "AAduioWaveMaker.h"

#define CHECK_WAVE_MAKER_ERROR_BOOL(_tag_, _ret_)		if (AAUDIO_OK != _ret_) {LOGE("%s error, _ret_ = %s", _tag_, AAudio_convertResultToText(_ret_)); return false;}
#define CHECK_WAVE_MAKER_ERROR_VOID(_tag_, _ret_)		if (AAUDIO_OK != _ret_) {LOGE("%s error, _ret_ = %s", _tag_, AAudio_convertResultToText(_ret_)); return;}
#define PRINTF_WAVE_MAKER_STATE(_tag_, _stream_)		if (nullptr != _stream_) {LOGD("%s state: %s", _tag_, AAudio_convertStreamStateToText(AAudioStream_getState(_stream_)));}

// 将缓冲区大小设置为数据量的两倍，用这种双缓冲的方式可以起到有效避免数据溢出，去除毛刺，降低延迟的作用
constexpr int32_t kBufferSizeInBursts = 2;

AAduioWaveMaker *AAduioWaveMaker::getInstance()
{
	static AAduioWaveMaker aaudioWaveMaker;
	return &aaudioWaveMaker;
}


bool AAduioWaveMaker::start()
{
	LOGD("AAduioWaveMaker::start begin");

	AAudioStreamBuilder* pStreamBuilder = nullptr;
	aaudio_result_t result = AAudio_createStreamBuilder(&pStreamBuilder);
	CHECK_WAVE_MAKER_ERROR_BOOL("AAduioWaveMaker::start AAudio_createStreamBuilder", result)

	AAudioStreamBuilder_setFormat(pStreamBuilder, AAUDIO_FORMAT_PCM_FLOAT);
	AAudioStreamBuilder_setChannelCount(pStreamBuilder, 1);
	AAudioStreamBuilder_setPerformanceMode(pStreamBuilder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
	AAudioStreamBuilder_setDataCallback(pStreamBuilder, AAduioWaveMaker::dataCallback, this);
	AAudioStreamBuilder_setErrorCallback(pStreamBuilder, AAduioWaveMaker::errorCallback, this);

	// 打开 stream
	result = AAudioStreamBuilder_openStream(pStreamBuilder, &m_pPlayingStream);
	if (AAUDIO_OK != result || nullptr == m_pPlayingStream) {
		LOGE("AAduioWaveMaker::start AAudioStreamBuilder_openStream error");
		return false;
	}

	if (nullptr == m_pOscillator) {
		return false;
	}
	// 给振荡器设置频率
	int32_t sampleRate = AAudioStream_getSampleRate (m_pPlayingStream);
	m_pOscillator->setSampleRate(sampleRate);

	// 设置 buffer 大小
	AAudioStream_setBufferSizeInFrames (m_pPlayingStream, AAudioStream_getFramesPerBurst(m_pPlayingStream) * kBufferSizeInBursts);

	// start stream
	result = AAudioStream_requestStart (m_pPlayingStream);
	CHECK_WAVE_MAKER_ERROR_BOOL("AAduioWaveMaker::start AAudioStream_requestStart", result)

	AAudioStreamBuilder_delete(pStreamBuilder);
	pStreamBuilder = nullptr;

	return true;
}

void AAduioWaveMaker::stop()
{
	LOGD("AAduioWaveMaker::stop begin");
	aaudio_result_t result;
	if (nullptr != m_pPlayingStream) {
		result = AAudioStream_requestStop (m_pPlayingStream);
		CHECK_WAVE_MAKER_ERROR_VOID("AAduioWaveMaker::stop AAudioStream_requestStop", result)
		result = AAudioStream_close (m_pPlayingStream);
		CHECK_WAVE_MAKER_ERROR_VOID("AAduioWaveMaker::stop AAudioStream_close", result)
	}
}

void AAduioWaveMaker::restart()
{
	LOGD("AAduioWaveMaker::restart begin");
	static std::lock_guard<std::mutex> lock(m_RestartLock);
	stop();
	start();
}

void AAduioWaveMaker::setToneOn(bool isToneOn)
{
	LOGD("AAduioWaveMaker::setToneOn begin isToneOn = %d", isToneOn);
	if (nullptr != m_pOscillator) {
		m_pOscillator->setWaveOn(isToneOn);
	}
}

AAduioWaveMaker::AAduioWaveMaker():
		m_pOscillator(nullptr),
		m_pPlayingStream(nullptr)
{
	LOGD("AAduioWaveMaker::AAduioWaveMaker()");
	m_pOscillator = new Oscillator();
}

AAduioWaveMaker::~AAduioWaveMaker()
{
	if (nullptr != m_pOscillator) {
		delete m_pOscillator;
		m_pOscillator = nullptr;
	}
}

aaudio_data_callback_result_t AAduioWaveMaker::dataCallback(AAudioStream *stream, void *userData, void *audioData, int32_t numFrames)
{
	LOGD("AAduioWaveMaker::dataCallback begin");
	auto pWaveMaker = static_cast<AAduioWaveMaker*>(userData);
	if (nullptr == pWaveMaker) {
		LOGE("AAduioWaveMaker::dataCallback pWaveMaker is nullptr");
		return -1;
	}
	if (nullptr != pWaveMaker->m_pOscillator){
		pWaveMaker->m_pOscillator->render(static_cast<float*>(audioData), numFrames);
	}
	pWaveMaker->m_pOscillator->render(static_cast<float*>(audioData), numFrames);

	return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

void AAduioWaveMaker::errorCallback (AAudioStream *stream, void *userData, aaudio_result_t error)
{
	LOGD("AAduioWaveMaker::errorCallback() error = %d", error);
	if (error == AAUDIO_ERROR_DISCONNECTED) {
		std::function<void(void)> restartFunction = std::bind(&AAduioWaveMaker::restart, static_cast<AAduioWaveMaker*>(userData));
		new std::thread(restartFunction);
	}
}


#define TWO_PI (3.14159 * 2)
#define AMPLITUDE 0.3
#define FREQUENCY 440.0
void AAduioWaveMaker::Oscillator::setWaveOn(bool isWaveOn)
{
	m_bWaveOn.store(isWaveOn);
}

void AAduioWaveMaker::Oscillator::setSampleRate(int32_t sampleRate)
{
	m_dPhaseIncrement = (TWO_PI * FREQUENCY) / (double) sampleRate;
}

/** 模拟 pcm 音频数据，塞入 audioData 中，用于写入 AAudioStream 播放 */
void AAduioWaveMaker::Oscillator::render(float *audioData, int32_t numFrames)
{
	if (!m_bWaveOn.load()) m_dPhase = 0;
	for (int i = 0; i < numFrames; i++) {

		if (m_bWaveOn.load()) {

			// 单声道，Calculates the next sample value for the sine wave.
			audioData[i] = (float) (sin(m_dPhase) * AMPLITUDE);

			// Increments the phase, handling wrap around.
			m_dPhase += m_dPhaseIncrement;
			if (m_dPhase > TWO_PI) m_dPhase -= TWO_PI;

		} else {
			// Outputs silence by setting sample value to zero.
			audioData[i] = 0;
		}
	}
}
