//
// Created by wcg3031 on 2020/4/1.
//

#include "processor_inner.h"
#include "LogAndroid.h"
#include "GLES3/gl3.h"
#include <string>
#include "MyDefineUtils.h"
#include "OpenImageHelper.h"
#include "SampleRender3D.h"
#include "shader_content.h"
#include "unistd.h"
#include "draw_utils.h"

int CreateSampleAndShaderByDrawType (const PHandle pProcessorHandle, DrawType drawType)
{
	LOGD("CreateSampleAndShaderByDrawType drawType = %d", drawType);
	CHECK_NULL_INPUT(pProcessorHandle)
	LPProcessorHandle MyProcessorHandle = (LPProcessorHandle)pProcessorHandle;
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
	LPProcessorHandle MyProcessorHandle = (LPProcessorHandle)pProcessorHandle;
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
		default:
			LOGD("onDrawFrame nDrawType = %d is unsupported", drawType);
			break;
	}
	return ERROR_OK;
}

int onSurfaceCreated (PHandle *ppProcessorHandle, const int effectType)
{
	LOGD("onSurfaceCreated");

	if (nullptr == ppProcessorHandle || nullptr != *ppProcessorHandle)
	{
		LOGE("onSurfaceCreated ppProcessorHandle is NULL");
		return ERROR_INPUT;
	}

	*ppProcessorHandle = (LPProcessorHandle)malloc (sizeof (ProcessorHandle));
	CHECK_NULL_MALLOC (*ppProcessorHandle);
	LPProcessorHandle MyProcessorHandle = (LPProcessorHandle)*ppProcessorHandle;
	memset(MyProcessorHandle, 0, sizeof(ProcessorHandle));

	/// set which sample you want to get
	LOGD("onSurfaceCreated effectType = %d", effectType);
	MyProcessorHandle->m_eDrawType = (DrawType) effectType;
	int ret = CreateSampleAndShaderByDrawType(MyProcessorHandle, MyProcessorHandle->m_eDrawType);
	LOGD("CreateSampleAndShaderByDrawType ret = %d", ret);

	if (nullptr != MyProcessorHandle->lpMyImageInfo)
	{
		OpenImageHelper::FreeMyImageInfo(MyProcessorHandle->lpMyImageInfo);
	}
	MyProcessorHandle->lpMyImageInfo = (LPMyImageInfo)malloc(sizeof(MyImageInfo));
	CHECK_NULL_MALLOC(MyProcessorHandle->lpMyImageInfo);
	memset (MyProcessorHandle->lpMyImageInfo, 0 , sizeof(MyImageInfo));

	if (nullptr != MyProcessorHandle->lpMyImageInfo_YUV)
	{
		OpenImageHelper::FreeMyImageInfo(MyProcessorHandle->lpMyImageInfo_YUV);
	}
	MyProcessorHandle->lpMyImageInfo_YUV = (LPMyImageInfo)malloc(sizeof(MyImageInfo));
	CHECK_NULL_MALLOC(MyProcessorHandle->lpMyImageInfo_YUV);
	memset(MyProcessorHandle->lpMyImageInfo_YUV, 0, sizeof(MyImageInfo));

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return ERROR_OK;
}

int onSurfaceChanged (const PHandle pProcessorHandle, const int width, const int height)
{
	LOGD("onSurfaceChanged");
	glViewport(0, 0, width, height);
	return 0;
}

