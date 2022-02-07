#include <jni.h>
#include <string>
#include "processor.h"

#include "MotionState.h"
#include "JniUtils.h"

PHandle pProcessorHandle = NULL;

extern "C"
JNIEXPORT jint JNICALL
Java_com_cgwang1580_openglessamples_MyGLRenderer_onSurfaceCreatedJNI(JNIEnv *env, jobject thiz)
{
	// TODO: implement onSurfaceCreatedJNI()
	int ret = onSurfaceCreated(&pProcessorHandle);
	return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_cgwang1580_openglessamples_MyGLRenderer_onSurfaceCreatedByTypeJNI(JNIEnv *env, jobject thiz, jint effect_type)
{
	// TODO: implement onSurfaceCreatedByTypeJNI()
	// TODO: implement onSurfaceCreatedJNI()
	int ret = onSurfaceCreated(&pProcessorHandle);
	return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_cgwang1580_openglessamples_MyGLRenderer_onSurfaceChangedJNI(JNIEnv *env, jobject thiz, jint width, jint height)
{
	// TODO: implement onSurfaceChangedJNI()
	int ret = onSurfaceChanged(pProcessorHandle, width, height);
	return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_cgwang1580_openglessamples_MyGLRenderer_onDrawFrameJNI(JNIEnv *env, jobject thiz)
{
	// TODO: implement onDrawFrameJNI()
	int ret = onDrawFrame(pProcessorHandle);
	return ret;
}extern "C"
JNIEXPORT jint JNICALL
Java_com_cgwang1580_openglessamples_MyGLRenderer_onSurfaceDestroyedJNI(JNIEnv *env, jobject thiz)
{
	// TODO: implement onSurfaceDestroyedJNI()
	int ret = onSurfaceDestroyed(&pProcessorHandle);
	return ret;
}extern "C"
JNIEXPORT jint JNICALL
Java_com_cgwang1580_openglessamples_MyGLRenderer_SetMotionState(JNIEnv *env, jobject thiz, jobject motion_state_gl)
{
	// TODO: implement SetMotionState()
	LOGD("Java_com_cgwang1580_senioropengles_MyGLRenderer_SetMotionState");
	MotionState motionState;
	int ret = ConvertMotionState (env, motion_state_gl, motionState);
	LOGD("ConvertMotionState ret = %d", ret);

	/*ret = ConvertJavaClassVariableTest (env, thiz);
	LOGD("ConvertJavaClassVariableTest ret = %d", ret);*/

	ret = setMotionState (pProcessorHandle, motionState);
	LOGD("setMotionState ret = %d", ret);

	return ret;
}