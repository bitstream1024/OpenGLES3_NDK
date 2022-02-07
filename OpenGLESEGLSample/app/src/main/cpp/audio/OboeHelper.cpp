//
// Created by chauncy on 2021/1/25.
//

#include "OboeHelper.h"
#include <LogAndroid.h>
#include <math.h>
#include <MyDefineUtils.h>
#include <FileUtils.h>

#define CHECK_NULL(_p_)		if (!_p_) return;

#define CHECK_OBOE_ERROR(_ret_)		if (oboe::Result::OK != _ret_) {return;}

OboeHelper::AudioDataCallback::AudioDataCallback(void *pAudioHelper)
{
	m_pNativeAudioHelperHolder = (OboeHelper*)pAudioHelper;
}

/*oboe::DataCallbackResult OboeHelper::AudioDataCallback::onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames)
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
}*/

// for recording callback
oboe::DataCallbackResult OboeHelper::AudioDataCallback::onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames)
{
	LOGD("OboeHelper::AudioDataCallback::onAudioReady begin audioData = %p, numFrames = %d", audioData, numFrames);
	// 保存 pcm 数据
	if (nullptr == oboeStream) {
		return oboe::DataCallbackResult::Stop;
	}

	int bufferLen = 2 * numFrames;
	if (nullptr != m_pNativeAudioHelperHolder && nullptr != m_pNativeAudioHelperHolder->m_pFile) {
		FileUtils::WriteDataWithFile(audioData, bufferLen, m_pNativeAudioHelperHolder->m_pFile);
	}

	return oboe::DataCallbackResult::Continue;
}

OboeHelper::AudioDataCallback::~AudioDataCallback()
{

}

OboeHelper *OboeHelper::getInstance()
{
	static OboeHelper instance;
	return &instance;
}


OboeHelper::OboeHelper():
m_pAudioStreamBuilder(nullptr),
m_pAudioDataCallback(nullptr),
m_pAudioStream(nullptr),
m_fPhase(0),
m_nChannelCount(1),
m_pFile(nullptr)
{
	LOGD("OboeHelper::OboeHelper() begin");
}

OboeHelper::~OboeHelper()
{
	LOGD("OboeHelper::~OboeHelper() begin");
}

void OboeHelper::startRecording()
{
	LOGD("OboeHelper::startRecording begin");

	m_pAudioDataCallback = new AudioDataCallback(this);
	auto pCallback = static_cast<oboe::AudioStreamCallback*>(m_pAudioDataCallback);

	// 创建音频流 builder
	auto* pStreamBuilder = new oboe::AudioStreamBuilder();

	std::lock_guard<std::mutex> lock (m_AudioLock);
	oboe::Result result = pStreamBuilder->setDirection(oboe::Direction::Input)
			->setPerformanceMode(oboe::PerformanceMode::LowLatency)
			->setSharingMode(oboe::SharingMode::Exclusive)
			->setFormat(oboe::AudioFormat::I16)
			->setChannelCount(oboe::ChannelCount::Mono)
			->setSampleRate(44100)
			->setSampleRateConversionQuality(oboe::SampleRateConversionQuality::Medium)
			->setCallback(pCallback)
			->openStream(reinterpret_cast<oboe::AudioStream **>(&m_pAudioStream));
	LOGD("OboeHelper::startRecording openStream result = %d",result);
	CHECK_OBOE_ERROR(result)

	char pcmPath[MAX_PATH] {0};
	sprintf (pcmPath, "/sdcard/AudioVideoTest/audio_oboe_%lld.pcm", MyTimeUtils::getCurrentTime());
	m_pFile = fopen (pcmPath, "ab+");
	if (nullptr == m_pFile) {
		LOGE("OboeHelper::startRecording m_pFile is nullptr");
		return;
	}

	result = m_pAudioStream->requestStart();
	LOGD("OboeHelper::startRecording requestStart result = %d",result);
}

void OboeHelper::stopRecording()
{
	LOGD("OboeHelper::StopAudio");

	std::lock_guard<std::mutex> lock (m_AudioLock);

	if (nullptr != m_pFile) {
		fclose(m_pFile);
		m_pFile = nullptr;
	}

	if (m_pAudioStream) {
		m_pAudioStream->stop();
		m_pAudioStream->close();
		SafeDelete(m_pAudioStream)
	}
}