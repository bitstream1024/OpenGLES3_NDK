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

#define RECORDING_SAMPLE_RATE				44100
#define RECORDING_CHANNELS					1
#define RECORDING_DATA_BIT					16
// 控制音频录制时每个缓存大小的系数
#define RECORDING_EACH_SAMPLE_SIZE_FACTOR	0.2f
#define RECORDING_FILE_FOLDER				"/sdcard/AudioVideoTest"

/** callback function for audio recorder*/
static void bqRecorderCallback (SLAndroidSimpleBufferQueueItf slBQItf, void *pContext) {
	LOGD("bqRecorderCallback begin");
	if (nullptr == pContext) {
		LOGE("bqRecorderCallback null error");
		return;
	}
	auto pHelper = static_cast<OpenSLESHelper*>(pContext);
	auto pRecorder = pHelper->getFdAudioRecorder();
	SLresult result;
	std::lock_guard<std::mutex> lock (pHelper->m_SLAudioRecorderLock);
	/// todo: write pcm data here
	FileUtils::WriteDataWithFile(pRecorder->pRecordBufferArray[pRecorder->nBufferIndex], pRecorder->lBufferSize, pRecorder->pFile);

	pRecorder->nBufferIndex = 1 - pRecorder->nBufferIndex;
	// 更新 buffer index，交换缓存用于存储音频数据
	memset(pRecorder->pRecordBufferArray[pRecorder->nBufferIndex], 0, pRecorder->lBufferSize);

	if (pRecorder->bRecording) {
		LOGD("bqRecorderCallback fdRecorderBufferQueue Enqueue nBufferIndex = %d", pRecorder->nBufferIndex);
		// 保存 pcm 数据结束之后，判断是否仍在录制，是就继续请求获取录制数据，类似 GLSurfaceView 的 requestRender 方法
		result = (*pRecorder->fdRecorderBufferQueue)->Enqueue(pRecorder->fdRecorderBufferQueue,
															  pRecorder->pRecordBufferArray[pRecorder->nBufferIndex], pRecorder->lBufferSize);
		CHECK_SL_ERROR_VOID("bqRecorderCallback fdRecorderBufferQueue Enqueue", result)
	} else {
		// 如果外部点击停止录制，则设置 stop 状态
		result = (*pRecorder->fdRecorderRecord)->SetRecordState(pRecorder->fdRecorderRecord, SL_RECORDSTATE_STOPPED);
		CHECK_SL_ERROR_VOID("bqRecorderCallback fdRecorderRecord SetRecordState", result)
	}
}

