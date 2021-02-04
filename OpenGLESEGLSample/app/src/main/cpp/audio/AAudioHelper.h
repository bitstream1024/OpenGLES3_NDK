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

private:
	AAudioHelper();
	~AAudioHelper();
	static aaudio_data_callback_result_t audioCallback(AAudioStream *stream, void *userData, void *audioData, int32_t numFrames);
	static void errorCallback (AAudioStream *stream, void *userData, aaudio_result_t error);
	void drainRecordingStream(AAudioStream* stream, void *audioData, int32_t numFrames);

	AAudioStream* 		 			m_pRecordingStream;
	FILE*							m_pFile;
	unsigned char*					m_pData;
	size_t 							m_BufferSize;
	unsigned int 					m_BytesPerData;
};



