//
// Created by chauncy on 2021/1/25.
//

#include <LogAndroid.h>
#include <string>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "OpenSLESHelper.h"
#include <FileUtils.h>
#include <MyDefineUtils.h>

#define	CHECK_SL_ERROR(_tag_, _ret_)	if (SL_RESULT_SUCCESS != _ret_) {LOGE("SL_RESULT %s: error _ret_ = %d", _tag_, _ret_); return _ret_;}
#define	CHECK_SL_ERROR_VOID(_tag_, _ret_)	if (SL_RESULT_SUCCESS != _ret_) {LOGE("SL_RESULT %s: error _ret_ = %d", _tag_, _ret_);}
#define CHECK_NULL_RETURN(_p_, _ret_)	if (!_p_) {return _ret_;}

static void bqRecorderCallback (SLAndroidSimpleBufferQueueItf slBQItf, void *pContext) {
	LOGD("bqRecorderCallback begin");
	if (nullptr == pContext) {
		LOGE("bqRecorderCallback null error");
		return;
	}
	auto pHelper = static_cast<OpenSLESHelper*>(pContext);
	FdAudioRecorder* const pRecorder = pHelper->getFdAudioRecorder();
	SLresult result;
	std::lock_guard<std::mutex> lock (pHelper->m_SLAudioRecordLock);
	/// todo: write pcm data here
	FileUtils::WriteDateToFile(pRecorder->pRecordBuffer, pRecorder->lBufferSize, pRecorder->filePath.c_str(), true);
	memset(pRecorder->pRecordBuffer, 0, pRecorder->lBufferSize);

	if (pRecorder->bRecording) {
		// 保存 pcm 数据结束之后，判断是否仍在录制，是就继续请求获取录制数据，类似 GLSurfaceView 的 requestRender 方法
		result = (*pRecorder->fdRecorderBufferQueue)->Enqueue(pRecorder->fdRecorderBufferQueue, pRecorder->pRecordBuffer, pRecorder->lBufferSize);
		CHECK_SL_ERROR_VOID("bqRecorderCallback fdRecorderBufferQueue Enqueue", result)
	}
}

OpenSLESHelper::OpenSLESHelper():
		m_EngineObject(nullptr),
		m_EngineEngine(nullptr),
		m_OutputMixObject(nullptr),
		m_OutputMixEnvironmentalReverb(nullptr)
{
	LOGD("OpenSLESHelper::OpenSLESHelper begin");
	ReverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

	memset(&m_AudioPlayer, 0, sizeof(m_AudioPlayer));
	memset(&m_AudioRecorder, 0, sizeof(m_AudioRecorder));
}

OpenSLESHelper::~OpenSLESHelper()
{
	LOGD("OpenSLESHelper::~OpenSLESHelper begin");
}

int OpenSLESHelper::createSLEngine()
{
	LOGD ("OpenSLESHelper::createSLEngine begin");

	SLresult result;

	// 创建 engine object
	result = slCreateEngine(&m_EngineObject, 0, nullptr, 0, nullptr, nullptr);
	CHECK_SL_ERROR("OpenSLESHelper::createSLEngine slCreateEngine", result)
	CHECK_NULL_RETURN(m_EngineObject, result)

	// 实例化 engine object
	result = (*m_EngineObject)->Realize(m_EngineObject, SL_BOOLEAN_FALSE);
	CHECK_SL_ERROR("OpenSLESHelper::createSLEngine m_EngineObject Realize", result)

	// 获得 engine object 中的 engine 接口
	result = (*m_EngineObject)->GetInterface(m_EngineObject, SL_IID_ENGINE, &m_EngineEngine);
	CHECK_SL_ERROR("OpenSLESHelper::createSLEngine m_EngineObject GetInterface", result)
	CHECK_NULL_RETURN(m_EngineEngine, result)

	// 创建混音器
	/*const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
	const SLboolean req[1] = {SL_BOOLEAN_FALSE};
	result = (*m_EngineEngine)->CreateOutputMix(m_EngineEngine, &m_OutputMixObject, 1, ids, req);
	CHECK_SL_ERROR("OpenSLESHelper::createSLEngine CreateOutputMix", result)
	CHECK_NULL_RETURN(m_OutputMixObject, result)

	// 实例化 m_OutputMixObject
	result = (*m_OutputMixObject)->Realize(m_OutputMixObject, SL_BOOLEAN_FALSE);
	CHECK_SL_ERROR("OpenSLESHelper::createSLEngine m_OutputMixObject Realize", result)

	// 获取音频混合方法接口
	result = (*m_OutputMixObject)->GetInterface(m_OutputMixObject, SL_IID_ENVIRONMENTALREVERB, &m_OutputMixEnvironmentalReverb);
	//CHECK_SL_ERROR("OpenSLESHelper::createSLEngine m_OutputMixObject GetInterface", result)
	//CHECK_NULL_RETURN(m_OutputMixEnvironmentalReverb, result)
	if (SL_RESULT_SUCCESS == result) {
		const auto* settings =(const SLEnvironmentalReverbSettings*) &ReverbSettings;
		result = (*m_OutputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(m_OutputMixEnvironmentalReverb, settings);
	}*/

	return 0;
}

