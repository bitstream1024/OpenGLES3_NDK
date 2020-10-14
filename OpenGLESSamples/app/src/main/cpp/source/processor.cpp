//
// Created by wcg3031 on 2020/4/1.
//

#include "processor_inner.h"
#include "LogAndroid.h"
#include "GLES3/gl3.h"
#include <string>
#include <SampleDrawYUV.h>
#include "MyDefineUtils.h"
#include "OpenImageHelper.h"
#include "SampleRender3D.h"
#include "shader_content.h"
#include "unistd.h"
#include "draw_utils.h"

int CreateSampleAndShaderByDrawType (const PHandle pProcessorHandle, DrawType drawType)
{
	LOGD("CreateSampleAndShaderByDrawType drawType = %d", drawType);
	CAL_TIME_COST("CreateSampleAndShaderByDrawType")
	CHECK_NULL_INPUT(pProcessorHandle)
	auto MyProcessorHandle = (LPProcessorHandle)pProcessorHandle;
	int ret = ERROR_OK;
	switch (drawType)
	{
		case eDraw_Triangle:
			ret = CreateShaderHelper(&MyProcessorHandle->mShaderSetTriangle, triangle_vertex_shader, triangle_fragment_shader);
			LOGD("CreateSampleAndShaderByDrawType CreateShaderHelper mShaderSetTriangle ret = %d", ret);
			break;
		case eDraw_SimpleTexture:
			ret = CreateShaderHelper(&MyProcessorHandle->mShaderSetTexture, texture_vertex_shader, texture_fragment_shader);
			LOGD("CreateSampleAndShaderByDrawType CreateShaderHelper mShaderSetTexture ret = %d", ret);
			break;
		case eDraw_TextureFBO:
			ret = CreateShaderHelper(&MyProcessorHandle->mShaderSetFBO, fbo_vertex_shader, fbo_fragment_shader);
			LOGD("CreateSampleAndShaderByDrawType CreateShaderHelper mShaderSetFBO ret = %d", ret);
			ret = CreateShaderHelper(&MyProcessorHandle->mShaderSetFBONormal, fbo_vertex_shader, fbo_normal_fragment_shader);
			LOGD("CreateSampleAndShaderByDrawType CreateShaderHelper mShaderSetFBONormal ret = %d", ret);
			break;
		case eDraw_HardwareBuffer:
			if (nullptr == MyProcessorHandle->pHardwareBufferHelper)
			{
				MyProcessorHandle->pHardwareBufferHelper = new AHardwareBufferHelper();
			}
			break;
		case eDraw_Transform:
			if (nullptr == MyProcessorHandle->m_pSampleTransform)
			{
				MyProcessorHandle->m_pSampleTransform = new SampleTransform ();
				MyProcessorHandle->m_pSampleTransform->InitSample();
			}
			break;
		case eDraw_Render3D:
			if (nullptr == MyProcessorHandle->m_pSampleRender3D)
			{
				MyProcessorHandle->m_pSampleRender3D = new SampleRender3D ();
				MyProcessorHandle->m_pSampleRender3D->InitSample();
			}
			break;
		case eDraw_TriangleFBO:
			if (nullptr == MyProcessorHandle->m_pSampleDrawFBO)
			{
				MyProcessorHandle->m_pSampleDrawFBO = new SampleDrawFBO ();
				MyProcessorHandle->m_pSampleDrawFBO->InitSample();
			}
			break;
		case eDraw_Render3DMesh:
			if (!MyProcessorHandle->m_pSampleRender3DMesh)
			{
				MyProcessorHandle->m_pSampleRender3DMesh = new SampleRender3DMesh ();
			}
			break;
		case eDraw_YUV:
			if (!MyProcessorHandle->m_pSampleRenderYUV) {
				MyProcessorHandle->m_pSampleRenderYUV = new SampleDrawYUV();
				MyProcessorHandle->m_pSampleRenderYUV->SetImageYuvResource(MyProcessorHandle->lpMyImageInfo_YUV);
				MyProcessorHandle->m_pSampleRenderYUV->InitSample();
			}
			break;
		default:
			LOGD("CreateSampleAndShaderByDrawType nDrawType = %d is unsupported", drawType);
			break;
	}
	return ERROR_OK;
}

