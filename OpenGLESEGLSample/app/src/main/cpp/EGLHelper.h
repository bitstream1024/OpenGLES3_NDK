//
// Created by wcg3031 on 2020/7/30.
//

#pragma once

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3ext.h>
#include <MyImageInfo.h>
#include "ShaderHelper.h"

#define GL_VBO_MAX	5

class EGLHelper
{
public:
	// 单例
	static EGLHelper* CreateInstance ();
	int Init ();
	int SetWindow (ANativeWindow * const pNativeWindow, const int windowWith, const int windowHeight);
	int UnInit ();
	int Draw ();
	int SetImageData (const int imgWidth, const int imgHeight, const unsigned char* pImgData);

private:
	EGLHelper ();
	~EGLHelper ();
	int CreateEGLEnv ();
	int DestroyEGLEnv ();

	int createShader();
	void destroyShader();
	int creteGLBuffer ();
	void destroyGLBuffer ();
	void generateSurfaceFrame();

	static EGLHelper *m_EGLHelper;
	EGLDisplay m_EGLDisplay;
	EGLConfig m_EGLConfig;
	EGLSurface m_EGLSurface;
	EGLContext m_EGLContext;
	bool m_bEGLEnvReady;

	ANativeWindow *m_pANativeWindow;
	PFNEGLPRESENTATIONTIMEANDROIDPROC m_pfneglPresentationTimeANDROID;

	GLuint m_VAO;
	GLuint m_VBO;
	//GLuint m_VBO[GL_VBO_MAX];
	GLuint m_EBO;
	GLuint m_FBO;
	GLuint m_FBOTexture;
	ShaderHelper* m_pShaderHelperNormal;
	ShaderHelper* m_pShaderHelperFBO;

	int m_WindowWidth;
	int m_WindowHeight;
	int m_ImgFormat;
	MyImageInfo m_RenderImg;

	int m_FrameID;
};



