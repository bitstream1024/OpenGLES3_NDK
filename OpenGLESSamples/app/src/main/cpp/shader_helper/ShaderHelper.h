/***
** this is a class for shader 
** created by chauncy_wang at 5/7/2019
***/

#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include <GLES3/gl3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define MY_MAX_PATH		256
#define GLES_VERSION_STRING		"#version 300 es\n"
#define GLES_MEDIUM_STRING		"precision mediump float;\n"
#define GLES_HIGHP_STRING		"precision highp float;\n"

//using namespace std;

class ShaderHelper {

public:
	// generator
	ShaderHelper(const char* vertexShaderCode, const char* fragmentShaderCode);
	~ShaderHelper();
	int getShaderHelperState ();
	void use();
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setVec3f(const std::string &name, float value1, float value2, float value3) const;
	void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
	unsigned int m_nProgramId;
	int m_nShaderState;
};
