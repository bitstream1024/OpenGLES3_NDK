//
// Created by chauncy on 2021/2/4.
//

/**A circle queue for audio data, it can be modified to a common circle queue for other project
 * author: chauncy
 * time: 2020/2/4
 */

#pragma once

#include <memory>
#include <mutex>
#include <condition_variable>
#include <string>

#define MAX_BUFFER_QUEUE_LENGTH		5
#define IF_DEBUG_LOG				1

#define LOG_QUEUE(_tag_, _info_)	if (0 != IF_DEBUG_LOG) {LOGE("%s, %s", _tag_, _info_);}


/**
 * 用于缓存音频数据的 buffer 队列，可以作为通用队列使用，线程安全，C++11
 */
struct LocalAAudioBuffer {

public:
	/**
	 * This function is constructor function, set default values
	 * @param eachBufferSize: max size of each buffer
	 * @param queueBufferLength: num of queue buffer, if not set, will use MAX_BUFFER_QUEUE_LENGTH
	 */
	LocalAAudioBuffer(const unsigned int eachBufferSize, const unsigned int queueBufferLength = MAX_BUFFER_QUEUE_LENGTH)
	{
		std::string TAG = "LocalAAudioBuffer";
		char info[200] {0};
		sprintf (info, "eachBufferSize = %d, queueBufferLength = %d", eachBufferSize, queueBufferLength);
		LOG_QUEUE(TAG.c_str(), info);
		bQueueReady = false;
		if (eachBufferSize <= 0) {
			LOG_QUEUE(TAG.c_str(), "eachBufferSize error")
			return;
		}
		EACH_BUFFER_MAX_SIZE = eachBufferSize;
		if (queueBufferLength > MAX_BUFFER_QUEUE_LENGTH) {
			LOG_QUEUE(TAG.c_str(), "QUEUE_BUFFER_LENGTH too large, use MAX_BUFFER_QUEUE_LENGTH")
			QUEUE_BUFFER_LENGTH = MAX_BUFFER_QUEUE_LENGTH;
		} else {
			QUEUE_BUFFER_LENGTH = queueBufferLength;
		}
		for (int i = 0; i < QUEUE_BUFFER_LENGTH; ++i) {
			pBufferArray[i] = (unsigned char*)malloc(EACH_BUFFER_MAX_SIZE);
			if (nullptr == pBufferArray[i]) {
				LOG_QUEUE(TAG.c_str(), "malloc failed");
				return;
			}
			memset(pBufferArray[i], 0, sizeof(EACH_BUFFER_MAX_SIZE));
			mBufferArrayLength[i] = 0;
		}
		beginIndex = 0;
		endIndex = 0;
		mQueueBufferNum = 0;
		bQueueReady = true;
		lostNum = 0;
	}

	~LocalAAudioBuffer()
	{
		LOGD("~LocalAAudioBuffer()");
		std::lock_guard<std::mutex> lock(mMutex);
		clear();
	}

	// 与 getBufferData 配合使用，可以同一线程也可以非同一线程，队列满了直接返回
	void setBufferData(void* const pSrcData, const unsigned int &length = 0)
	{
		std::lock_guard<std::mutex> lock (mMutex);
		if (isFull()) {
			++lostNum;
			LOG_QUEUE("setBufferData", "queue is full")
			return;
		}
		// 拷贝前先将原 buffer 数据置为 0，避免干扰
		memset (pBufferArray[endIndex], 0, mQueueBufferNum);
		if (0 == length) {
			memcpy (pBufferArray[endIndex], pSrcData, EACH_BUFFER_MAX_SIZE);
			mBufferArrayLength[endIndex] = EACH_BUFFER_MAX_SIZE;
		} else {
			memcpy (pBufferArray[endIndex], pSrcData, length);
			mBufferArrayLength[endIndex] = length;
		}
		endIndex = (endIndex + 1) % QUEUE_BUFFER_LENGTH;
		++ mQueueBufferNum;
	}

	// 与 setBufferData 配合使用，可以同一线程也可以非同一线程，队列为空直接返回
	void getBufferData(void *pDstData, unsigned int &length)
	{
		std::lock_guard<std::mutex> lock (mMutex);
		if (isEmpty()) {
			LOG_QUEUE("getBufferData", "queue is empty")
			return;
		}
		memcpy (pBufferArray[beginIndex], pDstData, mBufferArrayLength[beginIndex]);
		length = mBufferArrayLength[beginIndex];

		beginIndex = (beginIndex + 1) % QUEUE_BUFFER_LENGTH;
		-- mQueueBufferNum;
	}

