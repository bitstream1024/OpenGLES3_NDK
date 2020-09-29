//
// Created by wcg3031 on 2020/9/28.
//

#include "SampleTexture.h"
#include "DrawHelper.h"

SampleTexture::SampleTexture()
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
	/*OpenImageHelper::PrintMyImageInfo(pSrc);
	m_TextureImage.width = pSrc->width;
	m_TextureImage.height = pSrc->height;
	m_TextureImage.format = pSrc->format;
	memcpy(m_TextureImage.channel, pSrc->channel, 4 * sizeof(int));
	OpenImageHelper::AllocMyImageInfo(&m_TextureImage);
	memcpy(m_TextureImage.buffer[0], m_TextureImage.buffer[0], )*/
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
		if (nullptr != m_TextureImage.buffer[0]) {
			glBindTexture(GL_TEXTURE_2D, m_TextureID);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, m_TextureImage.width, m_TextureImage.height,
						  0, GL_RGBA, GL_UNSIGNED_BYTE, m_TextureImage.buffer[0]);
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

RESULT SampleTexture::DrawFrame()
{
	CAL_TIME_COST("SampleTexture::DrawFrame")

	glClearColor(1.f, 1.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glm::mat4 mvp (1.f);
	if(m_pScreenRect) {
		m_pScreenRect->DrawRect(mvp);
	}

	return 0;
}

RESULT SampleTexture::Destroy()
{
	LOGD("SampleTexture::Destroy");

	SafeDeleteTexture(&m_TextureID)
	SafeDeleteGLArrays(1, &m_VAO)
	SafeDelete(m_TextureShaderHelper)
	OpenImageHelper::FreeMyImageInfo(&m_TextureImage);
	m_pScreenRect->DestroyRectGLBuffer();
	return 0;
}

