//
// Created by chauncy on 2020/9/28.
//

#include "SampleTexture.h"
#include "DrawHelper.h"

static void UpdateTextureFuncAsync(SampleTexture* pSample)
{
    AUTO_COUNT_TIME_COST("UpdateTextureFuncAsync");
    // update texture
    LPMyImageInfo lpMyImageInfo = pSample->GetMyImageInfo();
    unsigned int frameID = pSample->GetFrameID();

    MyImageInfo testImageInfo {0};
    testImageInfo.width = lpMyImageInfo->width;
    testImageInfo.height = lpMyImageInfo->height;
    testImageInfo.format = lpMyImageInfo->format;
    memcpy(testImageInfo.wPitch, lpMyImageInfo->wPitch, 4 * sizeof(int));
    memcpy(testImageInfo.hPitch, lpMyImageInfo->hPitch, 4 * sizeof(int));
    unsigned int size = NativeImageUtils::AllocNativeImage(&testImageInfo);
    LOGV("UpdateTextureFuncAsync AllocNativeImage nRet = %u", size);
    //NativeImageUtils::CopyNativeImageToDst(lpMyImageInfo, &testImageInfo);

    while(true) {

        std::unique_lock<std::mutex> locker(pSample->m_TextureMutex);
        LOGV("UpdateTextureFuncAsync locker in");
        if (pSample->m_bTextureReady)
        {
            LOGV("UpdateTextureFuncAsync m_TextureUsedCondition wait");
            pSample->m_TextureReadyConditionVariable.wait(locker);
        }

        /*if (NativeImageUtils::IsNativeImageValid(&testImageInfo) && MY_FORMAT_RGB32 == testImageInfo.format
            && frameID < testImageInfo.height)
        {
            memset(testImageInfo.ppBuffer[0], 0, testImageInfo.wPitch[0] * frameID);
        }*/
        pSample->UpdateTexture(&testImageInfo);

        ++frameID;

        pSample->m_bTextureReady = true;
        LOGV("UpdateTextureFuncAsync m_TextureReadyCondition notify_one");
        pSample->m_TextureReadyConditionVariable.notify_one();

        if (pSample->m_bExitThread)
            break;
    }

    NativeImageUtils::FreeNativeImage(&testImageInfo);
}

static void UpdateTextureFuncSync(SampleTexture* pSample)
{
    AUTO_COUNT_TIME_COST("UpdateTextureFuncSync");
    // update texture
    LPMyImageInfo lpMyImageInfo = pSample->GetMyImageInfo();
    unsigned int frameID = pSample->GetFrameID();

    MyImageInfo testImageInfo {0};
    testImageInfo.width = lpMyImageInfo->width;
    testImageInfo.height = lpMyImageInfo->height;
    testImageInfo.format = lpMyImageInfo->format;
    memcpy(testImageInfo.wPitch, lpMyImageInfo->wPitch, 4 * sizeof(int));
    memcpy(testImageInfo.hPitch, lpMyImageInfo->hPitch, 4 * sizeof(int));
    NativeImageUtils::AllocNativeImage(&testImageInfo);

    NativeImageUtils::CopyNativeImageToDst(lpMyImageInfo, &testImageInfo);
    if (NativeImageUtils::IsNativeImageValid(&testImageInfo) && MY_FORMAT_RGB32 == testImageInfo.format && frameID < testImageInfo.height)
    {
        memset(testImageInfo.ppBuffer[0], 0, testImageInfo.wPitch[0] * frameID);
    }
    pSample->UpdateTexture(&testImageInfo);

    NativeImageUtils::FreeNativeImage(&testImageInfo);
    //++frameID;
    //pSample->UpdateFrameID(frameID);
}


SampleTexture::SampleTexture(): m_bThreadExist(false), m_bTextureReady(false), m_bExitThread(false)
{
	m_VAO = GL_NONE;
	m_TextureID = GL_NONE;
	m_TextureShaderHelper = nullptr;
	memset(&m_TextureImage, 0, sizeof(MyImageInfo));
	m_pScreenRect = new ScreenRect();
}

SampleTexture::~SampleTexture()
{
	SafeDelete(m_pScreenRect)
}

