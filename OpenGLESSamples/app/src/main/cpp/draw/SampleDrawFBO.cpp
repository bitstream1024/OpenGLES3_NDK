//
// Created by wcg3031 on 2020/6/1.
//

#include "SampleDrawFBO.h"

SampleDrawFBO::SampleDrawFBO ()
{
	LOGD("SampleDrawFBO::SampleDrawFBO");
	m_VAO = GL_NONE;
	m_VBO = GL_NONE;
	m_EBO = GL_NONE;
	m_pShaderHelperNormal = nullptr;
	m_Time = 0;
	initMVPMatrix();
}

SampleDrawFBO::~SampleDrawFBO ()
{
	LOGD("SampleDrawFBO::~SampleDrawFBO");
}

RESULT SampleDrawFBO::InitSample()
{
	LOGD("SampleDrawFBO::InitSample");
	RESULT ret = ERROR_OK;
	ret = createShader();
	CHECK_OK_RETURN(ret);
	creteGLBuffer();
	CHECK_OK_RETURN(ret);
	return ret;
}

void SampleDrawFBO::UnInitSample ()
{
	LOGD("SampleDrawFBO::UnInitSample");
	destroyShader();
	destroyGLBuffer ();
}

RESULT SampleDrawFBO::OnDrawFrame ()
{
	MyAutoTimeUtils("SampleDrawFBO::onDrawFrame");

	float angle = 0.f;
	long long currentTime = MyTimeUtils::getCurrentTime();
	if (!m_Time) {
		m_Time = currentTime;
	} else {
		angle = 1.f * (currentTime - m_Time)/1000 * 10; // 15 degrees each 1s
		LOGD("SampleRender3DMesh::OnDrawFrame view angle = %f", angle);
		m_Time = currentTime;
	}
	m_Model = glm::rotate(m_Model, glm::radians(angle), glm::vec3(0.f, 0.f, 1.f));
	LOGOUT_MAT4_MATH (m_Model, "onDrawFrame m_Model")
	LOGOUT_MAT4_MATH (m_Model, "onDrawFrame m_View")
	LOGOUT_MAT4_MATH (m_Projection, "onDrawFrame projection")
	glm::mat4 mvp = m_Projection * m_View * m_Model;
	LOGOUT_MAT4_MATH (mvp, "onDrawFrame mvp")

	// draw FBO
	m_pShaderHelperNormal->use();
	DrawHelper::CheckGLError("OnDrawFrame m_pShaderHelperNormal use");
	m_pShaderHelperNormal->setMat4("mvp", mvp);
	DrawHelper::CheckGLError("OnDrawFrame setMat4");
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glBindVertexArray(m_VAO);
	DrawHelper::CheckGLError("OnDrawFrame glBindVertexArray");
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
	DrawHelper::CheckGLError("OnDrawFrame glDrawElements");
	glBindVertexArray(GL_NONE);
	DrawHelper::CheckGLError("OnDrawFrame glBindVertexArray");

	GLint viewport[4]{0};
	glGetIntegerv(GL_VIEWPORT, viewport);
	SRECT sRect {0};
	sRect.left = viewport[0];sRect.top = viewport[1];sRect.right = viewport[2];sRect.bottom = viewport[3];
	char sPath[MAX_PATH] = "/sdcard/OpenGLESTest/testDrawFBO.png";
	//DrawHelper::SaveRenderImage(sRect, GL_RGB, sPath);

	// draw FBO texture to screen
	m_pShaderHelperFBO->use();
	DrawHelper::CheckGLError("OnDrawFrame m_pShaderHelperFBO use");
	m_pShaderHelperFBO->setMat4("mvp", mvp);
	DrawHelper::CheckGLError("OnDrawFrame m_pShaderHelperFBO setMat4");
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	DrawHelper::CheckGLError("OnDrawFrame glBindFramebuffer");
	glClear (GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, m_FBOTexture);
	DrawHelper::CheckGLError("OnDrawFrame glBindTexture");
	glBindVertexArray(m_VAO);
	DrawHelper::CheckGLError("OnDrawFrame glBindVertexArray");
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
	DrawHelper::CheckGLError("OnDrawFrame glDrawElements");
	glBindVertexArray(GL_NONE);
	DrawHelper::CheckGLError("OnDrawFrame glBindVertexArray");

	return ERROR_OK;
}

RESULT SampleDrawFBO::createShader()
{
	LOGD("SampleDrawFBO::createShader");
	m_pShaderHelperNormal = new ShaderHelper (triangle_vertex_shader0, triangle_fragment_shader0);
	RESULT ret = m_pShaderHelperNormal->getShaderHelperState();
	LOGD("createShader m_pShaderHelperNormal getShaderHelperState ret = %d", ret);

	m_pShaderHelperFBO = new ShaderHelper (triangle_vertex_shader0, triangle_fragment_shader1);
	ret = m_pShaderHelperFBO->getShaderHelperState();
	LOGD("createShader m_pShaderHelperFBO getShaderHelperState ret = %d", ret);
	return ret;
}

