//
// Created by wcg3031 on 2021/2/1.
//

#include <LogAndroid.h>
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
	if (pHelper->m_bFirstDataCallback) {
		pHelper->drainRecordingStream(stream, audioData, numFrames);
	}
	pHelper->m_bFirstDataCallback = false;

	aaudio_result_t result = AAUDIO_OK;
	aaudio_stream_state_t state = AAudioStream_getState(stream);
	if (AAUDIO_STREAM_STATE_STARTED != state) {
		LOGE("audioCallback state = %s", AAudio_convertStreamStateToText(state));
	}

	/*int64_t timeoutNanos = 100 * 1000000;
	result = AAudioStream_read(stream, audioData, numFrames, timeoutNanos);
	if (result < 0) {
		CHECK_AAUDIO_ERROR("audioCallback AAudioStream_read", result);
	}*/

	if (result != numFrames) {
		// 如果读取到的帧数和实际音频帧数不一致，需要将剩下的部分音频置为 0，避免杂音干扰
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

	//AAudioStreamBuilder_setDeviceId(pStreamBuilder, 2);
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
}

AAudioHelper::AAudioHelper():
		m_pRecordingStream(nullptr),
		m_bFirstDataCallback(true)
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

bool AAudioHelper::isFirstDataCallback()
{
	return this->m_bFirstDataCallback;
}

void AAudioHelper::updateFirstDataCallbackState(bool bFirst)
{
	this->m_bFirstDataCallback = bFirst;
}
