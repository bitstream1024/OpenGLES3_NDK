//
// Created by wcg3031 on 2020/6/4.
//

#pragma once

#include <MotionState.h>
#include "GLES3/gl3.h"
#include "glm/glm.hpp"

class SampleBase
{
public:
	SampleBase ();
	~SampleBase();
	void CreateGLBuffer ();
	void DestroyGLBuffer ();
	void SetMotionState (MotionState const motionState);
	MotionState GetMotionState ();

	long long m_Time;
	glm::mat4 m_Model;
	glm::mat4 m_View;
	glm::mat4 m_Projection;

private:
	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_EBO;

	GLuint m_TextureId;

	MotionState m_MotionState;
};



