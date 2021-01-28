//
// Created by chauncy on 2021/1/25.
//

#include "OboeHelper.h"
#include <LogAndroid.h>
#include <math.h>

#define CHECK_NULL(_p_)		if (!_p_) return;

#define CHECK_OBOE_ERROR(_ret_)		if (oboe::Result::OK != _ret_) {return;}


oboe::DataCallbackResult OboeHelper::AudioDataCallback::onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames)
{
	LOGD ("oboe::DataCallbackResult OboeHelper::AudioDataCallback::onAudioReady begin");
	auto *floatData = static_cast<float *>(audioData);
	float phase = 0.f;
	int channelCount = 0;
	if (m_pNativeAudioHelperHolder) {
		phase = m_pNativeAudioHelperHolder->m_fPhase;
		channelCount = m_pNativeAudioHelperHolder->m_nChannelCount;
	}
	const float amplitude = 0.5f;
	for (int i = 0; i < numFrames; ++i){
		float sampleValue = amplitude * sinf (phase);
		for (int j = 0; j < channelCount; ++j) {
			floatData[i * channelCount + j] = sampleValue;
		}
		phase += phase;
	}
	return oboe::DataCallbackResult::Continue;
}

OboeHelper::AudioDataCallback::AudioDataCallback(void *pAudioHelper)
{
	m_pNativeAudioHelperHolder = (OboeHelper*)pAudioHelper;
}

OboeHelper::AudioDataCallback::~AudioDataCallback()
{

}

OboeHelper::OboeHelper():
m_pAudioStreamBuilder(nullptr),
m_pAudioDataCallback(nullptr),
m_pAudioStream(nullptr),
m_fPhase(0),
m_nChannelCount(2)
{

}

OboeHelper::~OboeHelper()
{

}

void OboeHelper::startAudio()
{
	LOGD("OboeHelper::startAudio begin");
	if (!m_pAudioStreamBuilder) {
		m_pAudioStreamBuilder = new oboe::AudioStreamBuilder();
		CHECK_NULL(m_pAudioStreamBuilder)
	}

	m_pAudioDataCallback = new AudioDataCallback(this);
	auto pCallback = static_cast<oboe::AudioStreamCallback*>(m_pAudioDataCallback);

	std::lock_guard<std::mutex> lock (m_AudioLock);
	oboe::Result result = m_pAudioStreamBuilder->setPerformanceMode(oboe::PerformanceMode::LowLatency)
	->setSharingMode(oboe::SharingMode::Exclusive)
	->setFormat(oboe::AudioFormat::Float)
	->setSampleRate(44100)
	->setSampleRateConversionQuality(oboe::SampleRateConversionQuality::Medium)
	->setCallback(pCallback)
	->openStream(m_pAudioStream);
	LOGD("OboeHelper::startAudio openStream result = %d",result);
	CHECK_OBOE_ERROR(result)

	result = m_pAudioStream->requestStart();
	LOGD("OboeHelper::startAudio requestStart result = %d",result);
}

void OboeHelper::StopAudio()
{
	LOGD("OboeHelper::StopAudio");

	std::lock_guard<std::mutex> lock (m_AudioLock);
	if (m_pAudioStream) {
		m_pAudioStream->stop();
		m_pAudioStream->close();
		m_pAudioStream.reset();
	}
}
