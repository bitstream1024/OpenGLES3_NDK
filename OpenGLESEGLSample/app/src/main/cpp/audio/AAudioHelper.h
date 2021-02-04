//
// Created by chauncy on 2021/2/1.
//

#pragma once

#include <aaudio/AAudio.h>

class AAudioHelper
{
public:
	static AAudioHelper* getInstance();
	void startRecording();
	void stopRecording();
	bool isFirstDataCallback();
	void updateFirstDataCallbackState(bool bFirst);

private:
	AAudioHelper();
	~AAudioHelper();
	static aaudio_data_callback_result_t audioCallback(AAudioStream *stream, void *userData, void *audioData, int32_t numFrames);
	static void errorCallback (AAudioStream *stream, void *userData, aaudio_result_t error);
	void drainRecordingStream(AAudioStream* stream, void *audioData, int32_t numFrames);

	//AAudioStreamBuilder* 	m_pRecordingStreamBuilder;
	AAudioStream* 		 	m_pRecordingStream;
	bool 					m_bFirstDataCallback;
};