int DestroySampleAndShaderByDrawType (const PHandle pProcessorHandle, DrawType drawType)
{
	LOGD("DestroySampleAndShaderByDrawType drawType = %d", drawType);
	CHECK_NULL_INPUT(pProcessorHandle)
	auto MyProcessorHandle = (LPProcessorHandle)pProcessorHandle;
	int ret = ERROR_OK;

	switch (drawType)
	{
		case eDraw_Triangle:
			SafeDelete (MyProcessorHandle->mShaderSetTriangle.pShaderHelper);
			break;
		case eDraw_SimpleTexture:
			SafeDelete (MyProcessorHandle->mShaderSetTexture.pShaderHelper);
			break;
		case eDraw_TextureFBO:
			SafeDelete(MyProcessorHandle->mShaderSetFBO.pShaderHelper);
			SafeDelete(MyProcessorHandle->mShaderSetFBONormal.pShaderHelper);
			break;
		case eDraw_HardwareBuffer:
			if (MyProcessorHandle->pHardwareBufferHelper && MyProcessorHandle->pHardwareBufferHelper->getCreateState())
			{
				MyProcessorHandle->pHardwareBufferHelper->destroyGPUBuffer();
			}
			SafeDelete (MyProcessorHandle->pHardwareBufferHelper);
			break;
		case eDraw_Transform:
			if (MyProcessorHandle->m_pSampleTransform)
			{
				MyProcessorHandle->m_pSampleTransform->UnInitSample();
				SafeDelete(MyProcessorHandle->m_pSampleTransform);
			}
			break;
		case eDraw_Render3D:
			if (MyProcessorHandle->m_pSampleRender3D)
			{
				MyProcessorHandle->m_pSampleRender3D->UnInitSample();
				SafeDelete(MyProcessorHandle->m_pSampleRender3D);
			}
			break;
		case eDraw_TriangleFBO:
			if (MyProcessorHandle->m_pSampleDrawFBO)
			{
				MyProcessorHandle->m_pSampleDrawFBO->UnInitSample();
				SafeDelete(MyProcessorHandle->m_pSampleDrawFBO);
			}
			break;
		case eDraw_Render3DMesh:
			SafeDelete(MyProcessorHandle->m_pSampleRender3DMesh);
			break;
		case eDraw_YUV:
			if (MyProcessorHandle->m_pSampleRenderYUV)
				MyProcessorHandle->m_pSampleRenderYUV->UnInitSample();
			SafeDelete(MyProcessorHandle->m_pSampleRenderYUV)
			break;
		default:
			LOGD("onDrawFrame nDrawType = %d is unsupported", drawType);
			break;
	}
	return ERROR_OK;
}

int CreateProcessor (PHandle *ppProcessorHandle)
{
	CAL_TIME_COST("CreateProcessor")

	if (nullptr == ppProcessorHandle || nullptr != *ppProcessorHandle)
	{
		LOGE("onSurfaceCreated ppProcessorHandle is NULL");
		return ERROR_INPUT;
	}

	*ppProcessorHandle = (LPProcessorHandle)malloc (sizeof (ProcessorHandle));
	CHECK_NULL_MALLOC (*ppProcessorHandle);
	memset(*ppProcessorHandle, 0, sizeof(ProcessorHandle));

	return ERROR_OK;
}

