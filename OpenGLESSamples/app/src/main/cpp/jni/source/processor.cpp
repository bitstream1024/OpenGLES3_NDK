//
// Created by bitstream1024 on 2020/4/1.
//

#include "processor_inner.h"
#include "KitLogUtils.h"
#include "GLES3/gl3.h"
#include <string>
#include <SampleDrawYUV.h>
#include "KitCommonDefine.h"
#include "OpenImageHelper.h"
#include "SampleRender3D.h"
#include "shader_content.h"
#include "unistd.h"
#include "draw_utils.h"
#include "config.h"

int CreateSampleAndShaderByDrawType (const PHandle pProcessorHandle, SampleType drawType)
{
	LOGD("CreateSampleAndShaderByDrawType drawType = %d", drawType);
	AUTO_COUNT_TIME_COST("CreateSampleAndShaderByDrawType")
	CHECK_NULL_INPUT(pProcessorHandle)
	auto MyProcessorHandle = (LPProcessorHandle)pProcessorHandle;
	int ret = NONE_ERROR;
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
		case eDraw_Texture:
			MyProcessorHandle->m_pSampleTexture = (SampleTexture*)SampleFactory::CreateSample(drawType);
			MyProcessorHandle->m_pSampleTexture->SetImageInfo(MyProcessorHandle->m_pImageRGBA);
			MyProcessorHandle->m_pSampleTexture->Create();
            break;
		case eDraw_YUV:
			if (!MyProcessorHandle->m_pSampleRenderYUV)
            {
				MyProcessorHandle->m_pSampleRenderYUV = (SampleDrawYUV*)SampleFactory::CreateSample(drawType);
				MyProcessorHandle->m_pSampleRenderYUV->SetImageYuvResource(MyProcessorHandle->m_pImageYUV);
				MyProcessorHandle->m_pSampleRenderYUV->InitSample();
			}
			break;
        case eDraw_YUV16Bit:
			if (!MyProcessorHandle->m_pSampleRenderYUV16Bit)
            {
				MyProcessorHandle->m_pSampleRenderYUV16Bit = (SampleRender16Bit*)SampleFactory::CreateSample(drawType);
                MyProcessorHandle->m_pSampleRenderYUV16Bit->SetImage(
                        MyProcessorHandle->m_pImageP010);
				MyProcessorHandle->m_pSampleRenderYUV16Bit->InitSample();
			}
			break;
        case eDraw_Text:
            if (!MyProcessorHandle->m_pSampleRenderText)
            {
                MyProcessorHandle->m_pSampleRenderText = (SampleTextRender*) SampleFactory::CreateSample(drawType);
                MyProcessorHandle->m_pSampleRenderText->Init();
            }
            break;
		default:
			LOGD("CreateSampleAndShaderByDrawType nDrawType = %d is unsupported", drawType);
			break;
	}
	return NONE_ERROR;
}

int DestroySampleAndShaderByDrawType (const PHandle pProcessorHandle, SampleType drawType)
{
	LOGD("DestroySampleAndShaderByDrawType drawType = %d", drawType);
	CHECK_NULL_INPUT(pProcessorHandle)
	auto MyProcessorHandle = (LPProcessorHandle)pProcessorHandle;
	int ret = NONE_ERROR;

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
		case eDraw_Texture:
			if (nullptr != MyProcessorHandle->m_pSampleTexture) {
				MyProcessorHandle->m_pSampleTexture->Destroy();
				SafeDelete(MyProcessorHandle->m_pSampleTexture)
			}
		case eDraw_YUV:
			if (MyProcessorHandle->m_pSampleRenderYUV)
				MyProcessorHandle->m_pSampleRenderYUV->UnInitSample();
			SafeDelete(MyProcessorHandle->m_pSampleRenderYUV)
			break;
        case eDraw_YUV16Bit:
            if (MyProcessorHandle->m_pSampleRenderYUV16Bit) {
                MyProcessorHandle->m_pSampleRenderYUV16Bit->UnInitSample();
                SafeDelete(MyProcessorHandle->m_pSampleRenderYUV16Bit)
            }
            break;
        case eDraw_Text:
			if (MyProcessorHandle->m_pSampleRenderText)
				MyProcessorHandle->m_pSampleRenderText->UnInit();
			SafeDelete(MyProcessorHandle->m_pSampleRenderText)
			break;
		default:
			LOGD("onDrawFrame nDrawType = %d is unsupported", drawType);
			break;
	}
	return NONE_ERROR;
}