void SampleDrawFBO::destroyShader()
{
	LOGD("SampleDrawFBO::destroyShader");
	SafeDelete (m_pShaderHelperNormal);
}

RESULT SampleDrawFBO::creteGLBuffer ()
{
	LOGD("SampleDrawFBO::creteGLBuffer");

	int maxRenderBufferSize[4]{0};
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, maxRenderBufferSize);
	LOGD("creteGLBuffer maxRenderBufferSize (%d, %d, %d, %d)", maxRenderBufferSize[0], maxRenderBufferSize[1],
			maxRenderBufferSize[2], maxRenderBufferSize[3]);

	int viewportSize[4]{0};
	glGetIntegerv(GL_VIEWPORT, viewportSize);
	LOGD("creteGLBuffer viewportSize (%d, %d, %d, %d)", viewportSize[0], viewportSize[1],
			viewportSize[2], viewportSize[3]);
	int screenWidth = viewportSize[2];
	int screenHeight = viewportSize[3];
	glGenFramebuffers(1, &m_FBO);
	GLenum targetFBO = GL_TEXTURE_2D;
	DrawHelper::GetOneTexture(targetFBO, &m_FBOTexture);
	glBindTexture(targetFBO, m_FBOTexture);
	glTexImage2D(targetFBO, GL_FALSE, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(targetFBO, GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, targetFBO, m_FBOTexture, 0);
	GLenum tmpStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (GL_FRAMEBUFFER_COMPLETE != tmpStatus)
	{
		LOGE("creteGLBuffer GL_FRAMEBUFFER error");
		return ERROR_GL_STATE;
	}

	std::vector <float> vertex_multi{
			0.3199f, -0.5f, 0,
			-0.3199f, 0.5f, 0,
			0.3199f, 0.5f, 0,
			-0.3199f, -0.5f, 0
	};
	std::vector<int> index_multi{
			0, 1, 3,
			0, 2, 3
	};

	glGenVertexArrays(1, &m_VAO);
	DrawHelper::CheckGLError("creteGLBuffer glGenBuffers");
	glGenBuffers(1, &m_VBO);
	DrawHelper::CheckGLError("creteGLBuffer glGenBuffers");
	glGenBuffers(1, &m_EBO);
	DrawHelper::CheckGLError("creteGLBuffer glGenBuffers");

	bool bStatic = true;
	long lSize = 0;

	if (bStatic)
	{
		lSize = sizeof(float) * vertex_multi.size();
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, lSize, &vertex_multi[0], GL_STATIC_DRAW);
		DrawHelper::CheckGLError("creteGLBuffer glBufferData");

		lSize = sizeof(int) * index_multi.size();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, lSize, &index_multi[0], GL_STATIC_DRAW);
		DrawHelper::CheckGLError("creteGLBuffer glBufferData");
	}
	else
	{
		lSize = sizeof(float) * m_vertices.size();
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, lSize, &m_vertices[0], GL_STATIC_DRAW);
		LOGD("creteGLBuffer %p %p %p %p %p %p", &m_vertices[0], &m_vertices[1], &m_vertices[2],
			 &m_vertices[3], &m_vertices[4], &m_vertices[5]);
		DrawHelper::CheckGLError("creteGLBuffer glBufferData");

		lSize = sizeof(int) * m_Indices.size();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, lSize, &m_Indices[0], GL_STATIC_DRAW);
		DrawHelper::CheckGLError("creteGLBuffer glBufferData");
	}

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	DrawHelper::CheckGLError("creteGLBuffer glVertexAttribPointer");

	glBindVertexArray(GL_NONE);
	DrawHelper::CheckGLError("creteGLBuffer glBindVertexArray");

	return ERROR_OK;
}

void SampleDrawFBO::destroyGLBuffer ()
{
	LOGD("SampleDrawFBO::DestroyGLBuffer");
	SafeDeleteGLBuffer (1, &m_VAO);
	SafeDeleteGLBuffer (1, &m_VBO);
	SafeDeleteGLBuffer (1, &m_EBO);
	m_VAO = m_VBO = m_EBO = GL_NONE;
}

void SampleDrawFBO::initMVPMatrix ()
{
	LOGD("SampleRender3DMesh::initMVPMatrix");
	m_Model = glm::mat4 (1.f);
	m_View = glm::mat4 (1.f);
	glm::vec3 Position = glm::vec3 (0.f, 0.f, 3.f);
	glm::vec3 Target = glm::vec3 (0.f, 0.f, 0.f);
	glm::vec3 Up = glm::vec3 (0.f, 1.f, 0.f);
	m_View= glm::lookAt(Position, Target, Up);
	m_Projection = glm::perspective (glm::radians(45.f), 1.f, 0.1f, 1000.f);
}