int SetupResource (PHandle const pProcessorHandle)
{
	CAL_TIME_COST("SetupResource")

	LOGD("SetupResource pProcessorHandle = %p", pProcessorHandle);

	CHECK_NULL_INPUT (pProcessorHandle)
	auto MyProcessorHandle = (LPProcessorHandle)pProcessorHandle;
	int retCode = ERROR_OK;

	do{
		MyProcessorHandle->lpMyImageInfo = (LPMyImageInfo)malloc(sizeof(MyImageInfo));
		CHECK_NULL_MALLOC(MyProcessorHandle->lpMyImageInfo);
		memset (MyProcessorHandle->lpMyImageInfo, 0 , sizeof(MyImageInfo));
		retCode = OpenImageHelper::LoadPngFromFile(TEST_IMAGE_PATH_2, MyProcessorHandle->lpMyImageInfo);
		if (ERROR_OK != retCode) {
			LOGE("SetupResource d retCode = %d", retCode);
			break;
		}

		MyProcessorHandle->lpMyImageInfo_YUV = (LPMyImageInfo)malloc(sizeof(MyImageInfo));
		CHECK_NULL_MALLOC(MyProcessorHandle->lpMyImageInfo_YUV);
		memset(MyProcessorHandle->lpMyImageInfo_YUV, 0, sizeof(MyImageInfo));
		retCode = OpenImageHelper::LoadYuvFromFile(TEST_IMAGE_PATH_YUV_0, MyProcessorHandle->lpMyImageInfo_YUV);
		if (ERROR_OK != retCode) {
			LOGE("SetupResource LoadYuvFromFile retCode = %d", retCode);
			break;
		}
	} while (false);

	return retCode;
}

int DestroyProcessor (PHandle *ppProcessorHandle)
{
	LOGD("SetupResource pProcessorHandle = %p", *ppProcessorHandle);

	CHECK_NULL_INPUT (*ppProcessorHandle)
	auto MyProcessorHandle = (LPProcessorHandle)(*ppProcessorHandle);
	if (nullptr != MyProcessorHandle->lpMyImageInfo)
	{
		OpenImageHelper::FreeMyImageInfo(MyProcessorHandle->lpMyImageInfo);
		SafeFree(MyProcessorHandle->lpMyImageInfo);
	}
	if (nullptr != MyProcessorHandle->lpMyImageInfo_YUV)
	{
		OpenImageHelper::FreeMyImageInfo(MyProcessorHandle->lpMyImageInfo_YUV);
		SafeFree(MyProcessorHandle->lpMyImageInfo_YUV);
	}
	SafeFree (*ppProcessorHandle);
	LOGD("onSurfaceDestroyed *ppProcessorHandle = %p", *ppProcessorHandle);

	return ERROR_OK;
}

int onSurfaceCreated (PHandle const pProcessorHandle, const int effectType)
{
	CAL_TIME_COST("onSurfaceCreated")

	LOGD("onSurfaceCreated pProcessorHandle = %p", pProcessorHandle);

	CHECK_NULL_INPUT (pProcessorHandle)
	auto MyProcessorHandle = (LPProcessorHandle)pProcessorHandle;

	/// set which sample you want to get
	LOGD("onSurfaceCreated effectType = %d", effectType);
	MyProcessorHandle->m_eDrawType = (DrawType) effectType;
	int ret = CreateSampleAndShaderByDrawType(MyProcessorHandle, MyProcessorHandle->m_eDrawType);
	LOGD("CreateSampleAndShaderByDrawType ret = %d", ret);

	return ERROR_OK;
}

int onSurfaceChanged (PHandle const pProcessorHandle, const int width, const int height)
{
	LOGD("onSurfaceChanged");
	glViewport(0, 0, width, height);
	return 0;
}

