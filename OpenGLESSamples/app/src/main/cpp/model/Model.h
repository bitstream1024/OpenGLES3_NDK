//
// Created by wcg3031 on 2020/6/4.
//

//#pragma once

#pragma once

#include "Mesh.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

class Model
{
public:

	Model (const std::string modelPath)
	{
		LOGD ("Model::Model modelPath = %s", modelPath.c_str());
		loadModel(modelPath);
	}
	~Model()
	{
		LOGD("~Model");
		m_Meshes.clear();
		textures_loaded.clear();
	}
	void Draw (ShaderHelper *pShaderHelper)
	{
		LOGD ("Model::Draw");
		for (int i = 0; i < m_Meshes.size(); ++i)
		{
			m_Meshes[i].DrawMesh(pShaderHelper);
		}
	}

private:

	std::string m_Directory;
	std::vector <Mesh> m_Meshes;
	std::vector<Texture> textures_loaded;

	int loadModel (const std::string modePath)
	{
		LOGD ("Model::loadModel");
		int ret = ERROR_OK;
		Assimp::Importer importer;
		const aiScene *pScene = importer.ReadFile(modePath, aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
		{
			LOGE("loadModel error %s", importer.GetErrorString());
			return ERROR_ASSIMP;
		}
		m_Directory = modePath.substr(0, modePath.find_last_of('/'));

		processNode(pScene->mRootNode, pScene);

		return ERROR_OK;
	}
	void processNode (const aiNode *pNode, const aiScene *pScene)
	{
		LOGD ("Model::processNode");
		// process all meshes of node
		for (int i = 0; i < pNode->mNumMeshes; ++i)
		{
			aiMesh *pMesh = pScene->mMeshes [pNode->mMeshes[i]];
			m_Meshes.push_back(processMesh(pMesh, pScene));
		}

		// process it's children node
		for (int i = 0; i < pNode->mNumChildren; ++i)
		{
			processNode(pNode->mChildren[i], pScene);
		}
	}

	Mesh processMesh (const aiMesh *pMesh, const aiScene *pScene)
	{
		LOGD ("Model::processMesh");
		std::vector <Vertex> vertices;
		std::vector <unsigned int> indices;
		std::vector <Texture> textures;

		for (int i = 0; i < pMesh->mNumVertices; ++i)
		{
			Vertex vertex;
			glm::vec3 vector;
			vector.x = pMesh->mVertices[i].x;
			vector.y = pMesh->mVertices[i].y;
			vector.z = pMesh->mVertices[i].z;
			vertex.Position = vector;

			vector.x = pMesh->mNormals[i].x;
			vector.y = pMesh->mNormals[i].y;
			vector.z = pMesh->mNormals[i].z;
			vertex.Normals = vector;

			if (pMesh->mTextureCoords[0])
			{
				glm::vec2 vec;
				vec.x = pMesh->mTextureCoords[0][i].x;
				vec.y = pMesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.f, 0.f);

			// tangent
			/*vector.x = pMesh->mTangents[i].x;
			vector.y = pMesh->mTangents[i].y;
			vector.z = pMesh->mTangents[i].z;
			vertex.Tangent = vector;
			// bitangent
			vector.x = pMesh->mBitangents[i].x;
			vector.y = pMesh->mBitangents[i].y;
			vector.z = pMesh->mBitangents[i].z;
			vertex.Bitangent = vector;*/
			vertices.push_back(vertex);
		}

		// process faces
		for (int i = 0; i < pMesh->mNumFaces; ++i)
		{
			aiFace face = pMesh->mFaces[i];
			for (int j = 0; j < face.mNumIndices; ++j)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		// process materials
		aiMaterial *pMaterial = pScene->mMaterials[pMesh->mMaterialIndex];
		// 1. diffuse maps
		std::vector<Texture> diffuseMaps = loadMaterialTextures(pMaterial, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		std::vector<Texture> specularMaps = loadMaterialTextures(pMaterial, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<Texture> normalMaps = loadMaterialTextures(pMaterial, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<Texture> heightMaps = loadMaterialTextures(pMaterial, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		// return a mesh object created from the extracted mesh data
		return Mesh(vertices, indices, textures);
	}

	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
	{
		std::vector<Texture> textures;
		for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), m_Directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
		}
		return textures;
	}

	/*std::vector <Texture> loadMaterialTextures (const aiMaterial *mat, const aiTextureType type, const std::string typeName)
	{
		LOGD ("Model::loadMaterialTextures");
		std::vector <Texture> textures;
		for (int i = 0; i < mat->GetTextureCount(type); ++i)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for(unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
				if(!skip)
				{   // if texture hasn't been loaded already, load it
					Texture texture;
					texture.id = TextureFromFile(str.C_Str(), this->m_Directory);
					texture.type = typeName;
					texture.path = str.C_Str();
					textures.push_back(texture);
					textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
				}
			}
		}
		return textures;
	}*/

	unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false)
	{
		LOGD("Model::TextureFromFile %s, %s", path, directory.c_str());
		std::string filename = std::string(path);
		filename = directory + '/' + filename;
		LOGD("TextureFromFile filename = %s", filename.c_str());

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width = 0, height = 0;
		MyImageInfo myImageInfo {0};
		int ret = OpenImageHelper::LoadPngFromFile(filename.c_str(), &myImageInfo);
		LOGD("TextureFromFile LoadPngFromFile ret = %d", ret);
		OpenImageHelper::PrintMyImageInfo(&myImageInfo, "TextureFromFile myImageInfo");
		if (ERROR_OK == ret)
		{
			GLenum format = GL_RGB;
			if (MY_FORMAT_RGB == myImageInfo.format)
				format = GL_RGB;
			else if (MY_FORMAT_RGBA == myImageInfo.format)
				format = GL_RGBA;

			width = myImageInfo.channel[0];
			height = myImageInfo.height;
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, myImageInfo.buffer[0]);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			OpenImageHelper::FreeMyImageInfo(&myImageInfo);
		}
		else
		{
			LOGE("Texture failed to load at path: %s", path);
			OpenImageHelper::FreeMyImageInfo(&myImageInfo);
		}

		return textureID;
	}
};

