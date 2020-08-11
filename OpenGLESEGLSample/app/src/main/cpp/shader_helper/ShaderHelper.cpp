#include "common.h"
#include "glm/ext.hpp"
#include "LogAndroid.h"
#include "ShaderHelper.h"

ShaderHelper::ShaderHelper(const char* vertexShaderCode, const char* fragmentShaderCode):
		m_nShaderState(ERROR_OK), m_nProgramId(0)
{
	LOGD("Shader_Helper");
	unsigned int vertexShader, fragmentShader;
	int result = 0;
	char infoLog[MY_MAX_PATH * 2] = { 0 };
	do {
		// vertex shader.cpp
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
		glCompileShader(vertexShader);
		// get compile shader.cpp result
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
		if (!result) {
			glGetShaderInfoLog(vertexShader, MY_MAX_PATH * 2, NULL, infoLog);
			LOGE ("glCompileShader vertexShader error, %s", infoLog);
			m_nShaderState = ERROR_GL_STATE;
			break;
		}

		// fragment shader.cpp
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
		glCompileShader(fragmentShader);
		// get compile shader.cpp result
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
		if (!result) {
			glGetShaderInfoLog(fragmentShader, MY_MAX_PATH * 2, NULL, infoLog);
			LOGE ("glCompileShader fragmentShader error, %s ", infoLog);
			m_nShaderState = ERROR_GL_STATE;
			break;
		}

		// shader.cpp program
		m_nProgramId = glCreateProgram();
		glAttachShader(m_nProgramId, vertexShader);
		glAttachShader(m_nProgramId, fragmentShader);
		glLinkProgram(m_nProgramId);
		// get link result
		glGetProgramiv(m_nProgramId, GL_LINK_STATUS, &result);
		if (!result) {
			glGetProgramInfoLog(m_nProgramId, MY_MAX_PATH * 2, NULL, infoLog);
			LOGE ("glLinkProgram progreamId error, %s ", infoLog);
			m_nShaderState = ERROR_GL_STATE;
			break;
		}

		// delete shader.cpp
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	} while (false);
}

ShaderHelper::~ShaderHelper() {}

int ShaderHelper::getShaderHelperState ()
{
	return m_nShaderState;
}

void ShaderHelper::use() {
	glUseProgram(m_nProgramId);
}

void ShaderHelper::setBool (const std::string &name, bool value) const{
	glUniform1i(glGetUniformLocation(m_nProgramId, name.c_str()), value);
}

void ShaderHelper::setInt(const std::string &name, int value) const {
	glUniform1i(glGetUniformLocation(m_nProgramId, name.c_str()), value);
}

void ShaderHelper::setFloat(const std::string &name, float value) const {
	glUniform1f(glGetUniformLocation(m_nProgramId, name.c_str()), value);
}

void ShaderHelper::setVec3f(const std::string &name, float value1, float value2, float value3) const
{
	glUniform3f(glGetUniformLocation(m_nProgramId, name.c_str()), value1, value2, value3);
}

void ShaderHelper::setMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_nProgramId, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