int CreateProcessor (PHandle *ppProcessorHandle)
{
	AUTO_COUNT_TIME_COST("CreateProcessor")

	if (nullptr == ppProcessorHandle || nullptr != *ppProcessorHandle)
	{
		LOGE("onSurfaceCreated ppProcessorHandle is NULL");
		return ERROR_INPUT;
	}

	*ppProcessorHandle = (LPProcessorHandle)malloc (sizeof (ProcessorHandle));
	CHECK_NULL_MALLOC (*ppProcessorHandle);
	memset(*ppProcessorHandle, 0, sizeof(ProcessorHandle));

	return NONE_ERROR;
}

int SetupResource (PHandle const pProcessorHandle)
{
	AUTO_COUNT_TIME_COST("SetupResource")

	LOGD("SetupResource pProcessorHandle = %p", pProcessorHandle);

	CHECK_NULL_INPUT (pProcessorHandle)
	auto pMyHandle = (LPProcessorHandle)pProcessorHandle;
	int retCode = NONE_ERROR;

    std::string strPNGImgPath = std::string(ROOT_FOLDER).append(std::string(STR_IMAGE_PNG_DOG));
    std::string strNV21ImgPath = std::string(ROOT_FOLDER).append(std::string(STR_IMAGE_NV12_DOG));
    std::string strGray10ImgPath = std::string(ROOT_FOLDER).append(std::string(STR_IMAGE_GRAY_10BIT_DOG));
    std::string strP010ImgPath = std::string(ROOT_FOLDER).append(std::string(STR_IMAGE_P010_LSB_DOG));

	do {
        pMyHandle->m_pImageRGBA = (LPKitImage)malloc(sizeof(KitImage));
		CHECK_NULL_MALLOC(pMyHandle->m_pImageRGBA);
		memset (pMyHandle->m_pImageRGBA, 0 , sizeof(KitImage));
		retCode = OpenImageHelper::LoadPngFromFile(strPNGImgPath.c_str(), pMyHandle->m_pImageRGBA);
        CHECK_OK_BREAK(retCode, "SetupResource LoadYuvImageFromFile strPNGImgPath");

# if 0
        KitImageUtils::SaveYuvImageToFile(pMyHandle->m_pImageRGBA, "/sdcard/OpenGLESTest/dog_1_1280x1920.RGBA32");
#endif

        pMyHandle->m_pImageYUV = (LPKitImage)malloc(sizeof(KitImage));
		CHECK_NULL_MALLOC(pMyHandle->m_pImageYUV);
		memset(pMyHandle->m_pImageYUV, 0, sizeof(KitImage));
		retCode = KitImageUtils::LoadYuvImageFromFile(strNV21ImgPath.c_str(), pMyHandle->m_pImageYUV);
        CHECK_OK_BREAK(retCode, "SetupResource LoadYuvImageFromFile strNV21ImgPath");

        pMyHandle->m_pImageGray10 = (LPKitImage)malloc(sizeof(KitImage));
        CHECK_NULL_MALLOC(pMyHandle->m_pImageGray10);
        memset(pMyHandle->m_pImageGray10, 0, sizeof(KitImage));
        retCode = KitImageUtils::LoadYuvImageFromFile(strGray10ImgPath.c_str(), pMyHandle->m_pImageGray10);
        CHECK_OK_BREAK(retCode, "SetupResource LoadYuvImageFromFile strGray10ImgPath");

        pMyHandle->m_pImageP010 = (LPKitImage)malloc(sizeof(KitImage));
        CHECK_NULL_MALLOC(pMyHandle->m_pImageP010);
        memset(pMyHandle->m_pImageP010, 0, sizeof(KitImage));
        retCode = KitImageUtils::LoadYuvImageFromFile(strP010ImgPath.c_str(), pMyHandle->m_pImageP010);
        CHECK_OK_BREAK(retCode, "SetupResource LoadYuvImageFromFile strP010ImgPath");

#if 0
        char szPath[MAX_PATH] {0};
        snprintf(szPath, sizeof(szPath) - 1, "/sdcard/OpenGLESTest/temp_0_%dx%d.a",
                 pMyHandle->m_pImageGray10->width, pMyHandle->m_pImageGray10->height);
        KitImageUtils::SaveYuvImageToFile(pMyHandle->m_pImageGray10, szPath);
#endif // for test
	} while (false);

	return retCode;
}

