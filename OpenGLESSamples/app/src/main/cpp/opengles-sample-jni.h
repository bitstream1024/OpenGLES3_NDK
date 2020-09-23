//
// Created by wcg3031 on 2020/9/23.
//

#include <jni.h>

#pragma once

//#define JNI_LOGE(...)	LOGE("")
#define JNI_METHOD_NAME(name) Java_com_cgwang1580_openglessamples_NativeFunctionHelper_##name

extern "C" {
JNIEXPORT jlong JNICALL JNI_METHOD_NAME(CreateProcessorJNI)(JNIEnv *env, jobject thiz);
JNIEXPORT jint JNICALL JNI_METHOD_NAME(InitProcessorResourceJNI)(JNIEnv *env, jobject thiz, jlong handle);
JNIEXPORT jint JNICALL JNI_METHOD_NAME(DestroyProcessorJNI)(JNIEnv *env, jobject thiz, jlong handle);

JNIEXPORT jint JNICALL JNI_METHOD_NAME(OnSurfaceCreatedJNI)(JNIEnv *env, jobject thiz, jlong handle);
JNIEXPORT jint JNICALL JNI_METHOD_NAME(OnSurfaceCreatedByTypeJNI)(JNIEnv *env, jobject thiz, jlong handle, jint effect_type);
JNIEXPORT jint JNICALL JNI_METHOD_NAME(OnSurfaceChangedJNI)(JNIEnv *env, jobject thiz, jlong handle, jint width, jint height);
JNIEXPORT jint JNICALL JNI_METHOD_NAME(OnDrawFrameJNI)(JNIEnv *env, jobject thiz, jlong handle);
JNIEXPORT jint JNICALL JNI_METHOD_NAME(GetTextureFromFrameBufferJNI)(JNIEnv *env, jobject thiz, jlong handle);
JNIEXPORT jint JNICALL JNI_METHOD_NAME(OnSurfaceDestroyedJNI)(JNIEnv *env, jobject thiz, jlong handle);
JNIEXPORT jint JNICALL JNI_METHOD_NAME(SetMotionStateJNI)(JNIEnv *env, jobject thiz, jlong handle, jobject motion_state_gl);
}