void OpenSLESHelper::destroySLEngine()
{
	LOGD("OpenSLESHelper::destroySLEngine begin");

	destroySLPlayer();
	destroySLRecorder();

	if (nullptr != m_EngineObject) {
		(*m_EngineObject)->Destroy(m_EngineObject);
		m_EngineObject = nullptr;
		m_EngineEngine = nullptr;
	}
}

int OpenSLESHelper::createSLPlayerWithAssets(JNIEnv *env, jobject asset_manager, const char *path)
{
	const std::string TAG = "OpenSLESHelper::createSLPlayerWithAssets";
	LOGD("%s begin", TAG.c_str());

	AAssetManager* pMgr = AAssetManager_fromJava(env, asset_manager);
	if (!pMgr) {
		return JNI_FALSE;
	}
	AAsset* pAsset = AAssetManager_open(pMgr, path, AASSET_MODE_UNKNOWN);
	if (!pAsset){
		return JNI_FALSE;
	}

	// 通过 pAsset 获取资源内容
	off_t start = 0, length = 0;
	int fd = AAsset_openFileDescriptor(pAsset, &start, &length);
	if (fd < 0) {
		LOGE("%s env is nullptr", TAG.c_str());
		AAsset_close(pAsset);
		return JNI_FALSE;
	}

	// configure audio source
	SLDataLocator_AndroidFD locFd = {SL_DATALOCATOR_ANDROIDFD, fd, start, length};
	SLDataFormat_MIME formatMime = {SL_DATAFORMAT_MIME, nullptr, SL_CONTAINERTYPE_UNSPECIFIED};
	SLDataSource audioSrc = {&locFd, &formatMime};

	// configure audio sink
	SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, m_OutputMixObject};
	SLDataSink audioSnk = {&loc_outmix, nullptr};

	// create audio player
	const SLInterfaceID ids[3] = {SL_IID_SEEK, SL_IID_MUTESOLO, SL_IID_VOLUME};
	const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

	SLresult result;
	// 创建 audio player 实例
	result = (*m_EngineEngine)->CreateAudioPlayer(m_EngineEngine, &m_AudioPlayer.fdPlayerObject, &audioSrc, &audioSnk, 3, ids, req);
	CHECK_SL_ERROR("OpenSLESHelper::createSLPlayerWithAssets CreateAudioPlayer", result)

	// 实例化 audio player
	result = (*m_AudioPlayer.fdPlayerObject)->Realize(m_AudioPlayer.fdPlayerObject, SL_BOOLEAN_FALSE);
	CHECK_SL_ERROR("OpenSLESHelper::createSLPlayerWithAssets m_PlayerObject Realize", result)

	// 获得 audio player 接口方法
	result = (*m_AudioPlayer.fdPlayerObject)->GetInterface(m_AudioPlayer.fdPlayerObject, SL_IID_PLAY, &m_AudioPlayer.fdPlayerPlay);
	CHECK_SL_ERROR("OpenSLESHelper::createSLPlayerWithAssets SL_IID_PLAY GetInterface", result)
	result = (*m_AudioPlayer.fdPlayerObject)->GetInterface(m_AudioPlayer.fdPlayerObject, SL_IID_SEEK, &m_AudioPlayer.fdPlayerSeek);
	CHECK_SL_ERROR("OpenSLESHelper::createSLPlayerWithAssets SL_IID_SEEK GetInterface", result)
	result = (*m_AudioPlayer.fdPlayerObject)->GetInterface(m_AudioPlayer.fdPlayerObject, SL_IID_MUTESOLO, &m_AudioPlayer.fdPlayerMuteSolo);
	CHECK_SL_ERROR("OpenSLESHelper::createSLPlayerWithAssets SL_IID_MUTESOLO GetInterface", result)
	result = (*m_AudioPlayer.fdPlayerObject)->GetInterface(m_AudioPlayer.fdPlayerObject, SL_IID_VOLUME, &m_AudioPlayer.fdPlayerVolume);
	CHECK_SL_ERROR("OpenSLESHelper::createSLPlayerWithAssets SL_IID_VOLUME GetInterface", result)

	// 设置 loop
	result = (*m_AudioPlayer.fdPlayerSeek)->SetLoop(m_AudioPlayer.fdPlayerSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
	CHECK_SL_ERROR("OpenSLESHelper::createSLPlayerWithAssets fdPlayerSeek SetLoop", result)                

	return 0;
}

