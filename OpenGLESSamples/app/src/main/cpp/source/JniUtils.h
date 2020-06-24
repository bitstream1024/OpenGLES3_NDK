//
// Created by wcg3031 on 2020/6/18.
//

#pragma once

#include <LogAndroid.h>
#include <MyDefineUtils.h>

int ConvertMotionState (JNIEnv *env, jobject jMotionObject, MotionState &motionState)
{
	LOGD ("ConvertMotionState");
	CHECK_NULL_INPUT(env);
	CHECK_NULL_INPUT(jMotionObject);

	jclass jMotionClass = env->GetObjectClass(jMotionObject);
	if (nullptr == jMotionClass)
	{
		LOGE("ConvertMotionState jMotionClass is nullptr");
		return ERROR_INPUT;
	}

	jfieldID motionTypeID = env->GetFieldID(jMotionClass, "mMotionType", "I");
	jfieldID transformXID = env->GetFieldID(jMotionClass, "translate_x", "F");
	jfieldID transformYID = env->GetFieldID(jMotionClass, "translate_y", "F");
	jfieldID transformZID = env->GetFieldID(jMotionClass, "translate_z", "F");

	motionState.eMotionType = (MotionType)env->GetIntField(jMotionObject, motionTypeID);
	motionState.transform_x = env->GetFloatField(jMotionObject, transformXID);
	motionState.transform_y = env->GetFloatField(jMotionObject, transformYID);
	motionState.transform_z = env->GetFloatField(jMotionObject, transformZID);

	LOGD("ConvertMotionState motionState %d, (%f %f %f)", motionState.eMotionType,
			motionState.transform_x, motionState.transform_y, motionState.transform_z);
	env->DeleteLocalRef(jMotionClass);

	return ERROR_OK;
}

/// this function is used to test variable form java layer
int ConvertJavaClassVariableTest (JNIEnv *env, jobject thiz)
{
	LOGD ("ConvertJavaClassVariableTest");
	if (nullptr == env || nullptr == thiz)
	{
		LOGE("ConvertJavaClassVariableTest nullptr");
		return ERROR_INPUT;
	}
	jclass myGLRenderClazz = env->GetObjectClass(thiz);
	if (nullptr == myGLRenderClazz)
		return ERROR_INPUT;

	// get variable like int/double in thiz class
	jfieldID testNunID = env->GetFieldID(myGLRenderClazz, "TEST_NUM", "I");
	int testNum = env->GetIntField(thiz, testNunID);
	LOGD("ConvertJavaClassVariableTest testNum = %d", testNum);

	// get variable like String
	jfieldID testStringID = env->GetFieldID(myGLRenderClazz, "TEST_STRING", "Ljava/lang/String;");
	jstring jStringTestString = (jstring)env->GetObjectField(thiz, testStringID);
	const char *testString = env->GetStringUTFChars(jStringTestString, 0);
	LOGD("ConvertJavaClassVariableTest testString = %s", testString);

	// get static variable like int/double
	jfieldID testStaticNumID = env->GetStaticFieldID(myGLRenderClazz, "TEST_STATIC_NUM", "I");
	int testStaticNum = env->GetStaticIntField(myGLRenderClazz, testStaticNumID);
	LOGD("ConvertJavaClassVariableTest testStaticNum = %d", testStaticNum);

	// get static variable String
	jfieldID TAGFiledID = env->GetStaticFieldID(myGLRenderClazz, "TAG", "Ljava/lang/String;");
	jstring jstringTAG = (jstring)env->GetStaticObjectField(myGLRenderClazz, TAGFiledID);
	const char *sTag = env->GetStringUTFChars(jstringTAG, 0);
	LOGD("ConvertJavaClassVariableTest sTag = %s", sTag);

	// release
	env->DeleteLocalRef(myGLRenderClazz);
	return ERROR_OK;
}