//
// Created by bitstream1024 on 2020/6/1.
//

#include <unistd.h>
#include "SampleDrawFBO.h"

SampleDrawFBO::SampleDrawFBO ()
{
	LOGD("SampleDrawFBO::SampleDrawFBO");
	m_VAO_fbo = GL_NONE;
	m_EBO_fbo = GL_NONE;
	m_pShaderHelperNormal = nullptr;
	m_pShaderHelperFBO = nullptr;
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
	RESULT ret = NONE_ERROR;
	ret = createShader();
	CHECK_OK_RETURN(ret);
	createGLBuffer();
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
	AUTO_COUNT_TIME_COST("SampleDrawFBO::onDrawFrame");

	float angle = 0.f;
	long long currentTime = MyTimeUtils::GetCurrentTime();
	if (!m_Time) {
		m_Time = currentTime;
	} else {
		angle = 1.f * (currentTime - m_Time)/1000 * 10; // 15 degrees each 1s
		LOGD("SampleDrawFBO::OnDrawFrame view angle = %f", angle);
		m_Time = currentTime;
	}
	m_Model = glm::rotate(m_Model, glm::radians(angle), glm::vec3(0.f, 0.f, 1.f));
	LOGOUT_MAT4_MATH (m_Model, "onDrawFrame m_Model")
	LOGOUT_MAT4_MATH (m_View, "onDrawFrame m_View")
	LOGOUT_MAT4_MATH (m_Projection, "onDrawFrame projection")
	glm::mat4 mvp = m_Projection * m_View * m_Model;
	LOGOUT_MAT4_MATH (mvp, "onDrawFrame mvp")

	mvp = m_Model;
	// draw FBO
	m_pShaderHelperNormal->use();
	DrawHelper::CheckGLError("OnDrawFrame m_pShaderHelperNormal use");
	m_pShaderHelperNormal->setMat4("mvp", mvp);
	DrawHelper::CheckGLError("OnDrawFrame setMat4");
	glm::vec3 aColor = glm::vec3 (1.f, 0.f, 0.f);
	m_pShaderHelperNormal->setVec3f("aColor", aColor.x, aColor.y, aColor.z);
	DrawHelper::CheckGLError("OnDrawFrame setVec3f");
	/*glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	DrawHelper::CheckGLError("OnDrawFrame glPixelStorei");*/
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	DrawHelper::CheckGLError("OnDrawFrame glBindFramebuffer");

	// 在绑定GL_FRAMEBUFFER之后再clear颜色缓存，否则绘制会叠加上次的绘制结果
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.f, 0.f, 0.f, 1.f);

	glBindVertexArray(m_VAO);
	DrawHelper::CheckGLError("OnDrawFrame glBindVertexArray");
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
	DrawHelper::CheckGLError("OnDrawFrame glDrawElements");
	glBindVertexArray(GL_NONE);
	DrawHelper::CheckGLError("OnDrawFrame glBindVertexArray");

	// add draw test
	LOGD("SampleDrawFBO::onDrawFrame 2 angle = %f", angle);
	m_Model2 = glm::rotate(m_Model2, glm::radians(-2.f * angle), glm::vec3 (0.f, 0.f, 1.f));
	glm::mat4 mvp2 = m_Model2;
	LOGOUT_MAT4_MATH(mvp2, "onDrawFrame mvp 2");
	m_pShaderHelperNormal->setMat4("mvp", mvp2);
	glm::vec3 aColor2 = glm::vec3 (0.f, 1.f, 0.f);
	m_pShaderHelperNormal->setVec3f("aColor", aColor2.x, aColor2.y, aColor2.z);
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(GL_NONE);


	// if save render texture
	if (false) {
		GLint viewport[4]{0};
		glGetIntegerv(GL_VIEWPORT, viewport);
		KitRect sRect{0};
		sRect.left = viewport[0];
		sRect.top = viewport[1];
		sRect.right = viewport[2];
		sRect.bottom = viewport[3];
		char sPath[MAX_PATH]{0};
		sprintf(sPath, "/sdcard/OpenGLESTest/read/testDrawFBO_%04d.png", mFrameNum);
		DrawHelper::SaveRenderImage(sRect, GL_RGBA, sPath);
	}


	// draw FBO texture to screen
	m_pShaderHelperFBO->use();
	DrawHelper::CheckGLError("OnDrawFrame m_pShaderHelperFBO use");
	glm::mat4 mvp_bg = glm::mat4(1.f);
	m_pShaderHelperFBO->setMat4("mvp", mvp_bg);
	DrawHelper::CheckGLError("OnDrawFrame m_pShaderHelperFBO setMat4");
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	DrawHelper::CheckGLError("OnDrawFrame glBindFramebuffer");
	glClear (GL_COLOR_BUFFER_BIT);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glBindVertexArray(m_VAO_fbo);
	DrawHelper::CheckGLError("OnDrawFrame glBindVertexArray");
	glActiveTexture(GL_TEXTURE0);
	DrawHelper::CheckGLError("OnDrawFrame glActiveTexture");
	glBindTexture(GL_TEXTURE_2D, m_FBOTexture);
	DrawHelper::CheckGLError("OnDrawFrame glBindTexture");
	m_pShaderHelperFBO->setInt("screenTexture", 0);
	DrawHelper::CheckGLError("OnDrawFrame m_pShaderHelperFBO setInt");
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
	DrawHelper::CheckGLError("OnDrawFrame glDrawElements");
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	DrawHelper::CheckGLError("OnDrawFrame glBindTexture");
	glBindVertexArray(GL_NONE);
	DrawHelper::CheckGLError("OnDrawFrame glBindVertexArray");

	++mFrameNum;
	return NONE_ERROR;
}