/** callback function for pcm player*/
static void bqPcmPlayerCallback(SLAndroidSimpleBufferQueueItf slBQItf, void* pContext) {
	LOGD("bqPcmPlayerCallback begin");
	if (nullptr == pContext) {
		LOGE("bqPcmPlayerCallback null error");
		return;
	}
	auto pHelper = static_cast<OpenSLESHelper*>(pContext);
	auto pPlayer = pHelper->getFdPcmPlayer();
	// 清空上一帧缓存
	memset(pPlayer->pPlayerBufferArray[pPlayer->nBufferIndex], 0, pPlayer->lBufferSize);
	// 将数据读入新的缓存中
	pPlayer->nBufferIndex = 1 - pPlayer->nBufferIndex;
	unsigned long readSize = pPlayer->lBufferSize;
	FileUtils::ReadDataFromFile(pPlayer->pPlayerBufferArray[pPlayer->nBufferIndex], readSize, pPlayer->pFile);
	SLresult result;

	std::lock_guard<std::mutex> lock(pHelper->m_SLAudioPlayerLock);
	if (readSize != 0 && pPlayer->bPlaying) {
		// 将从文件中读取的 pcm 数据塞给播放器
		result = (*pPlayer->fdSimpleBufferQueue)->Enqueue(pPlayer->fdSimpleBufferQueue, pPlayer->pPlayerBufferArray[pPlayer->nBufferIndex], readSize);
		CHECK_SL_ERROR_VOID("bqPcmPlayerCallback fdSimpleBufferQueue Enqueue", result)
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

	memset(&m_AssetsAudioPlayer, 0, sizeof(m_AssetsAudioPlayer));
	memset(&m_PcmAudioPlayer, 0, sizeof(m_PcmAudioPlayer));
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

	// 创建混音器，用于创建播放器
	const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
	const SLboolean req[1] = {SL_BOOLEAN_FALSE};
	result = (*m_EngineEngine)->CreateOutputMix(m_EngineEngine, &m_OutputMixObject, 1, ids, req);
	CHECK_SL_ERROR_VOID("OpenSLESHelper::createSLEngine CreateOutputMix", result)
	CHECK_NULL_RETURN(m_OutputMixObject, result)

	// 实例化 m_OutputMixObject
	result = (*m_OutputMixObject)->Realize(m_OutputMixObject, SL_BOOLEAN_FALSE);
	CHECK_SL_ERROR_VOID("OpenSLESHelper::createSLEngine m_OutputMixObject Realize", result)

	// 获取混音器方法接口
	/*result = (*m_OutputMixObject)->GetInterface(m_OutputMixObject, SL_IID_ENVIRONMENTALREVERB, &m_OutputMixEnvironmentalReverb);
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
	destroyPcmPlayer();

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
	result = (*m_EngineEngine)->CreateAudioPlayer(m_EngineEngine, &m_AssetsAudioPlayer.fdPlayerObject, &audioSrc, &audioSnk, 3, ids, req);
	CHECK_SL_ERROR("OpenSLESHelper::createSLPlayerWithAssets CreateAudioPlayer", result)

	// 实例化 audio player
	result = (*m_AssetsAudioPlayer.fdPlayerObject)->Realize(m_AssetsAudioPlayer.fdPlayerObject, SL_BOOLEAN_FALSE);
	CHECK_SL_ERROR("OpenSLESHelper::createSLPlayerWithAssets m_PlayerObject Realize", result)

	// 获得 audio player 接口方法
	result = (*m_AssetsAudioPlayer.fdPlayerObject)->GetInterface(m_AssetsAudioPlayer.fdPlayerObject, SL_IID_PLAY, &m_AssetsAudioPlayer.fdPlayerPlay);
	CHECK_SL_ERROR("OpenSLESHelper::createSLPlayerWithAssets SL_IID_PLAY GetInterface", result)
	result = (*m_AssetsAudioPlayer.fdPlayerObject)->GetInterface(m_AssetsAudioPlayer.fdPlayerObject, SL_IID_SEEK, &m_AssetsAudioPlayer.fdPlayerSeek);
	CHECK_SL_ERROR("OpenSLESHelper::createSLPlayerWithAssets SL_IID_SEEK GetInterface", result)
	result = (*m_AssetsAudioPlayer.fdPlayerObject)->GetInterface(m_AssetsAudioPlayer.fdPlayerObject, SL_IID_MUTESOLO, &m_AssetsAudioPlayer.fdPlayerMuteSolo);
	CHECK_SL_ERROR("OpenSLESHelper::createSLPlayerWithAssets SL_IID_MUTESOLO GetInterface", result)
	result = (*m_AssetsAudioPlayer.fdPlayerObject)->GetInterface(m_AssetsAudioPlayer.fdPlayerObject, SL_IID_VOLUME, &m_AssetsAudioPlayer.fdPlayerVolume);
	CHECK_SL_ERROR("OpenSLESHelper::createSLPlayerWithAssets SL_IID_VOLUME GetInterface", result)

	// 设置 loop
	result = (*m_AssetsAudioPlayer.fdPlayerSeek)->SetLoop(m_AssetsAudioPlayer.fdPlayerSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
	CHECK_SL_ERROR("OpenSLESHelper::createSLPlayerWithAssets fdPlayerSeek SetLoop", result)                

	return 0;
}

void OpenSLESHelper::setSLPlayerState(bool bPlay)
{
	LOGD("OpenSLESHelper::setSLPlayerState bPlay = %d", bPlay);
	if (m_AssetsAudioPlayer.fdPlayerPlay) {
		SLresult result = (*m_AssetsAudioPlayer.fdPlayerPlay)->SetPlayState(m_AssetsAudioPlayer.fdPlayerPlay,
				bPlay ? SL_PLAYSTATE_PLAYING : SL_PLAYSTATE_PAUSED);
		LOGD("OpenSLESHelper::setSLPlayerState SetPlayState result = %d", result);
	}
}

void OpenSLESHelper::destroySLPlayer()
{
	LOGD("OpenSLESHelper::destroySLPlayer begin");
	if (nullptr != m_AssetsAudioPlayer.fdPlayerObject) {
		(*m_AssetsAudioPlayer.fdPlayerObject)->Destroy(m_AssetsAudioPlayer.fdPlayerObject);
		memset(&m_AssetsAudioPlayer, 0, sizeof(m_AssetsAudioPlayer));
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
	m_PcmFormat = format_pcm;
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

	// 如果已经处于录制状态，先停止录制，并清除 buffer
	SLuint32 nRecordState;
	(*m_AudioRecorder.fdRecorderRecord)->GetRecordState(m_AudioRecorder.fdRecorderRecord, &nRecordState);
	if (SL_RECORDSTATE_RECORDING == nRecordState) {
		result = (*m_AudioRecorder.fdRecorderRecord)->SetRecordState(m_AudioRecorder.fdRecorderRecord, SL_RECORDSTATE_STOPPED);
		CHECK_SL_ERROR_VOID("OpenSLESHelper::startRecording fdRecorderRecord SetRecordState", result)
		result = (*m_AudioRecorder.fdRecorderBufferQueue)->Clear(m_AudioRecorder.fdRecorderBufferQueue);
		CHECK_SL_ERROR_VOID("OpenSLESHelper::startRecording fdRecorderBufferQueue Clear", result)
	}

	// 分配 audio 录制缓存
	m_AudioRecorder.lBufferSize = sizeof(unsigned char) * RECORDING_DATA_BIT / 8 * RECORDING_CHANNELS
								  * RECORDING_SAMPLE_RATE * RECORDING_EACH_SAMPLE_SIZE_FACTOR;

	if (nullptr == m_AudioRecorder.pRecordBufferArray[0]) {
		m_AudioRecorder.pRecordBufferArray[0] = static_cast<unsigned char*>(malloc(m_AudioRecorder.lBufferSize));
	}
	if (nullptr == m_AudioRecorder.pRecordBufferArray[1]) {
		m_AudioRecorder.pRecordBufferArray[1] = static_cast<unsigned char*>(malloc(m_AudioRecorder.lBufferSize));
	}
	if (nullptr == m_AudioRecorder.pRecordBufferArray[0] || nullptr == m_AudioRecorder.pRecordBufferArray[1]) {
		LOGE("OpenSLESHelper::startRecording no memory");
		return;
	}
	memset(m_AudioRecorder.pRecordBufferArray[0], 0, sizeof(m_AudioRecorder.lBufferSize));
	memset(m_AudioRecorder.pRecordBufferArray[1], 0, sizeof(m_AudioRecorder.lBufferSize));

	if (nullptr != m_AudioRecorder.pFile) {
		fclose(m_AudioRecorder.pFile);
		m_AudioRecorder.pFile = nullptr;
	}
	char path[MAX_PATH * 2] {0};
	sprintf(path, "%s/audio_opengles_%lld", RECORDING_FILE_FOLDER, MyTimeUtils::GetCurrentTime());
	m_AudioRecorder.pFile = fopen(path, "ab+");
	std::lock_guard<std::mutex> lock (m_SLAudioRecorderLock);
	// 设置录制开启状态（OpenSL ES 也是一个状态机，通过改变状态来执行操作）
	result = (*m_AudioRecorder.fdRecorderRecord)->SetRecordState(m_AudioRecorder.fdRecorderRecord, SL_RECORDSTATE_RECORDING);
	CHECK_SL_ERROR_VOID("OpenSLESHelper::startRecording fdRecorderRecord SetRecordState", result)
	m_AudioRecorder.bRecording = true;

	// 在设置完录制状态后一定需要先 Enqueue 一次，这样的话才会开始采集回调
	m_AudioRecorder.nBufferIndex = 0;
	result = (*m_AudioRecorder.fdRecorderBufferQueue)->Enqueue(m_AudioRecorder.fdRecorderBufferQueue,
			m_AudioRecorder.pRecordBufferArray[m_AudioRecorder.nBufferIndex], m_AudioRecorder.lBufferSize);
	CHECK_SL_ERROR_VOID("OpenSLESHelper::startRecording fdRecorderBufferQueue Enqueue", result)
}

void OpenSLESHelper::stopRecording()
{
	LOGD("OpenSLESHelper::stopRecording begin");
	std::lock_guard<std::mutex> lock (m_SLAudioRecorderLock);
	if (nullptr != m_AudioRecorder.fdRecorderRecord) {
		m_AudioRecorder.bRecording = false;
	}
}

void OpenSLESHelper::destroySLRecorder()
{
	LOGD("OpenSLESHelper::destroySLRecorder begin");
	if (nullptr != m_AudioRecorder.fdRecorderRecord) {
		(*m_AudioRecorder.fdRecorderObject)->Destroy(m_AudioRecorder.fdRecorderObject);
		m_AudioRecorder.fdRecorderObject = nullptr;
		SafeFree(m_AudioRecorder.pRecordBufferArray[0])
		SafeFree(m_AudioRecorder.pRecordBufferArray[1])
		if (nullptr != m_AudioRecorder.pFile) {
			fclose(m_AudioRecorder.pFile);
			m_AudioRecorder.pFile = nullptr;
		}
		memset(&m_AudioRecorder, 0, sizeof(FdAudioRecorder));
	}
}

void OpenSLESHelper::createPcmPlayer()
{
	LOGD("OpenSLESHelper::createPcmPlayer begin");

	if (nullptr == m_OutputMixObject) {
		LOGE("OpenSLESHelper::createPcmPlayer m_OutputMixObject is nullptr");
		return;
	}

	// todo: 配置音频格式, 这里使用创建 AudioRecorder 时配置的音频格式
	SLDataFormat_PCM format_pcm = {
			SL_DATAFORMAT_PCM,
			RECORDING_CHANNELS,
			SL_SAMPLINGRATE_44_1,
			SL_PCMSAMPLEFORMAT_FIXED_16,
			SL_PCMSAMPLEFORMAT_FIXED_16,
			SL_SPEAKER_FRONT_CENTER,
			SL_BYTEORDER_LITTLEENDIAN
	};;
	SLDataLocator_AndroidSimpleBufferQueue android_queue = {
			SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
			2
	};
	SLDataSource audioSrc = {
			&android_queue,
			&format_pcm
	};
	// 设置 data sink
	SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, m_OutputMixObject};
	SLDataSink audioSnk = {
			&loc_outmix,
			nullptr
	};

	SLresult result;
	// 创建 audio player 实例
	const SLInterfaceID ids[1] = {SL_IID_BUFFERQUEUE};
	const SLboolean req[1] = {SL_BOOLEAN_TRUE};
	result = (*m_EngineEngine)->CreateAudioPlayer(m_EngineEngine, &m_PcmAudioPlayer.fdPlayerObject,
			&audioSrc, &audioSnk, sizeof(ids)/sizeof(SLInterfaceID), ids, req);
	CHECK_SL_ERROR_VOID("OpenSLESHelper::createPcmPlayer CreateAudioPlayer", result)

	// 实例化 audio player
	result = (*m_PcmAudioPlayer.fdPlayerObject)->Realize(m_PcmAudioPlayer.fdPlayerObject, SL_BOOLEAN_FALSE);
	CHECK_SL_ERROR_VOID("OpenSLESHelper::createPcmPlayer m_PlayerObject Realize", result)

	// 获得 audio player 接口方法
	result = (*m_PcmAudioPlayer.fdPlayerObject)->GetInterface(m_PcmAudioPlayer.fdPlayerObject, SL_IID_PLAY, &m_PcmAudioPlayer.fdPlayerPlay);
	CHECK_SL_ERROR_VOID("OpenSLESHelper::createPcmPlayer SL_IID_PLAY GetInterface", result)
	result = (*m_PcmAudioPlayer.fdPlayerObject)->GetInterface(m_PcmAudioPlayer.fdPlayerObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &m_PcmAudioPlayer.fdSimpleBufferQueue);
	CHECK_SL_ERROR_VOID("OpenSLESHelper::createPcmPlayer SL_IID_ANDROIDSIMPLEBUFFERQUEUE GetInterface", result)

	// 注册 player 回调函数
	result = (*m_PcmAudioPlayer.fdSimpleBufferQueue)->RegisterCallback(m_PcmAudioPlayer.fdSimpleBufferQueue, bqPcmPlayerCallback, this);
	CHECK_SL_ERROR_VOID("OpenSLESHelper::createPcmPlayer fdSimpleBufferQueue RegisterCallback", result)
}

FdAudioPlayer *OpenSLESHelper::getFdPcmPlayer()
{
	return &m_PcmAudioPlayer;
}

void OpenSLESHelper::startPlayPcmData(std::string pcmPath)
{
	LOGD("OpenSLESHelper::startPlayPcmData begin, pcmPath = %s", pcmPath.c_str());
	SLresult result;

	// 分配缓存，用于获取从 pcm 文件中读取的数据内容，借用音频录制时的计算方式
	m_PcmAudioPlayer.lBufferSize = sizeof(unsigned char) * RECORDING_DATA_BIT / 8 * RECORDING_CHANNELS
								  * RECORDING_SAMPLE_RATE * RECORDING_EACH_SAMPLE_SIZE_FACTOR;
	if (nullptr == m_PcmAudioPlayer.pPlayerBufferArray[0]) {
		m_PcmAudioPlayer.pPlayerBufferArray[0] = static_cast<unsigned char*>(malloc(m_PcmAudioPlayer.lBufferSize));
	}
	if (nullptr == m_PcmAudioPlayer.pPlayerBufferArray[1]) {
		m_PcmAudioPlayer.pPlayerBufferArray[1] = static_cast<unsigned char*>(malloc(m_PcmAudioPlayer.lBufferSize));
	}
	if (nullptr == m_PcmAudioPlayer.pPlayerBufferArray[0] || nullptr == m_PcmAudioPlayer.pPlayerBufferArray[1]) {
		LOGE("OpenSLESHelper::startPlayPcmData no memory");
		return;
	}
	memset(m_PcmAudioPlayer.pPlayerBufferArray[0], 0, m_PcmAudioPlayer.lBufferSize);
	memset(m_PcmAudioPlayer.pPlayerBufferArray[1], 0, m_PcmAudioPlayer.lBufferSize);

	m_PcmAudioPlayer.nBufferIndex = 0;
	if (nullptr != m_PcmAudioPlayer.pFile) {
		fclose(m_PcmAudioPlayer.pFile);
		m_PcmAudioPlayer.pFile = nullptr;
	}
	if (pcmPath.empty()) {
		char testPath [MAX_PATH * 2] {0};
		sprintf(testPath, "%s/myfile", RECORDING_FILE_FOLDER);
		//sprintf(testPath, "%s/audio_opensl_1611801325117.pcm", RECORDING_FILE_FOLDER);
		m_PcmAudioPlayer.pFile = fopen(testPath, "rb");
	} else {
		m_PcmAudioPlayer.pFile = fopen(pcmPath.c_str(), "rb");
	}
	// 先读取一帧 pcm 数据进来
	unsigned long lSize = m_PcmAudioPlayer.lBufferSize;
	FileUtils::ReadDataFromFile(m_PcmAudioPlayer.pPlayerBufferArray[m_PcmAudioPlayer.nBufferIndex],
			lSize, m_PcmAudioPlayer.pFile);

	std::lock_guard<std::mutex> lock (m_SLAudioPlayerLock);
	if (nullptr != m_PcmAudioPlayer.fdPlayerObject) {
		result = (*m_PcmAudioPlayer.fdPlayerPlay)->SetPlayState(m_PcmAudioPlayer.fdPlayerPlay, SL_PLAYSTATE_PLAYING);
		CHECK_SL_ERROR_VOID("OpenSLESHelper::startPlayPcmData SetPlayState", result)

		result = (*m_PcmAudioPlayer.fdSimpleBufferQueue)->Enqueue(m_PcmAudioPlayer.fdSimpleBufferQueue,
				m_PcmAudioPlayer.pPlayerBufferArray[m_PcmAudioPlayer.nBufferIndex], lSize);
		CHECK_SL_ERROR_VOID("OpenSLESHelper::startPlayPcmData fdSimpleBufferQueue Enqueue", result)

		m_PcmAudioPlayer.bPlaying = true;
	}
}

void OpenSLESHelper::stopPcmPlayer()
{
	LOGD("OpenSLESHelper::stopPcmPlayer begin");
	SLresult result;
	std::lock_guard<std::mutex> lock (m_SLAudioPlayerLock);
	if (nullptr != m_PcmAudioPlayer.fdPlayerObject) {
		result = (*m_PcmAudioPlayer.fdPlayerPlay)->SetPlayState(m_PcmAudioPlayer.fdPlayerPlay, SL_PLAYSTATE_STOPPED);
		CHECK_SL_ERROR_VOID("OpenSLESHelper::stopPcmPlayer SetPlayState", result)
		m_PcmAudioPlayer.bPlaying = false;
	}
}

void OpenSLESHelper::destroyPcmPlayer()
{
	LOGD("OpenSLESHelper::destroyPcmPlayer begin");
	SafeFree(m_PcmAudioPlayer.pPlayerBufferArray[0])
	SafeFree(m_PcmAudioPlayer.pPlayerBufferArray[1])
	if (nullptr != m_PcmAudioPlayer.pFile) {
		fclose(m_PcmAudioPlayer.pFile);
		m_PcmAudioPlayer.pFile = nullptr;
	}
	if (nullptr != m_PcmAudioPlayer.fdPlayerObject) {
		(*m_PcmAudioPlayer.fdPlayerPlay)->SetPlayState(m_PcmAudioPlayer.fdPlayerPlay, SL_PLAYSTATE_STOPPED);
		(*m_PcmAudioPlayer.fdPlayerObject)->Destroy(m_PcmAudioPlayer.fdPlayerObject);
		memset(&m_PcmAudioPlayer, 0, sizeof(m_PcmAudioPlayer));
	}
}
