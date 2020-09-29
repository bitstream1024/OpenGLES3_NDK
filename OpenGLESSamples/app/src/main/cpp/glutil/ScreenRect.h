//
// Created by wcg3031 on 2020/9/29.
//

#pragma once

#include <common.h>
#include <ShaderHelper.h>
#include <vector>

#define FLOAT_X  1.f
#define FLOAT_Y  FLOAT_X

class ScreenRect
{
public:
	ScreenRect ();
	~ScreenRect();
	RESULT CreateRectGLBuffer ();
	void SetTexture (int textureId, int srcWidth, int srcHeight);
	void DrawRect (glm::mat4 mvp);
	RESULT DestroyRectGLBuffer ();

private:
	ShaderHelper *m_pFullRectShader;
	GLuint m_FullRectVAO;
	GLuint m_FullTextureId;
	int m_SrcWidth;
	int m_SrcHeight;
	bool bReady;

	const std::vector<GLfloat> FULL_RECT_VERTICES {
			-FLOAT_X, -FLOAT_Y, 0.f,
			 FLOAT_X, -FLOAT_Y, 0.f,
			 FLOAT_X,  FLOAT_Y, 0.f,
			-FLOAT_X,  FLOAT_Y, 0.f
	};

	const std::vector<GLfloat> FULL_RECT_TEXCOORDS {
			0.f, 0.f,
			1.f, 0.f,
			1.f, 1.f,
			0.f, 1.f
	};

	const std::vector <GLuint> RECT_INDICES {
			0, 1, 2, 0, 2, 3
	};

	/// TexCoord = vec2(aTexCoor.x, 1.0f - aTexCoor.y);
	/// 因为OpenGL要求y轴0.0坐标是在图片的底部的，但是图片的y轴0.0坐标通常在顶部，所以需要做一个翻转
	const char *FULL_RECT_VERTEX_SHADER =
			GLES_VERSION_STRING
	R"(
in vec3 aPos;
in vec2 aTexCoor;
out vec2 TexCoord;
uniform mat4 mvp;
void main()
{
	gl_Position = mvp * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoor.x, 1.0f - aTexCoor.y);
}
)";

	const char *FULL_RECT_FRAGMENT_SHADER =
			GLES_VERSION_STRING
	GLES_MEDIUM_STRING
	R"(
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D screenTexture;
void main()
{
	FragColor = texture (screenTexture, TexCoord);
}
)";

	const char *FULL_TRI_VERTEX_SHADER =
			GLES_VERSION_STRING
			R"(
layout (location = 0) in vec3 aPos;
void main()
{
	gl_Position = vec4(aPos, 1.0);
}
)";

	const char *FULL_TRI_FRAGMENT_SHADER =
			GLES_VERSION_STRING
	GLES_MEDIUM_STRING
	R"(
out vec4 FragColor;
void main()
{
	FragColor = vec4 (1.f, 0.f, 0.f, 1.f);
}
)";
};



