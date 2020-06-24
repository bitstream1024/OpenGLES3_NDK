//
// Created by wcg3031 on 2020/6/4.
//

#pragma once

#include <string>
#include <vector>
#include "ShaderHelper.h"
#include "glm/glm.hpp"

#include "DrawHelper.h"

//using namespace std;

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normals;
	glm::vec2 TexCoords;

	/*glm::vec3 Tangent;
	glm::vec3 Bitangent;*/
};

struct Texture
{
	GLuint id;
	std::string type;
	std::string path;
};

class Mesh
{
public:

	Mesh (const std::vector <Vertex> vertices, const std::vector<GLuint> indices, const std::vector<Texture> texture)
	{
		LOGD("Mesh::Mesh");
		m_VAO = m_VBO = m_EBO = GL_NONE;
		m_vVertices = vertices;
		m_vIndices = indices;
		m_vTextures = texture;
		setupMesh();
	}
	~Mesh()
	{
		LOGD("Mesh::~Mesh");
		releaseMesh();
	}
	void DrawMesh (ShaderHelper *pShaderHelper)
	{
		LOGD("Mesh::DrawMesh");
		// bind appropriate textures
		unsigned int diffuseNr  = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr   = 1;
		unsigned int heightNr   = 1;
		for(unsigned int i = 0; i < m_vTextures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
			// retrieve texture number (the N in diffuse_textureN)
			std::string number;
			std::string name = m_vTextures[i].type;
			if(name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if(name == "texture_specular")
				number = std::to_string(specularNr++); // transfer unsigned int to stream
			else if(name == "texture_normal")
				number = std::to_string(normalNr++); // transfer unsigned int to stream
			else if(name == "texture_height")
				number = std::to_string(heightNr++); // transfer unsigned int to stream

			// now set the sampler to the correct texture unit
			pShaderHelper->setInt((name + number).c_str(), i);
			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, m_vTextures[i].id);
		}
		glBindVertexArray(m_VAO);
		DrawHelper::CheckGLError("DrawMesh glBindVertexArray");
		glDrawElements(GL_TRIANGLES, m_vIndices.size(), GL_UNSIGNED_INT, 0);
		DrawHelper::CheckGLError("DrawMesh glDrawElements");
		glBindVertexArray(GL_NONE);
		DrawHelper::CheckGLError("DrawMesh glBindVertexArray");
		glActiveTexture(GL_TEXTURE0);
		DrawHelper::CheckGLError("DrawMesh glActiveTexture");
	}

private:
	
	std::vector <Vertex> m_vVertices;
	std::vector <GLuint> m_vIndices;
	std::vector <Texture> m_vTextures;
	GLuint m_VAO, m_VBO, m_EBO;

	void setupMesh ()
	{
		LOGD("Mesh::initMesh");
		glGenVertexArrays(1, &m_VAO);
		DrawHelper::CheckGLError("setupMesh glGenVertexArrays m_VAO");
		glGenBuffers(1, &m_VBO);
		DrawHelper::CheckGLError("setupMesh glGenBuffers m_VBO");
		glGenBuffers(1, &m_EBO);
		DrawHelper::CheckGLError("setupMesh glGenBuffers m_EBO");

		glBindVertexArray(m_VAO);
		DrawHelper::CheckGLError("setupMesh glBindVertexArray m_VAO");
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		DrawHelper::CheckGLError("setupMesh glBindBuffer m_VBO");
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vVertices.size(), &m_vVertices[0], GL_STATIC_DRAW);
		DrawHelper::CheckGLError("setupMesh glBufferData m_VBO");

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_vIndices.size(), &m_vIndices[0], GL_STATIC_DRAW);
		DrawHelper::CheckGLError("setupMesh glBufferData m_EBO");

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		DrawHelper::CheckGLError("setupMesh glVertexAttribPointer");

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normals));
		DrawHelper::CheckGLError("setupMesh glVertexAttribPointer Normals");

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
		DrawHelper::CheckGLError("setupMesh glVertexAttribPointer TexCoords");

		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
		glBindVertexArray(GL_NONE);
	}

	void releaseMesh ()
	{
		LOGD("Mesh::unInit");
		m_vVertices.clear();
		m_vIndices.clear();
		m_vTextures.clear();
		for (int i = 0; i < m_vTextures.size(); ++i)
			SafeDeleteGLBuffer(1, &m_vTextures[i].id);
		SafeDeleteGLBuffer(1, &m_VAO);
		SafeDeleteGLBuffer(1, &m_VBO);
		SafeDeleteGLBuffer(1, &m_EBO);
	}
};