void OpenSLESHelper::setSLPlayerState(bool bPlay)
{
	LOGD("OpenSLESHelper::setSLPlayerState bPlay = %d", bPlay);
	if (m_AudioPlayer.fdPlayerPlay) {
		SLresult result = (*m_AudioPlayer.fdPlayerPlay)->SetPlayState(m_AudioPlayer.fdPlayerPlay,
				bPlay ? SL_PLAYSTATE_PLAYING : SL_PLAYSTATE_PAUSED);
		LOGD("OpenSLESHelper::setSLPlayerState SetPlayState result = %d", result);
	}
}

void OpenSLESHelper::destroySLPlayer()
{
	LOGD("OpenSLESHelper::destroySLPlayer begin");
	if (nullptr != m_AudioPlayer.fdPlayerObject) {
		(*m_AudioPlayer.fdPlayerObject)->Destroy(m_AudioPlayer.fdPlayerObject);
		memset(&m_AudioPlayer, 0, sizeof(m_AudioPlayer));
	}
}

int OpenSLESHelper::createSLRecorder()
{
	LOGD("OpenSLESHelper::createSLRecorder begin");

	SLresult result;
	// 配置 IO 设备
	SLDataLocator_IODevice loc_dev = {
			SL_DATALOCATOR_IODEVICE,
			SL_IODEVICE_AUDIOINPUT,
			SL_DEFAULTDEVICEID_AUDIOINPUT,
			NULL
	};
	SLDataSource audioSrc = {&loc_dev, NULL};

	// 配置输出 buffer 队列
	SLDataLocator_AndroidSimpleBufferQueue loc_bq = {
			SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
			2
	};
	// 配置输出数据格式
	SLuint32 sampleRate = 0;
	switch (RECORDING_SAMPLE_RATE)
	{
		case 16000:
			sampleRate = SL_SAMPLINGRATE_16;
			break;
		case 44100:
			sampleRate = SL_SAMPLINGRATE_44_1;
			break;
		default:
			LOGE("OpenSLESHelper::createSLRecorder sampleRate is error");
			throw (std::logic_error("sampleRate is error"));
			break;
	}
	// 根据录制参数进行配置
	SLuint32 channelMask = (1 == RECORDING_CHANNELS ? SL_SPEAKER_FRONT_CENTER : (SL_SPEAKER_FRONT_LEFT|SL_SPEAKER_FRONT_RIGHT));
	SLuint32 bitsPerSample = (8 == RECORDING_DATA_BIT ? SL_PCMSAMPLEFORMAT_FIXED_8 : SL_PCMSAMPLEFORMAT_FIXED_16);
	SLDataFormat_PCM format_pcm = {
			SL_DATAFORMAT_PCM,
			RECORDING_CHANNELS,
			sampleRate,
			bitsPerSample,
			bitsPerSample,
			channelMask,
			SL_BYTEORDER_LITTLEENDIAN
	};
	SLDataSink audioSnk = {
			&loc_bq,
			&format_pcm
	};

	// 创建 recorder 对象，并且指定开放 SL_IID_ANDROIDSIMPLEBUFFERQUEUE 这个接口
	const SLInterfaceID id[1] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
	const SLboolean req[1] = {SL_BOOLEAN_TRUE};
	result = (*m_EngineEngine)->CreateAudioRecorder(m_EngineEngine, &m_AudioRecorder.fdRecorderObject,
			&audioSrc, &audioSnk, 1, id, req);
	CHECK_SL_ERROR("OpenSLESHelper::createSLRecorder CreateAudioRecorder", result)
	CHECK_NULL_RETURN(m_AudioRecorder.fdRecorderObject, result)

	result = (*m_AudioRecorder.fdRecorderObject)->Realize(m_AudioRecorder.fdRecorderObject, SL_BOOLEAN_FALSE);
	CHECK_SL_ERROR("OpenSLESHelper::createSLRecorder fdRecorderObject Realize", result)

	// 获取 recorder 的接口方法
	result = (*m_AudioRecorder.fdRecorderObject)->GetInterface(m_AudioRecorder.fdRecorderObject, SL_IID_RECORD,
			&m_AudioRecorder.fdRecorderRecord);
	CHECK_SL_ERROR("OpenSLESHelper::createSLRecorder SL_IID_RECORD GetInterface", result)
	(void)result;

	// get the buffer queue interface
	result = (*m_AudioRecorder.fdRecorderObject)->GetInterface(m_AudioRecorder.fdRecorderObject,
			SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &m_AudioRecorder.fdRecorderBufferQueue);
	CHECK_SL_ERROR("OpenSLESHelper::createSLRecorder SL_IID_ANDROIDSIMPLEBUFFERQUEUE GetInterface", result)
	(void)result;

	// register callback on the buffer queue
	result = (*m_AudioRecorder.fdRecorderBufferQueue)->RegisterCallback(m_AudioRecorder.fdRecorderBufferQueue,
			bqRecorderCallback, this);
	CHECK_SL_ERROR("OpenSLESHelper::createSLRecorder fdRecorderBufferQueue RegisterCallback", result)
	(void)result;

	return 0;
}

