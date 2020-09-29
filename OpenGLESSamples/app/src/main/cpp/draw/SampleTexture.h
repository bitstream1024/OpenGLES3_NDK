//
// Created by wcg3031 on 2020/9/28.
//

#pragma once

#include <common.h>
#include <ShaderHelper.h>
#include <MyImageInfo.h>
#include "SampleBase.h"
#include "ScreenRect.h"

class SampleTexture: public SampleBase
{
public:
	SampleTexture();
	~SampleTexture();
	RESULT SetImageInfo (MyImageInfo *const pSrc);
	RESULT Create ();
	RESULT DrawFrame ();
	RESULT Destroy ();

private:
	GLuint m_VAO;
	GLuint m_TextureID;
	ShaderHelper *m_TextureShaderHelper;
	MyImageInfo m_TextureImage;

	ScreenRect* m_pScreenRect;
};



