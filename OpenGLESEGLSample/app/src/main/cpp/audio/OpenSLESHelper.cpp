//
// Created by chauncy on 2021/1/25.
//

#include <LogAndroid.h>
#include <assert.h>
#include <string>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "OpenSLESHelper.h"

#define	CHECK_SL_ERROR(_tag_, _ret_)	if (SL_RESULT_SUCCESS != _ret_) {LOGE("%s: error _ret_ = %d", _tag_, _ret_); return _ret_;}
#define CHECK_NULL_RETURN(_p_, _ret_)	if (!_p_) {return _ret_;}

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

	// 创建混音器，类似于 java 层的 MediaMuxer
	const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
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
	}

	return 0;
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

int OpenSLESHelper::createSLRecorder()
{

	return 0;
}

void OpenSLESHelper::destroySLEngine()
{
	LOGD("OpenSLESHelper::destroySLEngine begin");
	if (nullptr != m_AudioPlayer.fdPlayerObject) {
		(*m_AudioPlayer.fdPlayerObject)->Destroy(m_AudioPlayer.fdPlayerObject);
		memset(&m_AudioPlayer, 0, sizeof(m_AudioPlayer));
	}

	if (nullptr != m_EngineObject) {
		(*m_EngineObject)->Destroy(m_EngineObject);
		m_EngineObject = nullptr;
		m_EngineEngine = nullptr;
	}
}