int SampleDrawFBO::GetDrawTexture()
{
	return m_FBOTexture;
}

RESULT SampleDrawFBO::createShader()
{
	LOGD("SampleDrawFBO::createShader");
	m_pShaderHelperNormal = new ShaderHelper (triangle_vertex_shader0, triangle_fragment_shader0);
	RESULT ret = m_pShaderHelperNormal->getShaderHelperState();
	LOGD("createShader m_pShaderHelperNormal getShaderHelperState ret = %d", ret);

	m_pShaderHelperFBO = new ShaderHelper (fbo_vertex_shader, fbo_fragment_shader);
	ret = m_pShaderHelperFBO->getShaderHelperState();
	LOGD("createShader m_pShaderHelperFBO getShaderHelperState ret = %d", ret);
	return ret;
}

void SampleDrawFBO::destroyShader()
{
	LOGD("SampleDrawFBO::destroyShader");
	SafeDelete (m_pShaderHelperNormal)
	SafeDelete (m_pShaderHelperFBO)
}

RESULT SampleDrawFBO::createGLBuffer ()
{
	LOGD("SampleDrawFBO::createGLBuffer");

	int maxRenderBufferSize[4]{0};
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, maxRenderBufferSize);
	LOGD("createGLBuffer maxRenderBufferSize (%d, %d, %d, %d)", maxRenderBufferSize[0], maxRenderBufferSize[1],
			maxRenderBufferSize[2], maxRenderBufferSize[3]);

	int viewportSize[4]{0};
	glGetIntegerv(GL_VIEWPORT, viewportSize);
	LOGD("createGLBuffer viewportSize (%d, %d, %d, %d)", viewportSize[0], viewportSize[1],
			viewportSize[2], viewportSize[3]);
	int screenWidth = viewportSize[2];
	int screenHeight = viewportSize[3];
	glGenFramebuffers(1, &m_FBO);
	const GLenum TARGET_FBO = GL_TEXTURE_2D;
	DrawHelper::GetOneTexture(TARGET_FBO, &m_FBOTexture);
	glBindTexture(TARGET_FBO, m_FBOTexture);
	glTexImage2D(TARGET_FBO, GL_FALSE, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(TARGET_FBO, GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TARGET_FBO, m_FBOTexture, 0);
	glBindTexture(TARGET_FBO, GL_NONE);
	GLenum tmpStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (GL_FRAMEBUFFER_COMPLETE != tmpStatus)
	{
		LOGE("createGLBuffer GL_FRAMEBUFFER error");
		return ERROR_GL_STATE;
	}
	std::vector <float> vertex_multi{
		0.f, 0.f , 0.f,
		0.5f, 0.f, 0.f,
		0.5f, 0.75f, 0.f,
		0.f, 0.75f, 0.f
	};
	/*std::vector <float> vertex_multi{
			-1.f, -1.f, 0,
			 1.f, -1.f, 0,
			 1.f,  1.f, 0,
			-1.f,  1.f, 0
	};*/

	std::vector<unsigned int> index_multi{
			0, 1, 2,
			0, 1, 3
	};

	glGenVertexArrays(1, &m_VAO);
	DrawHelper::CheckGLError("createGLBuffer glGenBuffers");
	glGenBuffers(1, &m_VBO);
	DrawHelper::CheckGLError("createGLBuffer glGenBuffers");
	glGenBuffers(1, &m_EBO);
	DrawHelper::CheckGLError("createGLBuffer glGenBuffers");

	long lSize = 0;
	lSize = sizeof(float) * vertex_multi.size();
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, lSize, &vertex_multi[0], GL_STATIC_DRAW);
	DrawHelper::CheckGLError("createGLBuffer glBufferData");
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	lSize = sizeof(unsigned int) * index_multi.size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, lSize, &index_multi[0], GL_STATIC_DRAW);
	DrawHelper::CheckGLError("createGLBuffer glBufferData");

	m_pShaderHelperNormal->use();
	//GLuint posLocationNor = m_pShaderHelperNormal->getAttribLocation("aPos");
	GLuint posLocationNor = 0;
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glEnableVertexAttribArray(posLocationNor);
	glVertexAttribPointer(posLocationNor, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	DrawHelper::CheckGLError("createGLBuffer glVertexAttribPointer");

	glBindVertexArray(GL_NONE);
	DrawHelper::CheckGLError("createGLBuffer glBindVertexArray");

	std::vector <float> vertex_fbo{
			-1.f, -1.f, 0,
			 1.f, -1.f, 0,
			 1.f,  1.f, 0,
			-1.f,  1.f, 0
	};

	// FrameBuffer中texture坐标相反
	std::vector <float> texCoors {
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f
	};

	std::vector<unsigned int> index_fbo{
			0, 1, 2,
			0, 2, 3
	};
	GLuint VBO_fbo[2] = {GL_NONE};
	glGenBuffers(2, VBO_fbo);
	glGenBuffers(1, &m_EBO_fbo);
	glGenVertexArrays(1, &m_VAO_fbo);

	glBindVertexArray(m_VAO_fbo);
	lSize = sizeof(float) * vertex_fbo.size();
	glBindBuffer(GL_ARRAY_BUFFER, VBO_fbo[0]);
	glBufferData(GL_ARRAY_BUFFER, lSize, &vertex_fbo[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	DrawHelper::CheckGLError("createGLBuffer glBufferData");

	lSize = sizeof(float) * texCoors.size();
	glBindBuffer(GL_ARRAY_BUFFER, VBO_fbo[1]);
	glBufferData(GL_ARRAY_BUFFER, lSize, &texCoors[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	DrawHelper::CheckGLError("createGLBuffer glBufferData");

	lSize = sizeof(unsigned int) * index_fbo.size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO_fbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, lSize, &index_fbo[0], GL_STATIC_DRAW);
	DrawHelper::CheckGLError("createGLBuffer glBufferData");

	m_pShaderHelperFBO->use();
	DrawHelper::CheckGLError("createGLBuffer m_pShaderHelperFBO use");
	//GLint posAttrib  = m_pShaderHelperFBO->getAttribLocation("aPos");
	//GLint texCoodsAttrib = m_pShaderHelperFBO->getAttribLocation("aTex");
	//LOGD("createGLBuffer posLocation = %d, texCoodsLocation = %d", posAttrib, texCoodsAttrib);

	GLint posAttrib = 0;
	glBindBuffer(GL_ARRAY_BUFFER, VBO_fbo[0]);
	glEnableVertexAttribArray(posAttrib);
	DrawHelper::CheckGLError("createGLBuffer glEnableVertexAttribArray posLocation");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	DrawHelper::CheckGLError("createGLBuffer glVertexAttribPointer posLocation");

	GLint texCoodsAttrib = 1;
	glBindBuffer(GL_ARRAY_BUFFER, VBO_fbo[1]);
	glEnableVertexAttribArray(texCoodsAttrib);
	DrawHelper::CheckGLError("createGLBuffer glEnableVertexAttribArray texCoodsLocation");
	glVertexAttribPointer(texCoodsAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	DrawHelper::CheckGLError("createGLBuffer glVertexAttribPointer texCoodsLocation");
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindVertexArray(GL_NONE);

	SafeDeleteGLBuffers(2, VBO_fbo);

	return NONE_ERROR;
}

void SampleDrawFBO::destroyGLBuffer ()
{
	LOGD("SampleDrawFBO::DestroyGLBuffer");
	SafeDeleteGLArrays (1, &m_VAO)
	SafeDeleteGLBuffers (1, &m_VBO)
	SafeDeleteGLBuffers (1, &m_EBO)
	SafeDeleteGLArrays (1, &m_VAO_fbo)
	SafeDeleteGLBuffers (1, &m_EBO_fbo)

	SafeDeleteTexture(&m_FBOTexture)
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

	m_Model2 = glm::mat4(1.f);
}

RESULT SampleDrawFBO::OnDrawFrameRect()
{
	AUTO_COUNT_TIME_COST("SampleDrawFBO::OnDrawFrameRect")

	std::vector <GLfloat> vertex {
		-0.5f, -0.5f, 0,
		 0.5f, -0.5f, 0,
		 0.5f,  0.5f, 0,
		-0.5f,  0.5f, 0,
	};

	std::vector<GLuint > indices {
		0, 1, 2,
		0, 1, 3
	};

	GLuint VAO = GL_NONE, EBO = GL_NONE;
	GLuint VBO = GL_NONE;
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	DrawHelper::CheckGLError("OnDrawFrameRect glBindVertexArray VAO");

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	DrawHelper::CheckGLError("OnDrawFrameRect glBindBuffer GL_ARRAY_BUFFER");
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex.size(), &vertex[0], GL_STATIC_DRAW);
	DrawHelper::CheckGLError("OnDrawFrameRect glBufferData GL_ARRAY_BUFFER");
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);
	DrawHelper::CheckGLError("OnDrawFrameRect glBufferData GL_ELEMENT_ARRAY_BUFFER");

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	DrawHelper::CheckGLError("OnDrawFrameRect glVertexAttribPointer");
	glBindVertexArray(GL_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);

	glm::mat4 mvp = glm::mat4(1.f);
	m_pShaderHelperNormal->use();
	m_pShaderHelperNormal->setMat4("mvp", mvp);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	DrawHelper::CheckGLError("OnDrawFrameRect glDrawElements");
	glBindVertexArray(GL_NONE);

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);

	return 0;
}
