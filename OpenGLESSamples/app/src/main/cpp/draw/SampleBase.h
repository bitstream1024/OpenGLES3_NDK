//
// Created by wcg3031 on 2020/6/4.
//

#pragma once

#include <MotionState.h>
#include "GLES3/gl3.h"

class SampleBase
{
public:
	SampleBase ();
	~SampleBase();
	void CreateGLBuffer ();
	void DestroyGLBuffer ();
	void SetMotionState (MotionState const motionState);
	MotionState GetMotionState ();

private:
	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_EBO;

	GLuint m_TextureId;

	MotionState m_MotionState;
};