	// 与 getBufferDataWithCondition 配合使用，当有数据输入时通知 getBufferDataWithCondition 所在线程
	void setBufferDataWithCondition(void* const pSrcData, const unsigned int &length = 0)
	{
		std::unique_lock<std::mutex> lock (mMutex);
		LOGD("setBufferDataWithCondition length = %d", length);
		if (nullptr == pSrcData) {
			LOG_QUEUE("setBufferDataWithCondition", "pSrcData is nullptr")
			return;
		}
		if (isFull()) {
			++lostNum;
			std::string strLostNum = std::to_string(lostNum);
			LOG_QUEUE("setBufferDataWithCondition queue is full, lostNum = %d", strLostNum.c_str());
			return;
		}
		// 拷贝前先将原 buffer 数据置为 0，避免干扰
		memset (pBufferArray[endIndex], 0, EACH_BUFFER_MAX_SIZE);
		if (0 == length) {
			memcpy (pBufferArray[endIndex], pSrcData, EACH_BUFFER_MAX_SIZE);
			mBufferArrayLength[endIndex] = EACH_BUFFER_MAX_SIZE;
		} else {
			memcpy (pBufferArray[endIndex], pSrcData, length);
			mBufferArrayLength[endIndex] = length;
		}
		endIndex = (endIndex + 1) % QUEUE_BUFFER_LENGTH;
		++ mQueueBufferNum;
		mEmptyCondition.notify_one();
	}

	// 与 setBufferDataWithCondition 配合使用，在不同线程，如果数据为空，等待
	void getBufferDataWithCondition(void *pDstData, unsigned int &length)
	{
		// unique_lock 配合 condition_variable 使用，队列为空的时候等待
		std::unique_lock<std::mutex> locker (mMutex);
		LOGD("getBufferDataWithCondition length = %d", length);
		while (isEmpty()) {
			LOG_QUEUE("getBufferDataWithCondition", "queue is empty")
			// 队列空了 需要等待
			mEmptyCondition.wait(locker);
		}
		memcpy (pDstData, pBufferArray[beginIndex], mBufferArrayLength[beginIndex]);
		length = mBufferArrayLength[beginIndex];

		beginIndex = (beginIndex + 1) % QUEUE_BUFFER_LENGTH;
		-- mQueueBufferNum;
	}

	int getEachBufferMaxSize() {
		return EACH_BUFFER_MAX_SIZE;
	}

private:
	// don't lock this function
	int getQueueSize() {
		return (endIndex - beginIndex + MAX_BUFFER_QUEUE_LENGTH) % MAX_BUFFER_QUEUE_LENGTH;
	}

	bool isFull()
	{
		if (!bQueueReady) {
			LOG_QUEUE("isFull", "queue is not ready")
			return true;
		}
		return (getQueueSize() >= MAX_BUFFER_QUEUE_LENGTH);
	}

	bool isEmpty()
	{
		if (!bQueueReady) {
			LOG_QUEUE("isEmpty", "queue is not ready")
			return true;
		}
		return (getQueueSize() <= 0);
	}

	void clear() {
		for (int i = 0; i < MAX_BUFFER_QUEUE_LENGTH; ++i) {
			SafeFree(pBufferArray[i])
			mBufferArrayLength[i] = 0;
		}
		beginIndex = 0;
		endIndex = 0;
		QUEUE_BUFFER_LENGTH = 0;
	}

	// 队列中 buffer 数组长度，初始化之后不再改变
	unsigned int QUEUE_BUFFER_LENGTH;
	// 队列中每个 buffer 的最大长度, 初始化之后不在改变
	unsigned int EACH_BUFFER_MAX_SIZE;

	// buffer 数组，用于缓存数据
	unsigned char* pBufferArray[MAX_BUFFER_QUEUE_LENGTH];
	// 队列中每个 buffer 有效数据的长度，实际长度必须小于 EACH_BUFFER_MAX_SIZE
	unsigned int mBufferArrayLength[MAX_BUFFER_QUEUE_LENGTH];
	// 队列中目前未消费的 buffer 数量，实时改变
	unsigned int mQueueBufferNum;
	// 队列 buffer head point
	unsigned int beginIndex;
	// 队列 buffer rear point
	unsigned int endIndex;
	std::mutex mMutex;
	// 条件变量，当队列不为空的时候通知获取数据
	std::condition_variable mEmptyCondition;
	bool bQueueReady;
	// 因队列中 buffer 满了而丢弃的帧数统计
	unsigned int lostNum;
};