//
// Created by chauncy on 2020/9/29.
//

#include <LogAndroid.h>
#include "ScreenRect.h"
#include "DrawHelper.h"

ScreenRect::ScreenRect()
{
	LOGD("ScreenRect::ScreenRect");
	m_FullRectVAO = GL_NONE;
	m_FullTextureId = GL_NONE;
	m_pFullRectShader = nullptr;
	m_SrcWidth = 0;
	m_SrcHeight = 0;
	bReady = false;
}

ScreenRect::~ScreenRect()
{
	LOGD ("~ScreenRect::ScreenRect");
}

RESULT ScreenRect::CreateRectGLBuffer()
{
	LOGD("ScreenRect::CreateFullRectBuffer begin");
	DrawHelper::CheckGLError("SampleBase::CreateFullRectBuffer begin");

	// create shader
	m_pFullRectShader = new ShaderHelper (FULL_RECT_VERTEX_SHADER, FULL_RECT_FRAGMENT_SHADER);
	int shaderState = m_pFullRectShader->getShaderHelperState();
	if (nullptr == m_pFullRectShader || shaderState != ERROR_OK) {
		LOGE("ScreenRect::CreateFullRectBuffer m_pFullRectShader shaderState = %d", shaderState);
		return ERROR_GL_STATE;
	}

	// create full rect gl ppBuffer
	GLuint VBO[2] = {GL_NONE};
	glGenBuffers(2, VBO);
	GLuint EBO = GL_NONE;
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &m_FullRectVAO);

	glBindVertexArray(m_FullRectVAO);
	int lSize = sizeof(GLfloat) * FULL_RECT_VERTICES.size();
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, lSize, &FULL_RECT_VERTICES[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	DrawHelper::CheckGLError("ScreenRect::CreateFullRectBuffer glBufferData");

	lSize = sizeof(GLfloat) * FULL_RECT_TEXCOORDS.size();
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, lSize, &FULL_RECT_TEXCOORDS[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	DrawHelper::CheckGLError("SampleBase::CreateFullRectBuffer glBufferData");

	lSize = sizeof(GLuint) * RECT_INDICES.size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, lSize, &RECT_INDICES[0], GL_STATIC_DRAW);
	DrawHelper::CheckGLError("ScreenRect::CreateFullRectBuffer glBufferData");

	m_pFullRectShader->use();
	DrawHelper::CheckGLError("ScreenRect::CreateFullRectBuffer m_pFullRectShader use");
	GLint posAttrib  = m_pFullRectShader->getAttribLocation("aPos");
	GLint texCoodsAttrib = m_pFullRectShader->getAttribLocation("aTexCoor");
	LOGD("SampleBase::CreateFullRectBuffer posLocation = %d, texCoodsLocation = %d", posAttrib, texCoodsAttrib);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glEnableVertexAttribArray(posAttrib);
	DrawHelper::CheckGLError("ScreenRect::CreateFullRectBuffer glEnableVertexAttribArray posLocation");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	DrawHelper::CheckGLError("createGLBuffer glVertexAttribPointer posLocation");

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glEnableVertexAttribArray(texCoodsAttrib);
	DrawHelper::CheckGLError("ScreenRect::CreateFullRectBuffer glEnableVertexAttribArray texCoodsLocation");
	glVertexAttribPointer(texCoodsAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
	DrawHelper::CheckGLError("ScreenRect::CreateFullRectBuffer glVertexAttribPointer texCoodsLocation");
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindVertexArray(GL_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
	m_pFullRectShader->useNone();

	SafeDeleteGLBuffers(2, VBO)
	SafeDeleteGLBuffers(1, &EBO);

	bReady = true;

	DrawHelper::CheckGLError("ScreenRect::CreateFullRectBuffer end");
	return 0;
}

void ScreenRect::SetTexture(int textureId, int srcWidth, int srcHeight)
{
	LOGD("ScreenRect::SetTexture textureId = %d, srcWidth = %d, srcHeight = %d", textureId, srcWidth, srcHeight);
	m_FullTextureId = textureId;
	m_SrcWidth = srcWidth;
	m_SrcHeight = srcHeight;
}

void ScreenRect::DrawRect(glm::mat4 mvp)
{
	AUTO_COUNT_TIME_COST("ScreenRect::DrawRect")

	int viewport[4] {0};
	glGetIntegerv(GL_VIEWPORT, viewport);
	int viewportOut[4] {0};
	DrawHelper::ViewportFitInOut(viewport, m_SrcWidth, m_SrcHeight, viewportOut);
	glViewport(viewportOut[0], viewportOut[1], viewportOut[2], viewportOut[3]);

	if (bReady){
		m_pFullRectShader->use();
		m_pFullRectShader->setMat4("mvp", mvp);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_FullTextureId);
		m_pFullRectShader->setInt("screenTexture", 0);
		glBindVertexArray(m_FullRectVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
		DrawHelper::CheckGLError("ScreenRect::DrawRect glDrawElements");
		glBindVertexArray(GL_NONE);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		m_pFullRectShader->useNone();
	} else {
		LOGE("ScreenRect::DrawRect not ready");
	}

	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

RESULT ScreenRect::DestroyRectGLBuffer()
{
	LOGD("ScreenRect::DestroyRectGLBuffer begin");
	DrawHelper::CheckGLError("SampleBase::DestroyRectGLBuffer begin");
	SafeDeleteGLArrays(1, &m_FullRectVAO)
	SafeDelete(m_pFullRectShader)
	SafeDeleteTexture(&m_FullTextureId);
	bReady = false;
	DrawHelper::CheckGLError("SampleBase::DestroyRectGLBuffer end");
	return 0;
}