int onDrawFrame (const PHandle pProcessorHandle)
{
	LOGD("onDrawFrame");
	CHECK_NULL_INPUT (pProcessorHandle)
	LPProcessorHandle MyProcessorHandle = (LPProcessorHandle)pProcessorHandle;
	++MyProcessorHandle->mRenderTime;

	int ret = ERROR_OK;
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (nullptr != MyProcessorHandle->lpMyImageInfo && nullptr == MyProcessorHandle->lpMyImageInfo->buffer[0])
	{
		OpenImageHelper::LoadPngFromFile(TEST_IMAGE_PATH_2, MyProcessorHandle->lpMyImageInfo);
		//OpenImageHelper::SaveImageToPng (MyProcessorHandle->lpMyImageInfo, "/sdcard/OpenGLESTest/testpng.png");
		/*MyImageInfo myImageInfo {0};
		myImageInfo.width = MyProcessorHandle->lpMyImageInfo->width;
		myImageInfo.height = MyProcessorHandle->lpMyImageInfo->height;
		myImageInfo.format = MY_FORMAT_RGBA;
		myImageInfo.channel [0] = myImageInfo.channel[1] = myImageInfo.channel[2] = myImageInfo.width;
		myImageInfo.buffer[0] = MyProcessorHandle->lpMyImageInfo->buffer[0];
		OpenImageHelper::SaveImageToYuv(&myImageInfo, "/sdcard/OpenGLESTest/test_0_1280x720.RGB32");*/
	}

	if (nullptr != MyProcessorHandle->lpMyImageInfo_YUV && nullptr == MyProcessorHandle->lpMyImageInfo_YUV->buffer[0])
	{
		OpenImageHelper::LoadYuvFromFile(TEST_IMAGE_PATH_YUV_0, MyProcessorHandle->lpMyImageInfo_YUV);
		/*char sPath[MAX_PATH]{0};
		sprintf(sPath, "/sdcard/OpenGLESTest/Save_0_%dx%d.nv12", MyProcessorHandle->lpMyImageInfo_YUV->channel[0],
				MyProcessorHandle->lpMyImageInfo_YUV->height);
		OpenImageHelper::SaveImageToYuv(MyProcessorHandle->lpMyImageInfo_YUV, sPath);*/

	}
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
			usleep(33000);  // fps 33ms
			break;
		case eDraw_Transform:
			ret = MyProcessorHandle->m_pSampleTransform->OnDrawFrame();
			break;
		case eDraw_Render3D:
			ret = MyProcessorHandle->m_pSampleRender3D->OnDrawFrame();
			break;
		case eDraw_TriangleFBO:
			ret = MyProcessorHandle->m_pSampleDrawFBO->OnDrawFrame();
			break;
		case eDraw_Render3DMesh:
			if (MyProcessorHandle->m_pSampleRender3DMesh)
			{
				MyProcessorHandle->m_pSampleRender3DMesh->SetMotionState(MyProcessorHandle->m_MotionState);
				MyProcessorHandle->m_MotionState.setZero();
				ret = MyProcessorHandle->m_pSampleRender3DMesh->OnDrawFrame();
			}
			break;
		default:
			LOGD("onDrawFrame nDrawType = %d", nDrawType);
			break;
	}
	return ret;
}

int setMotionState (const PHandle pProcessorHandle, MotionState const motionState)
{
	LOGD("setMotionState");
	CHECK_NULL_INPUT (pProcessorHandle)
	LPProcessorHandle MyProcessorHandle = (LPProcessorHandle)pProcessorHandle;

	MyProcessorHandle->m_MotionState.eMotionType = motionState.eMotionType;
	MyProcessorHandle->m_MotionState.transform_x = motionState.transform_x;
	MyProcessorHandle->m_MotionState.transform_y = motionState.transform_y;
	MyProcessorHandle->m_MotionState.transform_z = motionState.transform_z;

	if (eMOTION_TRANSLATE == MyProcessorHandle->m_MotionState.eMotionType)
		MyProcessorHandle->m_MotionState.logMotionState("setMotionState m_MotionState");

	return ERROR_OK;
}

int onSurfaceDestroyed (PHandle *ppProcessorHandle)
{
	LOGD("onSurfaceDestroyed");

	CHECK_NULL_INPUT (*ppProcessorHandle);
	LPProcessorHandle MyProcessorHandle = (LPProcessorHandle)*ppProcessorHandle;

	DestroySampleAndShaderByDrawType (MyProcessorHandle, MyProcessorHandle->m_eDrawType);

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
	SafeFree (MyProcessorHandle);

	return 0;
}