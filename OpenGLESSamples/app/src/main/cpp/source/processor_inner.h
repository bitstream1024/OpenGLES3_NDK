//
// Created by wcg3031 on 2020/4/1.
//

#pragma once

#include "ShaderHelper.h"
#include "processor.h"
#include <string>
#include "AHardwareBufferHelper.h"
#include "SampleTransform.h"
#include "SampleRender3D.h"
#include "SampleDrawFBO.h"
#include "SampleRender3DMesh.h"

#define TEST_IMAGE_PATH_0	"/sdcard/OpenGLESTest/television.png"
#define TEST_IMAGE_PATH_1	"/sdcard/OpenGLESTest/testlib.png"
#define TEST_IMAGE_PATH_2	"/sdcard/OpenGLESTest/night.png"
#define TEST_IMAGE_PATH_YUV_0	"/sdcard/OpenGLESTest/Img_test_1_640X480.nv12"

typedef struct __tag_color_set_
{
	float r;
	float g;
	float b;
	float alpha;
}ColorSet, *LPColorSet;

typedef struct __shader_set_
{
	std::string vertexShader;
	std::string fragmentShader;
	ShaderHelper *pShaderHelper;
}ShaderSet, *LPShaderSet;

typedef enum
{
	eDraw_Triangle = 0,
	eDraw_SimpleTexture,
	eDraw_TextureFbo,
	eDraw_HardwareBuffer,
	eDraw_TransFrom,
	eDraw_Render3D,
	eDraw_TriangleFbo,
	eDraw_Render3DMesh,
}DrawType;

typedef struct __tag_processor_handle
{
	DrawType m_eDrawType;
	int mRenderTime;
	MotionState m_MotionState;
	ShaderSet mShaderSetTriangle;
	ShaderSet mShaderSetTexture;
	ShaderSet mShaderSetFBO;
	ShaderSet mShaderSetFBONormal;
	LPMyImageInfo lpMyImageInfo;
	LPMyImageInfo lpMyImageInfo_YUV;
	AHardwareBufferHelper *pHardwareBufferHelper;
	SampleTransform *m_pSampleTransform;
	SampleRender3D *m_pSampleRender3D;
	SampleDrawFBO * m_pSampleDrawFBO;
	SampleRender3DMesh *m_pSampleRender3DMesh;
} ProcessorHandle, *LPProcessorHandle;