FdAudioRecorder *OpenSLESHelper::getFdAudioRecorder()
{
	return &m_AudioRecorder;
}

void OpenSLESHelper::startRecording()
{
	LOGD("OpenSLESHelper::startRecording begin");
	SLresult result;
	//std::lock_guard<std::mutex> lock (m_SLAudioLock);

	// 如果已经处于录制状态，先停止录制，并清除 buffer
	result = (*m_AudioRecorder.fdRecorderRecord)->SetRecordState(m_AudioRecorder.fdRecorderRecord, SL_RECORDSTATE_STOPPED);
	CHECK_SL_ERROR_VOID("OpenSLESHelper::startRecording fdRecorderRecord SetRecordState", result)
	result = (*m_AudioRecorder.fdRecorderBufferQueue)->Clear(m_AudioRecorder.fdRecorderBufferQueue);
	CHECK_SL_ERROR_VOID("OpenSLESHelper::startRecording fdRecorderBufferQueue Clear", result)

	// 分配 audio 录制缓存
	if (nullptr == m_AudioRecorder.pRecordBuffer) {
		m_AudioRecorder.lBufferSize = sizeof(unsigned char) * RECORDING_DATA_BIT/8 * RECORDING_CHANNELS
				* RECORDING_SAMPLE_RATE * RECORDING_EACH_SAMPLE_TIME;
		m_AudioRecorder.pRecordBuffer = static_cast<unsigned char*>(malloc(m_AudioRecorder.lBufferSize));
		if (nullptr == m_AudioRecorder.pRecordBuffer){
			LOGE("OpenSLESHelper::startRecording no memory");
			return;
		}
	}

	m_AudioRecorder.filePath = "/sdcard/AudioVideoTest/audio_opensl_" + std::to_string(MyTimeUtils::getCurrentTime()) + ".pcm";
	std::lock_guard<std::mutex> lock (m_SLAudioRecordLock);
	// 设置录制开启状态（OpenSL ES 也是一个状态机，通过改变状态来执行操作）
	result = (*m_AudioRecorder.fdRecorderRecord)->SetRecordState(m_AudioRecorder.fdRecorderRecord, SL_RECORDSTATE_RECORDING);
	CHECK_SL_ERROR_VOID("OpenSLESHelper::startRecording fdRecorderRecord SetRecordState", result)
	m_AudioRecorder.bRecording = true;

	// 在设置完录制状态后一定需要先 Enqueue 一次，这样的话才会开始采集回调
	result = (*m_AudioRecorder.fdRecorderBufferQueue)->Enqueue(m_AudioRecorder.fdRecorderBufferQueue,
			m_AudioRecorder.pRecordBuffer, m_AudioRecorder.lBufferSize);
	CHECK_SL_ERROR_VOID("OpenSLESHelper::startRecording fdRecorderRecord SetRecordState", result)
}

void OpenSLESHelper::stopRecording()
{
	LOGD("OpenSLESHelper::stopRecording begin");
	std::lock_guard<std::mutex> lock (m_SLAudioRecordLock);
	if (nullptr != m_AudioRecorder.fdRecorderRecord) {
		(*m_AudioRecorder.fdRecorderRecord)->SetRecordState(m_AudioRecorder.fdRecorderRecord, SL_RECORDSTATE_STOPPED);
		m_AudioRecorder.bRecording = false;
	}
}

void OpenSLESHelper::destroySLRecorder()
{
	LOGD("OpenSLESHelper::destroySLRecorder begin");
	if (nullptr != m_AudioRecorder.fdRecorderRecord) {
		(*m_AudioRecorder.fdRecorderObject)->Destroy(m_AudioRecorder.fdRecorderObject);
		m_AudioRecorder.fdRecorderObject = nullptr;
		SafeFree (m_AudioRecorder.pRecordBuffer)
		memset(&m_AudioRecorder, 0, sizeof(FdAudioRecorder));
	}
}