RESULT SampleTexture::SetImageInfo(MyImageInfo *const pSrc)
{
	LOGD("SampleTexture::SetImageInfo");
	memcpy(&m_TextureImage, pSrc, sizeof(MyImageInfo));

	return 0;
}

RESULT SampleTexture::Create()
{
	LOGD("SampleTexture::Create begin");
	int retCode = ERROR_OK;
	do {
		if (m_pScreenRect) {
			retCode = m_pScreenRect->CreateRectGLBuffer();
			if (retCode != ERROR_OK) {
				LOGE("SampleTexture::Create CreateRectGLBuffer retCode = %d", retCode);
				break;
			}
		}
		// create texture
		DrawHelper::GetOneTexture(GL_TEXTURE_2D, &m_TextureID);
		if (nullptr != m_TextureImage.ppBuffer[0]) {
			glBindTexture(GL_TEXTURE_2D, m_TextureID);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, m_TextureImage.width, m_TextureImage.height,
						  0, GL_RGBA, GL_UNSIGNED_BYTE, m_TextureImage.ppBuffer[0]);
			DrawHelper::CheckGLError("SampleTexture::Create glTexImage2D");
			glGenerateMipmap (GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, GL_NONE);
			m_pScreenRect->SetTexture(m_TextureID, m_TextureImage.width, m_TextureImage.height);
		} else {
			LOGE("SampleTexture::Create m_TextureImage error");
			retCode = ERROR_IMAGE;
			break;
		}
	} while (false);

	return retCode;
}

RESULT SampleTexture::Destroy()
{
    LOGD("SampleTexture::Destroy");

    m_bExitThread = true;

    SafeDeleteTexture(&m_TextureID)
    SafeDeleteGLArrays(1, &m_VAO)
    SafeDelete(m_TextureShaderHelper)
    OpenImageHelper::FreeMyImageInfo(&m_TextureImage);
    m_pScreenRect->DestroyRectGLBuffer();
    return 0;
}

RESULT SampleTexture::UpdateTexture(MyImageInfo *const pSrcImg)
{
    if (nullptr == pSrcImg || nullptr == pSrcImg->ppBuffer[0]) {
        LOGE("UpdateTexture input image is not valid!!!");
        return ERROR_INPUT;
    }
    DrawHelper::UpdateTexture(pSrcImg, m_TextureID, GL_TEXTURE_2D);
    return 0;
}

RESULT SampleTexture::DrawFrame()
{
	AUTO_COUNT_TIME_COST("SampleTexture::DrawFrame");

	glClearColor(1.f, 0.5f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    UpdateTextureFuncSync(this);

	glm::mat4 mvp (1.f);
	if(m_pScreenRect) {
		m_pScreenRect->DrawRect(mvp);
	}

    UpdateFrameID();

	return 0;
}

RESULT SampleTexture::DrawFrameAsync()
{
    AUTO_COUNT_TIME_COST("SampleTexture::DrawFrameAsync");

    // create new thread to update texture for test
    if (!this->m_bThreadExist)
    {
        std::thread tTextureThread(UpdateTextureFuncAsync, this);
        tTextureThread.detach();
        this->m_bThreadExist = true;
    }

    std::unique_lock<std::mutex> locker(m_TextureMutex);
    LOGV("SampleTexture::DrawFrameAsync locker in");
    if (!m_bTextureReady)
    {
        LOGV("SampleTexture::DrawFrameAsync m_TextureReadyCondition wait");
        m_TextureReadyConditionVariable.wait(locker);
    }

    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glm::mat4 mvp (1.f);
    if(m_pScreenRect) {
        m_pScreenRect->DrawRect(mvp);
    }

    m_bTextureReady = false;
    LOGV("SampleTexture::DrawFrameAsync m_TextureReadyCondition notify_one");
    m_TextureReadyConditionVariable.notify_one();

    return 0;
}

MyImageInfo *SampleTexture::GetMyImageInfo()
{
    return &m_TextureImage;
}

GLuint SampleTexture::GetTextureID()
{
    return m_TextureID;
}

int SampleTexture::GetFrameID()
{
    return m_FrameID;
}

void SampleTexture::UpdateFrameID(int frameID)
{
    if (frameID < 0) ++ this->m_FrameID;
    else this->m_FrameID = frameID;

}

