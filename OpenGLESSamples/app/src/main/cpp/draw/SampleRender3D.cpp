//
// Created by wcg3031 on 2020/5/22.
//

#include "SampleRender3D.h"
#include "LogAndroid.h"
#include "common.h"
#include "MyDefineUtils.h"
#include "DrawHelper.h"

SampleRender3D::SampleRender3D()
{
	LOGD("SampleRender3D::SampleRender3D");
	m_VAO = GL_NONE;
	m_VBO = GL_NONE;
	m_EBO = GL_NONE;
	m_VBO_Position = GL_NONE;
	m_VBO_Normal = GL_NONE;
	m_VBO_Color = GL_NONE;
	m_pShaderHelper = nullptr;
}

SampleRender3D::~SampleRender3D()
{
	LOGD("~SampleRender3D::SampleRender3D");
}

RESULT SampleRender3D::InitSample ()
{
	LOGD("SampleRender3D::InitSample");
	RESULT ret = ERROR_OK;
	ret = createShader();
	CHECK_OK_RETURN(ret);
	createRectBars();
	CHECK_OK_RETURN(ret);
	creteGLBuffer();
	CHECK_OK_RETURN(ret);
	return ret;
}

void SampleRender3D::UnInitSample ()
{
	LOGD("SampleRender3D::UnInitSample");
	destroyShader();
	destroyGLBuffer ();
}

RESULT SampleRender3D::OnDrawFrame ()
{
	LOGD("SampleRender3D::onDrawFrame");

	glm::mat4 modelView = glm::mat4(1.0f);
	glm::mat4 projection = glm::perspective (glm::radians(45.f), 1.f, 0.1f, 1000.f);
	LOGOUT_MAT4 (projection, "onDrawFrame projection")

	m_pShaderHelper->use();
	DrawHelper::CheckGLError("OnDrawFrame use");
	glBindVertexArray(m_VAO);
	DrawHelper::CheckGLError("OnDrawFrame glBindVertexArray");
	//glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, (void*)0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
	DrawHelper::CheckGLError("OnDrawFrame glDrawElements");
	glBindVertexArray(GL_NONE);
	DrawHelper::CheckGLError("OnDrawFrame glBindVertexArray");
	return ERROR_OK;
}

RESULT SampleRender3D::createShader()
{
	LOGD("SampleRender3D::createShader");
	m_pShaderHelper = new ShaderHelper (triangle_vertex_shader1, triangle_fragment_shader1);
	RESULT ret = m_pShaderHelper->getShaderHelperState();
	LOGD("createShader getShaderHelperState ret = %d", ret);
	return ret;
}

void SampleRender3D::destroyShader()
{
	LOGD("SampleRender3D::destroyShader");
	SafeDelete (m_pShaderHelper);
}

RESULT SampleRender3D::creteGLBuffer ()
{
	LOGD("SampleRender3D::creteGLBuffer");
	std::vector <float> vertex_multi{
			0.3199f, -0.05f, 0,
			-0.3199f, 0.05f, 0,
			0.3199f, 0.05f, 0,
			-0.3199f, -0.05f, 0
	};
	std::vector<int> index_multi{
			0, 1, 2,
			2, 3, 0
	};

	glGenVertexArrays(1, &m_VAO);
	DrawHelper::CheckGLError("creteGLBuffer glGenBuffers");
	glGenBuffers(1, &m_VBO);
	DrawHelper::CheckGLError("creteGLBuffer glGenBuffers");
	glGenBuffers(1, &m_EBO);
	DrawHelper::CheckGLError("creteGLBuffer glGenBuffers");
	/*glGenBuffers(1, &m_VBO_Position);
	DrawHelper::CheckGLError("creteGLBuffer glGenBuffers");
	glGenBuffers(1, &m_VBO_Normal);
	DrawHelper::CheckGLError("creteGLBuffer glGenBuffers");
	glGenBuffers(1, &m_VBO_Color);
	DrawHelper::CheckGLError("creteGLBuffer glGenBuffers");*/

	bool bStatic = true;
	long lSize = 0;

	if (bStatic)
	{
		lSize = sizeof(float) * vertex_multi.size();
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, lSize, &vertex_multi[0], GL_STATIC_DRAW);
		DrawHelper::CheckGLError("creteGLBuffer glBufferData");

		lSize = sizeof(int) * index_multi.size();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, lSize, &index_multi[0], GL_STATIC_DRAW);
		DrawHelper::CheckGLError("creteGLBuffer glBufferData");
	}
	else
	{
		loadBarMesh();
		lSize = sizeof(float) * m_vertices.size();
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, lSize, &m_vertices[0], GL_STATIC_DRAW);
		LOGD("creteGLBuffer %p %p %p %p %p %p", &m_vertices[0], &m_vertices[1], &m_vertices[2],
				&m_vertices[3], &m_vertices[4], &m_vertices[5]);
		DrawHelper::CheckGLError("creteGLBuffer glBufferData");

		lSize = sizeof(int) * m_Indices.size();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, lSize, &m_Indices[0], GL_STATIC_DRAW);
		DrawHelper::CheckGLError("creteGLBuffer glBufferData");
	}

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	DrawHelper::CheckGLError("creteGLBuffer glVertexAttribPointer");

	//glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

	glBindVertexArray(GL_NONE);
	DrawHelper::CheckGLError("creteGLBuffer glBindVertexArray");

	return ERROR_OK;
}

