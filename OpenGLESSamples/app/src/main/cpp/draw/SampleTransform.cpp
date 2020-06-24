//
// Created by wcg3031 on 2020/5/21.
//

#include "LogAndroid.h"
#include "common.h"
#include "MyDefineUtils.h"
#include <GLES3/gl3.h>
#include "SampleTransform.h"
#include "DrawHelper.h"

const GLfloat vVertex[24] {
		-0.5f,  0.5f,  0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f
};

/*static const GLfloat vTexture[] {
	1.0f, 1.0f, 1.0f
};*/

const GLuint vIndex[36] {
		0, 1, 2,
		0, 2, 3,
		0, 3, 7,
		0, 4, 7,
		0, 1, 4,
		1, 4, 5,
		1, 2, 6,
		1, 5, 6,
		4, 5, 6,
		4, 6, 7,
		2, 3, 7,
		2, 6, 7
};

SampleTransform::SampleTransform()
{
	LOGD("SampleTransform::SampleTransform");
	m_VAO = GL_NONE;
	m_VBO = GL_NONE;
	m_EBO = GL_NONE;
	for (auto val : m_sVBO)
		val = GL_NONE;
}

SampleTransform::~SampleTransform()
{
	LOGD("~SampleTransform::SampleTransform");
	SafeDeleteGLBuffer (1, &m_VAO);
	SafeDeleteGLBuffer (1, &m_VBO);
	SafeDeleteGLBuffer (1, &m_EBO);
	SafeDeleteGLBuffer (2, m_sVBO);
	m_VAO = m_VBO = m_EBO = GL_NONE;
	for (auto val : m_sVBO)
		val = GL_NONE;
	SafeDelete(m_pShaderHelper);
}

RESULT SampleTransform::createShader()
{
	LOGD("SampleTransform::createShader");
	m_pShaderHelper = new ShaderHelper (triangle_vertex_shader, triangle_fragment_shader);
	RESULT ret = m_pShaderHelper->getShaderHelperState();
	LOGD("createShader getShaderHelperState ret = %d", ret);
	return ret;
}

RESULT SampleTransform::creteGLBuffer ()
{
	LOGD("SampleTransform::creteGLBuffer");

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	glGenBuffers(2, m_sVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_sVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vVertex), vVertex, GL_STATIC_DRAW);
	DrawHelper::CheckGLError("creteGLBuffer glBufferData");

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sVBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vIndex), vIndex, GL_STATIC_DRAW);
	DrawHelper::CheckGLError("creteGLBuffer glBufferData");

	glBindBuffer(GL_ARRAY_BUFFER, m_sVBO[0]);
	DrawHelper::CheckGLError("creteGLBuffer glBindBuffer");
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	DrawHelper::CheckGLError("creteGLBuffer glVertexAttribPointer");
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sVBO[1]);
	glBindVertexArray(GL_NONE);

	return ERROR_OK;
}

RESULT SampleTransform::InitSample ()
{
	LOGD("SampleTransform::InitSample");
	int ret = ERROR_OK;
	createShader();
	CHECK_OK_RETURN(ret)
	ret = creteGLBuffer();
	CHECK_OK_RETURN(ret);
	return ret;
}

void SampleTransform::UnInitSample ()
{
	LOGD("SampleTransform::UnInitSample");
	SafeDeleteGLBuffer (1, &m_VAO);
	SafeDeleteGLBuffer (1, &m_VBO);
	SafeDeleteGLBuffer (1, &m_EBO);
	SafeDeleteGLBuffer (2, m_sVBO);
	m_VAO = m_VBO = m_EBO = GL_NONE;
	for (auto  val : m_sVBO) val = GL_NONE;
	SafeDelete (m_pShaderHelper);
}

RESULT SampleTransform::OnDrawFrame ()
{
	LOGD("SampleTransform::onDrawFrame");
	m_pShaderHelper->use();
	DrawHelper::CheckGLError("OnDrawFrame use");
	glBindVertexArray(m_VAO);
	DrawHelper::CheckGLError("OnDrawFrame glBindVertexArray");
	glDrawElements(GL_TRIANGLES, sizeof(vIndex)/ sizeof(GLuint), GL_UNSIGNED_INT, (void*)0);
	DrawHelper::CheckGLError("OnDrawFrame glDrawElements");
	glBindVertexArray(GL_NONE);
	DrawHelper::CheckGLError("OnDrawFrame glBindVertexArray");
	return ERROR_OK;
}