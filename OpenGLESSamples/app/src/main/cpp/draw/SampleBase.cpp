//
// Created by wcg3031 on 2020/6/4.
//

#include "LogAndroid.h"
#include "SampleBase.h"
#include "DrawHelper.h"

SampleBase::SampleBase()
{
	LOGD("SampleBase::SampleBase");
	m_VAO = GL_NONE;
	m_VBO = GL_NONE;
	m_EBO = GL_NONE;
	m_TextureId = GL_NONE;
}

SampleBase::~SampleBase()
{
	LOGD("SampleBase::~SampleBase");
	SafeDeleteGLBuffer(1, &m_VAO);
	SafeDeleteGLBuffer(1, &m_VBO);
	SafeDeleteGLBuffer(1, &m_EBO);
	SafeDeleteGLBuffer(1, &m_TextureId);
}

void SampleBase::CreateGLBuffer ()
{
	LOGD("SampleBase::CreateGLBuffer");
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);
	DrawHelper::GetOneTexture(GL_TEXTURE_2D, &m_TextureId);
}

void SampleBase::DestroyGLBuffer ()
{
	LOGD("SampleBase::DestroyGLBuffer");
	SafeDeleteGLBuffer(1, &m_VAO);
	SafeDeleteGLBuffer(1, &m_VBO);
	SafeDeleteGLBuffer(1, &m_EBO);
	SafeDeleteGLBuffer(1, &m_TextureId);
}

void SampleBase::SetMotionState (MotionState const motionState)
{
	LOGD("SampleBase::SetMotionState");
	m_MotionState.eMotionType = motionState.eMotionType;
	m_MotionState.transform_x = motionState.transform_x;
	m_MotionState.transform_y = motionState.transform_y;
	m_MotionState.transform_z = motionState.transform_z;
}

MotionState SampleBase::GetMotionState ()
{
	LOGD("SampleBase::GetMotionState");
	return m_MotionState;
}