void SampleRender3D::destroyGLBuffer ()
{
	LOGD("SampleRender3D::DestroyGLBuffer");
	SafeDeleteGLBuffer (1, &m_VAO);
	SafeDeleteGLBuffer (1, &m_VBO);
	SafeDeleteGLBuffer (1, &m_VBO_Position);
	SafeDeleteGLBuffer (1, &m_VBO_Normal);
	SafeDeleteGLBuffer (1, &m_VBO_Color);
	SafeDeleteGLBuffer (1, &m_EBO);
	m_VAO = m_VBO = m_VBO_Position = m_VBO_Normal = m_VBO_Color = m_EBO = GL_NONE;
}

RESULT SampleRender3D::createRectBars ()
{
	LOGD("SampleRender3D::createRectBars");
	Vector3D<float> bar_center(0.f, 0.f, 0.05f);
	float bar_radius = 0.32f, bar_height = 0.2f, bar_width = 0.1f;
	int num_bars = 16;
	generateRecBarsTest(m_SimpleMesh, bar_center, bar_radius, bar_height, bar_width, num_bars);
	float torus_radius = 1.f, tube_radius = 0.5f;
	generateTorus(m_SimpleMeshTorus, bar_center, torus_radius, tube_radius);
	return ERROR_OK;
}

void SampleRender3D::loadBarMesh ()
{
	LOGD("SampleRender3D::loadMesh");
	long size = m_SimpleMesh.vertices.size();
	//size = 4;
	for (int i = 0; i < size; ++i)
	{
		m_vertices.push_back(m_SimpleMesh.vertices[i].operator[](0));
		m_vertices.push_back(m_SimpleMesh.vertices[i].operator[](1));
		m_vertices.push_back(0);
	}

	size = m_SimpleMesh.faces.size();
	//size = 2;
	for (int i = 0; i < size; ++i)
	{
		m_Indices.push_back(m_SimpleMesh.faces[i].operator[](0));
		m_Indices.push_back(m_SimpleMesh.faces[i].operator[](1));
		m_Indices.push_back(m_SimpleMesh.faces[i].operator[](2));
	}
}

RESULT SampleRender3D::convertVertex ()
{
	LOGD("SampleRender3D::convertVertex");
	// get vertex
	Vertex1 tVertex;
	for (int i = 0; i < m_SimpleMesh.vertices.size(); ++i)
	{
		glm::vec3 aPosition ((float)(m_SimpleMesh.vertices[i][0]), m_SimpleMesh.vertices[i][1], m_SimpleMesh.vertices[i][2]);
		tVertex.Position = aPosition;
		//glm::vec3 aNormal (m_SimpleMesh.normals[i][0], m_SimpleMesh.normals[i][1], m_SimpleMesh.normals[i][2]);
		glm::vec3 aNormal (0, 0, 0);
		tVertex.Normal = aNormal;
		glm::vec3 aColor (1.0f, 1.0f, 1.0f);
		tVertex.PointColor = aColor;
		m_VertexLists.push_back(tVertex);
	}

	for (int i = 0; i < m_SimpleMesh.faces.size(); ++i)
	{
		m_Indices.push_back(m_SimpleMesh.faces[i][0]);
		m_Indices.push_back(m_SimpleMesh.faces[i][1]);
		m_Indices.push_back(m_SimpleMesh.faces[i][2]);
	}

	return ERROR_OK;
}