int onDrawFrame (PHandle const pProcessorHandle)
{
	LOGD("processor onDrawFrame begin");

	LOGD("onDrawFrame pProcessorHandle = %p", pProcessorHandle);

	CHECK_NULL_INPUT (pProcessorHandle)
	auto MyProcessorHandle = (LPProcessorHandle)pProcessorHandle;
	++MyProcessorHandle->mRenderTime;

	int ret = ERROR_OK;

	DrawType nDrawType = MyProcessorHandle->m_eDrawType;
	switch (nDrawType)
	{
		case eDraw_Triangle:
			ret = drawTriangle (MyProcessorHandle->mShaderSetTriangle.pShaderHelper);
			LOGD("onDrawFrame drawTriangle ret = %d", ret);
			break;
		case eDraw_SimpleTexture:
			ret = drawTexture(MyProcessorHandle->mShaderSetTexture.pShaderHelper, MyProcessorHandle->lpMyImageInfo);
			LOGD("onDrawFrame drawTexture ret = %d", ret);
			break;
		case eDraw_TextureFBO:
			ret = drawFBO(MyProcessorHandle->mShaderSetFBO.pShaderHelper, MyProcessorHandle->mShaderSetFBONormal.pShaderHelper,
						  MyProcessorHandle->lpMyImageInfo);
			LOGD("onDrawFrame drawFBO ret = %d", ret);
			break;
		case eDraw_HardwareBuffer:
			ret = drawByHardwareBuffer(MyProcessorHandle->pHardwareBufferHelper, MyProcessorHandle->lpMyImageInfo);
			LOGD("onDrawFrame drawByHardwareBuffer ret = %d", ret);
			//usleep(33000);  // fps 33ms
			break;
		case eDraw_Transform:
			ret = MyProcessorHandle->m_pSampleTransform->OnDrawFrame();
			LOGD("onDrawFrame m_pSampleTransform OnDrawFrame ret = %d", ret);
			break;
		case eDraw_Render3D:
			ret = MyProcessorHandle->m_pSampleRender3D->OnDrawFrame();
			LOGD("onDrawFrame m_pSampleRender3D OnDrawFrame ret = %d", ret);
			break;
		case eDraw_TriangleFBO:
			ret = MyProcessorHandle->m_pSampleDrawFBO->OnDrawFrame();
			//ret = MyProcessorHandle->m_pSampleDrawFBO->OnDrawFrameRect();
			LOGD("onDrawFrame m_pSampleDrawFBO OnDrawFrame ret = %d", ret);
			break;
		case eDraw_Render3DMesh:
			if (MyProcessorHandle->m_pSampleRender3DMesh)
			{
				MyProcessorHandle->m_pSampleRender3DMesh->SetMotionState(MyProcessorHandle->m_MotionState);
				MyProcessorHandle->m_MotionState.setZero();
				ret = MyProcessorHandle->m_pSampleRender3DMesh->OnDrawFrame();
				LOGD("onDrawFrame m_pSampleRender3DMesh OnDrawFrame ret = %d", ret);
			}
			break;
		case eDraw_YUV:
			if (MyProcessorHandle->m_pSampleRenderYUV) {
				MyProcessorHandle->m_pSampleRenderYUV->OnDrawFrame();
			}
			break;
		default:
			LOGD("onDrawFrame nDrawType = %d", nDrawType);
			break;
	}

	LOGD("processor onDrawFrame end");

	return ret;
}

int getTextureFromFrameBuffer (PHandle const pProcessorHandle) {
	LOGD("getTextureFromFrameBuffer");
	if (nullptr == pProcessorHandle) {
		return -1;
	}
	LPProcessorHandle MyProcessorHandle = (LPProcessorHandle)pProcessorHandle;
	return MyProcessorHandle->m_pSampleDrawFBO->GetDrawTexture();
}

int setMotionState (PHandle const pProcessorHandle, MotionState const motionState)
{
	LOGD("setMotionState");
	CHECK_NULL_INPUT (pProcessorHandle)
	auto MyProcessorHandle = (LPProcessorHandle)pProcessorHandle;

	MyProcessorHandle->m_MotionState.eMotionType = motionState.eMotionType;
	MyProcessorHandle->m_MotionState.transform_x = motionState.transform_x;
	MyProcessorHandle->m_MotionState.transform_y = motionState.transform_y;
	MyProcessorHandle->m_MotionState.transform_z = motionState.transform_z;

	if (eMOTION_TRANSLATE == MyProcessorHandle->m_MotionState.eMotionType)
		MyProcessorHandle->m_MotionState.logMotionState("setMotionState m_MotionState");

	return ERROR_OK;
}

int onSurfaceDestroyed (PHandle const pProcessorHandle)
{
	LOGD("processor onSurfaceDestroyed = %p", pProcessorHandle);

	CHECK_NULL_INPUT (pProcessorHandle)
	auto MyProcessorHandle = (LPProcessorHandle)pProcessorHandle;

	int retCode = DestroySampleAndShaderByDrawType (MyProcessorHandle, MyProcessorHandle->m_eDrawType);
	if (ERROR_OK != retCode) {
		LOGE("processor onSurfaceDestroyed retCode = %d", retCode);
	}

	return retCode;
}