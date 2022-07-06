//
// Created by chauncy on 2020/4/21.
//

#pragma once

#include "GLES3/gl3.h"
#include "GLES2/gl2ext.h"
#include "EGL/egl.h"
#include "OpenImageHelper.h"
#include "NativeImageUtils.h"

#define SafeDeleteGLBuffers(_num_, _pVal_)	\
if (_pVal_) {    \
	glDeleteBuffers (_num_, _pVal_);	\
    for (int i = 0; i < _num_; ++i) {	\
        *(_pVal_ + i) = GL_NONE;	\
    }	\
}

#define SafeDeleteGLArrays(_num_, _pVal_)	\
if (_pVal_) {    \
	glDeleteVertexArrays (_num_, _pVal_);	\
    for (int i = 0; i < _num_; ++i) {	\
        *(_pVal_ + i) = GL_NONE;	\
    }	\
}

#define SafeDeleteTexture(_pTexture_)	if (_pTexture_) {glDeleteTextures(1, _pTexture_); *_pTexture_ = GL_NONE;}

#define IF_LOGOUT_MAT_INFO	0
#define LOGOUT_MAT4(_mat4_,_info_)		\
	if (IF_LOGOUT_MAT_INFO) {	\
		for (int i = 0; i < 4; ++i) { \
			LOGD ("%s (%f, %f, %f, %f)", _info_, _mat4_[i][0], _mat4_[i][1], _mat4_[i][2], _mat4_[i][3]); \
		}\
	}

#define LOGOUT_MAT4_MATH(_mat4_,_info_)		\
	if (IF_LOGOUT_MAT_INFO) {	\
		for (int i = 0; i < 4; ++i) { \
			LOGD ("%s MATH (%f, %f, %f, %f)", _info_, _mat4_[0][i], _mat4_[1][i], _mat4_[2][i], _mat4_[3][i]); \
		}\
	}

#define GL_CHECK_ERROR(_info_)	DrawHelper::CheckGLError(__FILE__, __LINE__, _info_)

class DrawHelper
{
public:
	static void CheckGLError(const char *TAG)
	{
		GLenum errorCode = glGetError();
		if (GL_NO_ERROR != errorCode) {
			std::string errorInfo = "";
			switch (errorCode) {
				case GL_INVALID_ENUM: 					errorInfo = "GL_INVALID_ENUM";	break;
				case GL_INVALID_VALUE: 					errorInfo = "GL_INVALID_VALUE";	break;
				case GL_INVALID_OPERATION: 				errorInfo = "GL_INVALID_OPERATION";	break;
				case GL_OUT_OF_MEMORY: 					errorInfo = "GL_OUT_OF_MEMORY";	break;
				case GL_INVALID_FRAMEBUFFER_OPERATION: 	errorInfo = "GL_INVALID_FRAMEBUFFER_OPERATION";	break;
				default:								break;
			}
			LOGE("%s CheckGLError errorCode = %d, errorInfo = %s", TAG, errorCode, errorInfo.c_str());
		}
	}

	static void CheckGLError(const char * file, int line, const char *TAG)
	{
		GLenum errorCode = glGetError();
		if (GL_NO_ERROR != errorCode) {
			std::string errorInfo = "";
			switch (errorCode) {
				case GL_INVALID_ENUM: 					errorInfo = "GL_INVALID_ENUM";	break;
				case GL_INVALID_VALUE: 					errorInfo = "GL_INVALID_VALUE";	break;
				case GL_INVALID_OPERATION: 				errorInfo = "GL_INVALID_OPERATION";	break;
				case GL_OUT_OF_MEMORY: 					errorInfo = "GL_OUT_OF_MEMORY";	break;
				case GL_INVALID_FRAMEBUFFER_OPERATION: 	errorInfo = "GL_INVALID_FRAMEBUFFER_OPERATION";	break;
				default:								break;
			}
			LOGE("file: %s, line: %d, %s CheckGLError errorCode = %d, errorInfo = %s", file, line,
					TAG, errorCode, errorInfo.c_str());
		}
	}

	static void CheckEGLError(const char *TAG)
	{
		EGLint error = eglGetError();
		if (EGL_SUCCESS != error)
			LOGE("%s CheckEGLError error_code = %d", TAG, error);
	}