void SampleRender3D::generateRecBarsTest(
		SimpleMesh& mesh,
		Vector3D<float>& center,
		float radius,
		float height,
		float width,
		int num_bars,
		Vector3D<unsigned char> default_color)
{
	float half_height = height / 2.f, half_width = width / 2.f;
	Vector3D<float> points[4] = {
			Vector3D<float>(radius, -half_width, -half_height),
			Vector3D<float>(radius, half_width, -half_height),
			Vector3D<float>(radius, half_width, half_height),
			Vector3D<float>(radius, -half_width, half_height)
	};

	mesh.vertices.clear();
	mesh.faces.clear();
	float delta_theta = M_PI * 2.f / float(num_bars);
	for (size_t k = 0; k < num_bars; k++) {
		float theta = k * delta_theta;
		float cos = cosf(theta);
		float sin = sinf(theta);
		for (size_t i = 0; i < 4; i++) {
			Vector3D<float> p;
			p[0] = cos * points[i][0] - sin * points[i][1];
			p[1] = sin * points[i][0] + cos * points[i][1];
			p[2] = points[i][2];
			mesh.vertices.push_back(center + p);
			mesh.colors.push_back(default_color);
		}
		mesh.faces.push_back(Vector3D<int>(4 * k, 4 * k + 1, 4 * k + 2));
		mesh.faces.push_back(Vector3D<int>(4 * k + 2, 4 * k + 3, 4 * k));
	}
}

void SampleRender3D::generateTorus(SimpleMesh& mesh, Vector3D<float>& center, float torus_radius, float tube_radius)
{
	LOGD("SampleRender3D::generateTorus center (%f, %f, %f), torus_radius = %f, tube_radius = %f",
			center[0], center[1], center[2], torus_radius, tube_radius);

	const int radial_resolution = 20;
	const int tubular_resolution = 20;

	mesh.vertices.resize(radial_resolution * tubular_resolution);
	mesh.colors.resize(radial_resolution * tubular_resolution);
	mesh.faces.resize(2 * radial_resolution * tubular_resolution);
	auto vert_idx = [&](int uidx, int vidx) {
		return uidx * tubular_resolution + vidx;
	};
	double u_step = 2 * M_PI / double(radial_resolution);
	double v_step = 2 * M_PI / double(tubular_resolution);
	for (int uidx = 0; uidx < radial_resolution; ++uidx) {
		double u = uidx * u_step;
		Vector3D<float> w(cos(u), sin(u), 0);
		for (int vidx = 0; vidx < tubular_resolution; ++vidx) {
			double v = vidx * v_step;
			mesh.vertices[vert_idx(uidx, vidx)] = center + w * torus_radius + w * tube_radius * cos(v)
					+ Vector3D<float>(0, 0, tube_radius * sin(v));

			mesh.colors[vert_idx(uidx, vidx)] = Vector3D<unsigned char>(0, 0, 255);

			int tri_idx = (uidx * tubular_resolution + vidx) * 2;
			mesh.faces[tri_idx + 0] = Vector3D<int>(
					vert_idx((uidx + 1) % radial_resolution, vidx),
					vert_idx((uidx + 1) % radial_resolution,
							 (vidx + 1) % tubular_resolution),
					vert_idx(uidx, vidx));
			mesh.faces[tri_idx + 1] = Vector3D<int>(
					vert_idx(uidx, vidx),
					vert_idx((uidx + 1) % radial_resolution,
							 (vidx + 1) % tubular_resolution),
					vert_idx(uidx, (vidx + 1) % tubular_resolution));
		}
	}
}