int DestroyProcessor (PHandle *ppProcessorHandle)
{
	LOGD("DestroyProcessor pProcessorHandle = %p", *ppProcessorHandle);

	CHECK_NULL_INPUT (*ppProcessorHandle)
	auto MyProcessorHandle = (LPProcessorHandle)(*ppProcessorHandle);

    OpenImageHelper::FreeMyImageInfo(MyProcessorHandle->m_pImageRGBA);
    SafeFree(MyProcessorHandle->m_pImageRGBA);
    OpenImageHelper::FreeMyImageInfo(MyProcessorHandle->m_pImageYUV);
    SafeFree(MyProcessorHandle->m_pImageYUV);
    OpenImageHelper::FreeMyImageInfo(MyProcessorHandle->m_pImageGray10);
    SafeFree(MyProcessorHandle->m_pImageGray10);
    OpenImageHelper::FreeMyImageInfo(MyProcessorHandle->m_pImageP010);
    SafeFree(MyProcessorHandle->m_pImageP010);

	SafeFree (*ppProcessorHandle);
	LOGD("onSurfaceDestroyed *ppProcessorHandle = %p", *ppProcessorHandle);

	return NONE_ERROR;
}

int onSurfaceCreated (PHandle const pProcessorHandle, const int effectType)
{
	AUTO_COUNT_TIME_COST("onSurfaceCreated")

	LOGD("onSurfaceCreated pProcessorHandle = %p", pProcessorHandle);

	CHECK_NULL_INPUT (pProcessorHandle)
	auto MyProcessorHandle = (LPProcessorHandle)pProcessorHandle;

	/// set which sample you want to get
	LOGD("onSurfaceCreated effectType = %d", effectType);
	MyProcessorHandle->m_eDrawType = (SampleType) effectType;
	int ret = CreateSampleAndShaderByDrawType(MyProcessorHandle, MyProcessorHandle->m_eDrawType);
	LOGD("CreateSampleAndShaderByDrawType ret = %d", ret);

	return NONE_ERROR;
}

int onSurfaceChanged (PHandle const pProcessorHandle, const int width, const int height)
{
	LOGD("onSurfaceChanged");
	glViewport(0, 0, width, height);
	return 0;
}

