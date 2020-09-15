//
// Created by wcg3031 on 2020/6/1.
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
	int GetDrawTexture ();

private:
	RESULT createShader();
	void destroyShader();
	RESULT creteGLBuffer ();
	void destroyGLBuffer ();
	void initMVPMatrix ();

	std::vector <float> m_vertices;
	std::vector <int> m_Indices;

	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_EBO;
	GLuint m_FBO;
	GLuint m_FBOTexture;
	ShaderHelper *m_pShaderHelperNormal;
	ShaderHelper *m_pShaderHelperFBO;

	const char *triangle_vertex_shader0 =
			GLES_VERSION_STRING
	R"(
layout (location = 0) in vec3 aPos;
uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(aPos, 1.0);
}
)";

	const char *triangle_fragment_shader0 =
			GLES_VERSION_STRING
	GLES_MEDIUM_STRING
	R"(
out vec4 FragColor;
void main()
{
	FragColor = vec4(1.0f, 0.0f, 0.2f, 1.0f);
}
)";

	const char *triangle_fragment_shader1 =
			GLES_VERSION_STRING
			GLES_MEDIUM_STRING
			R"(
out vec4 FragColor;
void main()
{
	FragColor = vec4(0.0f, 1.0f, 0.2f, 1.0f);
}
)";

};



