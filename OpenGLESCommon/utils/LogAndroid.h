//
// Created by wcg3031 on 2020/4/1.
//

#pragma once

#include "android/log.h"
#include "sys/time.h"

#define LOG_TAG	"MY_LOG"

#define OUPUT_MY_LOG

class MyTimeUtils;

#ifdef OUPUT_MY_LOG
#define LOGE(...)  __android_log_print (ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...)  __android_log_print (ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGV(...)  __android_log_print (ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGI(...)  __android_log_print (ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

#define START_TIME(_FUN_)		{ \
									long long t0 = MyTimeUtils::getCurrentTime(); \
									LOGD ("%s start", (_FUN_)); \

#define STOP_TIME(_FUN_)		\
									long long t1 = MyTimeUtils::getCurrentTime(); \
									LOGD ("%s stop time %lld ms", (_FUN_), (t1 -  t0)); \
								}

#define CAL_TIME_COST(_FUN_) 	MyAutoTimeUtils _time_print1_ ((_FUN_));


#else
#define LOGE(...)
#define LOGD(...)
#define LOGV(...)
#define LOGI(...)
#define START_TIME(_FUN_)
#define STOP_TIME(_FUN_)
#endif

class MyTimeUtils
{
public:
	static long long getCurrentTime ()
	{
		struct timeval time;
		gettimeofday(&time, NULL);
		long long curTime = (long)(time.tv_sec*1000 + time.tv_usec/1000);
		return curTime;
	}
};

class MyAutoTimeUtils
{
public:
	MyAutoTimeUtils (const char* sFun) : mFunName(NULL)
	{
		if (sFun)
		{
			mFunName = (char*)sFun;
			mStartTime = MyTimeUtils::getCurrentTime();
		}
	}

	~MyAutoTimeUtils()
	{
		if (mFunName)
		{
			long long deltaTime = MyTimeUtils::getCurrentTime() - mStartTime;
			LOGD("%s const %lld ms", mFunName, deltaTime);
		}
	}

private:
	long long mStartTime;
	char *mFunName;
};


