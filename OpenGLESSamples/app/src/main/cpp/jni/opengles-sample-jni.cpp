#include <string>
#include "processor.h"

#include "MotionState.h"
#include "JniUtils.h"

#include "opengles-sample-jni.h"

JNIEXPORT jlong JNICALL JNI_METHOD_NAME(CreateProcessorJNI)(JNIEnv *env, jobject thiz)
{
	// TODO: implement CreateProcessor()
	PHandle pProcessorHandle = nullptr;
	int retCode = CreateProcessor(&pProcessorHandle);
	if (retCode != NONE_ERROR || nullptr == pProcessorHandle) {
		LOGE("jni CreateProcessor error");
	}
	return (jlong)pProcessorHandle;
}

JNIEXPORT jint JNICALL JNI_METHOD_NAME(InitProcessorResourceJNI)(JNIEnv *env, jobject thiz, jlong handle)
{
	// TODO: implement InitProcessorResource()
	auto addr = (PHandle)handle;
	int retCode = SetupResource(addr);
	return retCode;
}

JNIEXPORT jint JNICALL JNI_METHOD_NAME(DestroyProcessorJNI)(JNIEnv *env, jobject thiz, jlong handle)
{
	// TODO: implement DestroyProcessor()
	auto addr = (PHandle)handle;
	int retCode = DestroyProcessor(&addr);
	return retCode;
}



JNIEXPORT jint JNICALL JNI_METHOD_NAME(OnSurfaceCreatedJNI)(JNIEnv *env, jobject thiz, jlong handle)
{
	// TODO: implement onSurfaceCreatedJNI()
	auto addr = (PHandle)handle;
	int retCode = onSurfaceCreated(addr);
	return retCode;
}

JNIEXPORT jint JNICALL JNI_METHOD_NAME(OnSurfaceCreatedByTypeJNI)(JNIEnv *env, jobject thiz, jlong handle, jint effect_type)
{
	// TODO: implement onSurfaceCreatedByTypeJNI()
	auto addr = (PHandle)handle;
	int drawType = effect_type;
	int retCode = onSurfaceCreated(addr, drawType);
	return retCode;
}

JNIEXPORT jint JNICALL JNI_METHOD_NAME(OnSurfaceChangedJNI)(JNIEnv *env, jobject thiz, jlong handle, jint width, jint height)
{
	// TODO: implement onSurfaceChangedJNI()
	auto addr = (PHandle)handle;
	int retCode = onSurfaceChanged(addr, width, height);
	return retCode;
}

JNIEXPORT jint JNICALL JNI_METHOD_NAME(OnDrawFrameJNI)(JNIEnv *env, jobject thiz, jlong handle)
{
	// TODO: implement onDrawFrameJNI()
	auto addr = (PHandle)handle;
	int retCode = onDrawFrame(addr);
	return retCode;
}

JNIEXPORT jint JNICALL JNI_METHOD_NAME(GetTextureFromFrameBufferJNI)(JNIEnv *env, jobject thiz, jlong handle)
{
	// TODO: implement getTextureFromFrameBuffer()
	auto addr = (PHandle)handle;
	return getTextureFromFrameBuffer(addr);
}

JNIEXPORT jint JNICALL JNI_METHOD_NAME(OnSurfaceDestroyedJNI)(JNIEnv *env, jobject thiz, jlong handle)
{
	// TODO: implement onSurfaceDestroyedJNI()
	auto addr = (PHandle)handle;
	int retCode = onSurfaceDestroyed(addr);
	return retCode;
}

JNIEXPORT jint JNICALL JNI_METHOD_NAME(SetMotionStateJNI)(JNIEnv *env, jobject thiz, jlong handle, jobject motion_state_gl)
{
	// TODO: implement SetMotionState()
	//LOGD("Java_com_cgwang1580_senioropengles_MyGLRenderer_SetMotionState");
	MotionState motionState;
	int retCode = ConvertMotionState (env, motion_state_gl, motionState);
	//LOGD("ConvertMotionState ret = %d", retCode);

	auto addr = (PHandle)handle;
	retCode = setMotionState (addr, motionState);
	//LOGD("setMotionState ret = %d", retCode);

	return retCode;
}