    static void GetOneTexture(const GLenum target, GLuint *pTexture)
    {
        LOGD("getOneTexture target = %d", target);
        glGenTextures(1, pTexture);
        glBindTexture(target, *pTexture);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (GL_TEXTURE_2D == target)
        {
            glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
        else if (GL_TEXTURE_EXTERNAL_OES == target)
        {
            glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        glBindTexture(target, GL_NONE);
        DrawHelper::CheckGLError("GetOneTexture");
    }

    static void ConvertNativeImageFormatToGL(const int &nativeFormat, GLenum &glFormat)
    {
	    switch (nativeFormat)
        {
            case MY_FORMAT_RGB32:
                glFormat = GL_RGBA;
                break;
            case MY_FORMAT_RGB24:
                glFormat = GL_RGB;
                break;
            case MY_FORMAT_NV21:
                glFormat = GL_LUMINANCE;
                break;
            default:
                glFormat = GL_RGBA;
                break;
        }
    }

    static void UpdateTexture(const MyImageInfo *pSrcImg, const GLuint &nTextureID, const int &nTextureType = -1)
    {
        AUTO_COUNT_TIME_COST("DrawHelper::UpdateTexture");

        LOGD("UpdateTexture nTextureID = %d, nTextureType = %d", nTextureID, nTextureType);
        GLenum TEXTURE_TARGET = GL_TEXTURE_2D;
        if (nTextureType > 0)
        {
            TEXTURE_TARGET = nTextureType;
        }
        NativeImageUtils::PrintNativeImageInfo(pSrcImg);
        if (nTextureID > 0 && NativeImageUtils::IsNativeImageValid(pSrcImg))
        {
            GLenum glFormat = GL_RGBA;
            ConvertNativeImageFormatToGL(pSrcImg->format, glFormat);
            glBindTexture(TEXTURE_TARGET, nTextureID);
            glTexImage2D(TEXTURE_TARGET, 0, glFormat, pSrcImg->width, pSrcImg->height,
                         0, glFormat, GL_UNSIGNED_BYTE, pSrcImg->ppBuffer[0]);
            glBindTexture(TEXTURE_TARGET, GL_NONE);
        }
    }

	static void SaveRenderImage (const SRECT sRect, const GLenum format, std::string sPath)
	{
		LOGD("SaveRenderImage");
		LOGD("SaveRenderImage sRect = (%d,%d,%d,%d), format = %d, sPath = %s", sRect.left, sRect.top,
				sRect.right, sRect.bottom, format, sPath.c_str());
		int width = sRect.right - sRect.left;
		int height = sRect.bottom - sRect.top;
		MyImageInfo myImageInfo {0};
		myImageInfo.width = width;
		myImageInfo.height = height;
		switch (format)
		{
			case GL_RGB:
				myImageInfo.format = MY_FORMAT_RGB24;
				break;
			case GL_RGBA:
				myImageInfo.format = MY_FORMAT_RGB32;
				break;
			default:
				myImageInfo.format = MY_FORMAT_RGB24;
				break;
		}
		NativeImageUtils::AssignNativeImagePitchByWidth(&myImageInfo);
        NativeImageUtils::AllocNativeImage(&myImageInfo);
		START_TIME ("SaveRenderImage glReadPixels")
			glReadPixels(0, 0, width, height, format, GL_UNSIGNED_BYTE, myImageInfo.ppBuffer[0]);
		STOP_TIME ("SaveRenderImage glReadPixels")
		OpenImageHelper::ExchangeImageCoordinateY(&myImageInfo);
		OpenImageHelper::SaveImageToPng(&myImageInfo, sPath.c_str());
		OpenImageHelper::FreeMyImageInfo(&myImageInfo);
	}

	static void ViewportFitInOut (const int viewportIn[4], const int texWidth, int texHeight, int viewportOut[4])
	{
		LOGD("DrawHelper::ViewportFitInOut viewportIn (%d,%d,%d,%d) texWidth=%d texHeight=%d", viewportIn[0],
				viewportIn[1], viewportIn[2], viewportIn[3], texWidth, texHeight);
		float ratio = 1.f * texHeight /texWidth;
		int viewHeight = static_cast<int>(viewportIn[2] * ratio);
		viewportOut[0] = viewportIn[0];
		viewportOut[1] = (viewportIn[3] - viewHeight)/2;
		viewportOut[2] = viewportIn[2];
		viewportOut[3] = viewHeight;
		LOGD("DrawHelper::ViewportFitInOut viewportOut (%d,%d,%d,%d)", viewportOut[0], viewportOut[1],
				viewportOut[2], viewportOut[3]);
	}
};