int onDrawFrame (PHandle const pProcessorHandle)
{
	//LOGD("processor onDrawFrame begin");
	//LOGD("onDrawFrame pProcessorHandle = %p", pProcessorHandle);

	CHECK_NULL_INPUT (pProcessorHandle)
	auto MyProcessorHandle = (LPProcessorHandle)pProcessorHandle;
	++MyProcessorHandle->mRenderTime;

	int ret = NONE_ERROR;

	SampleType nDrawType = MyProcessorHandle->m_eDrawType;
    //LOGD("processor onDrawFrame nDrawType=%d", nDrawType);
	switch (nDrawType)
	{
		case eDraw_Triangle:
			ret = drawTriangle (MyProcessorHandle->mShaderSetTriangle.pShaderHelper);
			LOGD("onDrawFrame drawTriangle ret = %d", ret);
			break;
		case eDraw_SimpleTexture:
			ret = drawTexture(MyProcessorHandle->mShaderSetTexture.pShaderHelper, MyProcessorHandle->m_pImageRGBA);
			LOGD("onDrawFrame drawTexture ret = %d", ret);
			break;
		case eDraw_TextureFBO:
			ret = drawFBO(MyProcessorHandle->mShaderSetFBO.pShaderHelper, MyProcessorHandle->mShaderSetFBONormal.pShaderHelper,
						  MyProcessorHandle->m_pImageRGBA);
			LOGD("onDrawFrame drawFBO ret = %d", ret);
			break;
		case eDraw_HardwareBuffer:
			ret = drawByHardwareBuffer(MyProcessorHandle->pHardwareBufferHelper, MyProcessorHandle->m_pImageRGBA);
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
		case eDraw_Texture:
			if (MyProcessorHandle->m_pSampleTexture) {
                MyProcessorHandle->m_pSampleTexture->DrawFrame();
                //MyProcessorHandle->m_pSampleTexture->DrawFrameAsync();
            }
            break;
		case eDraw_YUV:
			if (MyProcessorHandle->m_pSampleRenderYUV) {
				MyProcessorHandle->m_pSampleRenderYUV->OnDrawFrame();
			}
			break;
        case eDraw_YUV16Bit:
            if (MyProcessorHandle->m_pSampleRenderYUV16Bit) {
                MyProcessorHandle->m_pSampleRenderYUV16Bit->OnDrawFrame();
            }
            break;
        case eDraw_Text:
            if (MyProcessorHandle->m_pSampleRenderText) {
                MyProcessorHandle->m_pSampleRenderText->Draw();
            }
            break;
		default:
			LOGD("onDrawFrame nDrawType = %d", nDrawType);
			break;
	}

	//LOGD("processor onDrawFrame end");

	return ret;
}

int getTextureFromFrameBuffer (PHandle const pProcessorHandle)
{
	LOGD("getTextureFromFrameBuffer");
	if (nullptr == pProcessorHandle) {
		return -1;
	}
	LPProcessorHandle MyProcessorHandle = (LPProcessorHandle)pProcessorHandle;
	return MyProcessorHandle->m_pSampleDrawFBO->GetDrawTexture();
}

int setMotionState (PHandle const pProcessorHandle, MotionState const motionState)
{
	//LOGD("setMotionState");
	CHECK_NULL_INPUT (pProcessorHandle)
	auto MyProcessorHandle = (LPProcessorHandle)pProcessorHandle;

	MyProcessorHandle->m_MotionState.eMotionType = motionState.eMotionType;
	MyProcessorHandle->m_MotionState.transform_x = motionState.transform_x;
	MyProcessorHandle->m_MotionState.transform_y = motionState.transform_y;
	MyProcessorHandle->m_MotionState.transform_z = motionState.transform_z;

	if (eMOTION_TRANSLATE == MyProcessorHandle->m_MotionState.eMotionType)
		MyProcessorHandle->m_MotionState.logMotionState("setMotionState m_MotionState");

	return NONE_ERROR;
}

int onSurfaceDestroyed (PHandle const pProcessorHandle)
{
	LOGD("processor onSurfaceDestroyed = %p", pProcessorHandle);

	CHECK_NULL_INPUT (pProcessorHandle)
	auto MyProcessorHandle = (LPProcessorHandle)pProcessorHandle;

	int retCode = DestroySampleAndShaderByDrawType (MyProcessorHandle, MyProcessorHandle->m_eDrawType);
	if (NONE_ERROR != retCode) {
		LOGE("processor onSurfaceDestroyed retCode = %d", retCode);
	}

	return retCode;
}