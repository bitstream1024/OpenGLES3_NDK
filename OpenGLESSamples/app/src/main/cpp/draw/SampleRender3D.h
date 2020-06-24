//
// Created by wcg3031 on 2020/5/22.
//

#pragma once

#include "ShaderHelper.h"
#include "common.h"
#include "MyDefineUtils.h"
#include "barHelper.h"

#include "glm/vec3.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

struct Vertex1
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 PointColor;
};

class SampleRender3D
{
public:
	SampleRender3D ();
	~SampleRender3D ();

	RESULT InitSample ();
	void UnInitSample ();
	RESULT OnDrawFrame ();

private:
	RESULT createShader();
	void destroyShader();
	RESULT creteGLBuffer ();
	void destroyGLBuffer ();
	RESULT createRectBars ();
	RESULT convertVertex ();
	void loadBarMesh ();
	void generateRecBarsTest(
			SimpleMesh& mesh,
			Vector3D<float>& center,
			float radius,
			float height,
			float width,
			int num_bars,
			Vector3D<unsigned char> default_color = Vector3D<unsigned char>(255, 0, 0));

	void generateTorus(SimpleMesh& mesh, Vector3D<float>& center, float torus_radius, float tube_radius);

	SimpleMesh m_SimpleMesh;
	SimpleMesh m_SimpleMeshTorus;
	std::vector <Vertex1> m_VertexLists;

	std::vector <float> m_vertices;
	std::vector <int> m_Indices;

	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_EBO;
	ShaderHelper *m_pShaderHelper;
	GLuint m_VBO_Position;
	GLuint m_VBO_Normal;
	GLuint m_VBO_Color;

	// "layout (location = 1) in vec3 aColor;\n"
	const char triangle_vertex_shader[MAX_CONTENT] = "#version 300 es\n"
													 "layout (location = 0) in vec3 aPos;\n"
													 "void main()\n"
													 "{\n"
													 "    gl_Position = vec4(aPos, 1.0);\n"
													 "}";

	const char triangle_fragment_shader[MAX_CONTENT] = "#version 300 es\n"
													   "precision mediump float;\n"
													   "out vec4 FragColor;\n"
													   "\n"
													   "void main()\n"
													   "{\n"
													   "    FragColor = vec4(1.0f, 0.0f, 0.2f, 1.0f);\n"
													   "}";

	const char *triangle_vertex_shader1 =
			GLES_VERSION_STRING
			R"(
layout (location = 0) in vec3 aPos;
void main()
{
	gl_Position = vec4(aPos, 1.0);
}
)";

	const char *triangle_fragment_shader1 =
			GLES_VERSION_STRING
			GLES_MEDIUM_STRING
			R"(
out vec4 FragColor;
void main()
{
	FragColor = vec4(1.0f, 0.0f, 0.2f, 1.0f);
}
)";

};




