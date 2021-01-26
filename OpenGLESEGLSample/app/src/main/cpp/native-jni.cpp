#include "native-jni.h"
#include <string>
#include <LogAndroid.h>
#include <common.h>
#include <MyDefineUtils.h>
#include <MediaCodecHelper.h>
#include "EGLHelper.h"
#include "audio/OpenSLESHelper.h"

int m_ImgBufferLength = 0;
unsigned char* m_pImgData = nullptr;
bool bNeedEnocode = true;

OpenSLESHelper m_SLESHelper;

///---------------------------------- NativeEGLHelper function ------------------------------------///
extern "C" jint Java_com_example_opengleseglsample_NativeEGLHelper_Init(JNIEnv *env, jobject clazz)
{
    // TODO: implement Init()
    int retCode = 0;
    if (MediaCodecHelper::CreateInstance()) {
		retCode = MediaCodecHelper::CreateInstance()->PrepareEncoderWithCtx(1080, 1920, 15, "/sdcard/native_out.mp4", nullptr);
    }

    if (EGLHelper::CreateInstance()) {
        retCode = EGLHelper::CreateInstance()->Init(1, MediaCodecHelper::CreateInstance()->GetInputWindow());
        LOGD ("Java_com_example_opengleseglsample_NativeEGLHelper_Init Init ret = %d", retCode);
    }
    return retCode;
}

extern "C" jint Java_com_example_opengleseglsample_NativeEGLHelper_UnInit(JNIEnv *env, jobject clazz)
{
    // TODO: implement UnInit()
    int retCode = 0;
    if (EGLHelper::CreateInstance()) {
        retCode = EGLHelper::CreateInstance()->UnInit();
        LOGD ("Java_com_example_opengleseglsample_NativeEGLHelper_UnInit UnInit ret = %d", retCode);
    }
    if (nullptr != m_pImgData) {
        SafeFree(m_pImgData);
        m_ImgBufferLength = 0;
    }

    return retCode;
}

extern "C" jint Java_com_example_opengleseglsample_NativeEGLHelper_Draw(JNIEnv *env, jobject clazz)
{
    // TODO: implement Draw()
    int retCode = 0;
    if (EGLHelper::CreateInstance() && MediaCodecHelper::CreateInstance()) {
    	if (bNeedEnocode) {
			for (int i = 0; i < 200; ++i) {
				retCode = EGLHelper::CreateInstance()->Draw();
				EGLHelper::CreateInstance()->SwapBuffers();
				EGLHelper::CreateInstance()->SetPresentationTime((long)MyTimeUtils::getCurrentTime() * 1000000);
				MediaCodecHelper::CreateInstance()->DrainEncoder(false);
				LOGD ("Java_com_example_opengleseglsample_NativeEGLHelper_Draw Draw ret = %d", retCode);
			}
			MediaCodecHelper::CreateInstance()->DrainEncoder(true);
			if (MediaCodecHelper::CreateInstance()) {
				MediaCodecHelper::CreateInstance()->ReleaseEncoder();
				bNeedEnocode = false;
			}
    	} else {
			retCode = EGLHelper::CreateInstance()->Draw();
			LOGD ("Java_com_example_opengleseglsample_NativeEGLHelper_Draw Draw ret = %d", retCode);
    	}
    }
    return retCode;
}

extern "C" jint Java_com_example_opengleseglsample_NativeEGLHelper_SetImageData(JNIEnv *env, jobject clazz,
        jbyteArray data, jint img_width, jint img_height, jint format)
{
    // TODO: implement SetImageData()
    int retCode = 0;
    int width = img_width;
    int height = img_height;

    if (nullptr == data) {
        return ERROR_INPUT;
    }

    int length = env->GetArrayLength(data);
    if (m_ImgBufferLength != length && nullptr != m_pImgData) {
        free(m_pImgData);
        m_pImgData = nullptr;
        m_pImgData = (unsigned char*)malloc(m_ImgBufferLength);
    }
    m_ImgBufferLength = length;

    if (nullptr == m_pImgData) {
        m_pImgData = (unsigned char*)malloc(m_ImgBufferLength);
    }
    jbyte *bytes = env->GetByteArrayElements(data, 0);
    memcpy(m_pImgData, bytes, m_ImgBufferLength);
    env->ReleaseByteArrayElements(data, bytes, 0);

    if (EGLHelper::CreateInstance()) {
        retCode = EGLHelper::CreateInstance()->SetImageData(width, height, m_pImgData);
        LOGD ("Java_com_example_opengleseglsample_NativeEGLHelper_SetImageData Draw ret = %d", retCode);
    }
    return retCode;
}




///---------------------------------- OpenSLESActivity function ------------------------------------///
extern "C" jint Java_com_example_opengleseglsample_OpenSLESActivity_createSLEngine(JNIEnv *env, jobject thiz)
{
	const std::string TAG = "Java_com_example_opengleseglsample_OpenSLESActivity_createSLEngine";
	int nRet = m_SLESHelper.createSLEngine();
	LOGD("%s createSLEngine nRet = %d", TAG.c_str(), nRet);
	return nRet;
}

extern "C" jboolean Java_com_example_opengleseglsample_OpenSLESActivity_createAssetAudioPlayer(JNIEnv *env,
		jobject thiz, jobject asset_manager, jstring file_name)
{
	const std::string TAG = "Java_com_example_opengleseglsample_OpenSLESActivity_createAssetAudioPlayer";
	if (!env) {
		LOGE("%s env is nullptr", TAG.c_str());
		return JNI_FALSE;
	}
	// convert Java string to UTF-8
	const char* utf8Name = env->GetStringUTFChars(file_name, 0);
	if (!utf8Name) {
		return JNI_FALSE;
	}

	int nRet = m_SLESHelper.createSLPlayerWithAssets(env, asset_manager, utf8Name);
	env->ReleaseStringUTFChars(file_name, utf8Name);
	return 0 == nRet;
}

extern "C" void Java_com_example_opengleseglsample_OpenSLESActivity_setPlayingAssetAudioPlayerState(JNIEnv *env,
																									jobject thiz, jboolean b_play)
{
	const std::string TAG = "Java_com_example_opengleseglsample_OpenSLESActivity_setPlayingAssetAudioPlayerState";
	m_SLESHelper.setSLPlayerState(b_play);
}

extern "C" void Java_com_example_opengleseglsample_OpenSLESActivity_destroySLEngine(JNIEnv *env, jobject thiz)
{
	m_SLESHelper.destroySLEngine();
}