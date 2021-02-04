//
// Created by wcg3031 on 2021/2/1.
//

#include <LogAndroid.h>
#include <MyDefineUtils.h>
#include <FileUtils.h>
#include "AAudioHelper.h"

#define MY_TIMEOUT_NANOS							100 * 1000000	// 100 ms

#define CHECK_AAUDIO_ERROR(_tag_, _ret_)			if (AAUDIO_OK != _ret_) {LOGE("%s error, _ret_ = %s", _tag_, AAudio_convertResultToText(_ret_)); return _ret_;}
#define CHECK_AAUDIO_ERROR_VOID(_tag_, _ret_)		if (AAUDIO_OK != _ret_) {LOGE("%s error, _ret_ = %s", _tag_, AAudio_convertResultToText(_ret_)); return;}
#define PRINTF_AAUDIO_STATE(_tag_, _stream_)		if (nullptr != _stream_) {LOGD("%s state: %s", _tag_, AAudio_convertStreamStateToText(AAudioStream_getState(_stream_)));}

aaudio_data_callback_result_t AAudioHelper::audioCallback(AAudioStream *stream, void *userData, void *audioData, int32_t numFrames)
{
	LOGD("audioCallback() begin numFrames = %d", numFrames);

	auto pHelper = static_cast<AAudioHelper*>(userData);
	if (nullptr == pHelper) {
		LOGE("audioCallback error");
		return -1;
	}
	aaudio_stream_state_t state = AAudioStream_getState(stream);
	if (AAUDIO_STREAM_STATE_STARTED != state) {
		LOGE("audioCallback state = %s", AAudio_convertStreamStateToText(state));
		return AAUDIO_CALLBACK_RESULT_CONTINUE;
	}

	if (nullptr != audioData && nullptr != pHelper->m_pData) {
		// todo：保存 pcm 数据，这里不应直接使用 write 函数，这种耗时操作容易阻塞这个高优先级线程，考虑在线程中做 IO 操作
		FileUtils::WriteDataWithFile(audioData, numFrames * pHelper->m_BytesPerData, pHelper->m_pFile);
	}

	return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

void AAudioHelper::errorCallback (AAudioStream *stream, void *userData, aaudio_result_t error)
{
	LOGD("AAudioHelper::errorCallback() error = %d", error);
	if (nullptr != stream) {
		aaudio_stream_state_t state = AAudioStream_getState(stream);
		if (AAUDIO_STREAM_STATE_DISCONNECTED == state) {
			LOGE("errorCallback error");
		}
	}
}

// singleton
AAudioHelper *AAudioHelper::getInstance()
{
	static AAudioHelper instance;
	return &instance;
}

void AAudioHelper::startRecording()
{
	LOGD("AAudioHelper::startRecording() begin");
	AAudioStreamBuilder* pStreamBuilder;
	AAudio_createStreamBuilder(&pStreamBuilder);
	if (nullptr == pStreamBuilder) {
		LOGE("AAudioHelper::startRecording pStreamBuilder is nullptr");
		return;
	}

	// use default device
	AAudioStreamBuilder_setDeviceId(pStreamBuilder, AAUDIO_UNSPECIFIED);
	AAudioStreamBuilder_setFormat(pStreamBuilder, AAUDIO_FORMAT_PCM_I16);
	AAudioStreamBuilder_setChannelCount(pStreamBuilder, 1);
	AAudioStreamBuilder_setSampleRate(pStreamBuilder, 44100);
	AAudioStreamBuilder_setPerformanceMode(pStreamBuilder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
	AAudioStreamBuilder_setDirection(pStreamBuilder, AAUDIO_DIRECTION_INPUT);
	AAudioStreamBuilder_setDataCallback(pStreamBuilder, audioCallback, this);
	AAudioStreamBuilder_setErrorCallback(pStreamBuilder, errorCallback, this);

	aaudio_result_t result;
	result = AAudioStreamBuilder_openStream (pStreamBuilder, &m_pRecordingStream);
	//CHECK_AAUDIO_ERROR_VOID("AAudioHelper::startRecording AAudioStream_requestStart", result)
	CHECK_AAUDIO_ERROR_VOID("AAudioHelper::startRecording AAudioStreamBuilder_openStream", result)
	if (nullptr == m_pRecordingStream) {
		LOGE("AAudioHelper::startRecording m_pRecordingStream is nullptr");
		return;
	}
	PRINTF_AAUDIO_STATE("AAudioHelper::startRecording AAudioStreamBuilder_openStream", m_pRecordingStream)

	int32_t framesPerBurst = AAudioStream_getFramesPerBurst(m_pRecordingStream);
	int32_t bufferSize = AAudioStream_getBufferSizeInFrames(m_pRecordingStream);
	int32_t bufferCapacity = AAudioStream_getBufferCapacityInFrames(m_pRecordingStream);
	int32_t sampleRate = AAudioStream_getSampleRate(m_pRecordingStream);
	LOGD("AAudioHelper::startRecording framesPerBurst = %d, bufferSize = %d, bufferCapacity = %d, sampleRate = %d",
			framesPerBurst, bufferSize, bufferCapacity, sampleRate);
	int32_t channelCount = AAudioStream_getChannelCount(m_pRecordingStream);
	aaudio_format_t format = AAudioStream_getFormat(m_pRecordingStream);
	if (AAUDIO_FORMAT_PCM_I16 == format) {
		m_BytesPerData = 2;
	} else if (AAUDIO_FORMAT_PCM_FLOAT == format) {
		m_BytesPerData = 4;
	}
	/*m_BufferSize = bytesPerData * channelCount * sampleRate;
	// malloc for audio buffer data
	m_pData = (unsigned char*) malloc(m_BufferSize);
	if (nullptr == m_pData) {
		LOGE("AAudioHelper::startRecording m_pData is nullptr");
		return;
	}
	memset(m_pData, 0, m_BufferSize);*/
	char path [MAX_PATH * 2] {0};
	sprintf(path, "/sdcard/AudioVideoTest/audio_aaudio_%lld.pcm", MyTimeUtils::getCurrentTime());
	if (nullptr == m_pFile) {
		m_pFile = fopen(path, "ab+");
		if (nullptr == m_pFile) {
			LOGE("AAudioHelper::startRecording m_pFile is nullptr");
			return;
		}
	}

	result = AAudioStream_requestStart(m_pRecordingStream);
	CHECK_AAUDIO_ERROR_VOID("AAudioHelper::startRecording AAudioStream_requestStart", result)
	PRINTF_AAUDIO_STATE("AAudioHelper::startRecording AAudioStream_requestStart", m_pRecordingStream)

	aaudio_stream_state_t currentState = AAudioStream_getState(m_pRecordingStream);
	aaudio_stream_state_t nextState;
	result = AAudioStream_waitForStateChange(m_pRecordingStream, currentState, &nextState, MY_TIMEOUT_NANOS);
	CHECK_AAUDIO_ERROR_VOID("AAudioHelper::startRecording AAudioStream_waitForStateChange", result)

	result = AAudioStreamBuilder_delete(pStreamBuilder);
	CHECK_AAUDIO_ERROR_VOID("AAudioHelper::startRecording AAudioStreamBuilder_delete", result)
	pStreamBuilder  = nullptr;
}

void AAudioHelper::stopRecording()
{
	LOGD("AAudioHelper::stopRecording() begin");
	aaudio_result_t result;
	if (nullptr != m_pRecordingStream) {
		result = AAudioStream_requestStop (m_pRecordingStream);
		CHECK_AAUDIO_ERROR_VOID(AAudio_convertResultToText(result), result)
		result = AAudioStream_close(m_pRecordingStream);
		CHECK_AAUDIO_ERROR_VOID(AAudio_convertResultToText(result), result)
		m_pRecordingStream = nullptr;
	}
	if (nullptr != m_pFile) {
		fclose(m_pFile);
		m_pFile = nullptr;
	}
	SafeFree(m_pData)
}

AAudioHelper::AAudioHelper():
		m_pRecordingStream(nullptr),
		m_pFile(nullptr),
		m_pData(nullptr),
		m_BufferSize(0),
		m_BytesPerData(0)
{
	LOGD("AAudioHelper::AAudioHelper()");
}

AAudioHelper::~AAudioHelper()
= default;

void AAudioHelper::drainRecordingStream(AAudioStream* stream, void *audioData, int32_t numFrames)
{
	LOGD("AAudioHelper::drainRecordingStream() begin");
	aaudio_result_t clearFrames = 0;
	do {
		if (nullptr != stream) {
			clearFrames = AAudioStream_read (stream, audioData, numFrames, 0);
		}
	} while(clearFrames > 0);
}
