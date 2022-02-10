//
// Created by chauncy on 2020/6/1.
//

#pragma once

#include "GLES3/gl3.h"
#include "ShaderHelper.h"
#include "DrawHelper.h"
#include "SampleBase.h"
#include <vector>

//using namespace std;

class SampleDrawFBO : public SampleBase
{
public:
	SampleDrawFBO ();
	~SampleDrawFBO ();

	RESULT InitSample ();
	void UnInitSample ();
	RESULT OnDrawFrame ();
	RESULT OnDrawFrameRect ();
	int GetDrawTexture ();

private:
	RESULT createShader();
	void destroyShader();
	RESULT createGLBuffer ();
	void destroyGLBuffer ();
	void initMVPMatrix ();

	std::vector <float> m_vertices;
	std::vector <int> m_Indices;

	GLuint m_VAO_fbo;
	GLuint m_EBO_fbo;

	GLuint m_FBO;
	GLuint m_FBOTexture;
	ShaderHelper *m_pShaderHelperNormal;
	ShaderHelper *m_pShaderHelperFBO;

	glm::mat4 m_Model2;

	const char *triangle_vertex_shader0 =
			GLES_VERSION_STRING
	R"(
layout (location=0) in vec3 aPos;
uniform mat4 mvp;
uniform vec3 aColor;
out vec3 textColor;

void main()
{
	gl_Position = mvp * vec4(aPos, 1.0);
	textColor = aColor;
}
)";

	const char *triangle_fragment_shader0 =
			GLES_VERSION_STRING
	GLES_MEDIUM_STRING
	R"(
in vec3 textColor;
out vec4 FragColor;
void main()
{
	FragColor = vec4(textColor.x, textColor.y, textColor.z, 1.0f);
}
)";


	const char *fbo_vertex_shader =
			GLES_VERSION_STRING
	R"(
layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aTex;
out vec2 TexCoords;
uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(aPos, 1.0);
	TexCoords = aTex;
}
)";

	const char *fbo_fragment_shader =
			GLES_VERSION_STRING
			GLES_MEDIUM_STRING
			R"(
in vec2 TexCoords;
out vec4 FragColor;
uniform sampler2D screenTexture;

void main()
{
	vec4 tempColor = texture (screenTexture, TexCoords);
	//float grayValue = tempColor.r * 0.299 + tempColor.g * 0.587 + tempColor.b * 0.114;
	//FragColor = vec4 (vec3(grayValue), tempColor.a);
	FragColor = vec4 (tempColor);
}
)";

};



