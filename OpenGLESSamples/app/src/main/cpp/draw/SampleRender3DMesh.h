//
// Created by wcg3031 on 2020/6/4.
//

#pragma once

#include "SampleBase.h"
#include "Model.h"
#include "common.h"
#include "ShaderHelper.h"

/**
 * SampleRender3DMesh is a easy sample to render 3D model, load model by assimp
 */
class SampleRender3DMesh : public SampleBase
{
public:
	SampleRender3DMesh ();
	~SampleRender3DMesh ();
	RESULT OnDrawFrame ();

private:
	void createRenderGLBuffer ();
	void destroyRenderGLBuffer ();

	void initMVPMatrix ();
	void makeMVPByMotionState (const glm::mat4 model, const glm::mat4 view, const glm::mat4 projection,
			glm::mat4 &dstMVP);
	void makeViewMatrixByMotionState (const glm::mat4 srcView, glm::mat4 &dstView);

	Model *m_pModel;
	ShaderHelper *m_pShaderHelper;

	long long m_Time;
	glm::mat4 m_Model;
	glm::mat4 m_View;
	glm::mat4 m_Projection;

	const char *pVertex_shader_0 =
			GLES_VERSION_STRING
			R"(
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNom;
layout (location = 2) in vec2 aTexCoord;

out vec2 v_texCoord;
uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(aPos, 1.0);
	v_texCoord = aTexCoord;
}
)";

	const char *pFragment_shader_0 =
			GLES_VERSION_STRING
			GLES_MEDIUM_STRING
			R"(
out vec4 FragColor;
in vec2 v_texCoord;
uniform sampler2D texture_diffuse1;
void main()
{
	FragColor = texture(texture_diffuse1, v_texCoord);
}
)";
};



