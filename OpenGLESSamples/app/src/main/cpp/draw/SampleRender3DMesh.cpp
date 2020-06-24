//
// Created by wcg3031 on 2020/6/4.
//

#include "LogAndroid.h"
#include "SampleRender3DMesh.h"

#define ModelPath "/sdcard/OpenGLESTest/nanosuit/nanosuit.obj"

SampleRender3DMesh::SampleRender3DMesh ()
	:m_pModel(nullptr), m_pShaderHelper(nullptr),m_Time(0)
{
	LOGD("SampleRender3DMesh::SampleRender3DMesh");
	if (!m_pModel)
		m_pModel = new Model (ModelPath);

	if (!m_pShaderHelper)
		m_pShaderHelper = new ShaderHelper (pVertex_shader_0, pFragment_shader_0);
	if (m_pShaderHelper)
	{
		int state = m_pShaderHelper->getShaderHelperState();
		LOGD("SampleRender3DMesh state = %d", state);
	}
	initMVPMatrix ();
};

SampleRender3DMesh::~SampleRender3DMesh ()
{
	LOGD("SampleRender3DMesh::~SampleRender3DMesh");
	SafeDelete(m_pModel);
	SafeDelete(m_pShaderHelper);
};

RESULT SampleRender3DMesh::OnDrawFrame ()
{
	LOGD("SampleRender3DMesh::OnDrawFrame");
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_pShaderHelper->use();

	float angle = 0.f;
	float trans = 0.f;
	long long currentTime = MyTimeUtils::getCurrentTime();
	if (!m_Time)
	{
		m_Time = currentTime;
	}
	else
	{
		angle = 1.f * (currentTime - m_Time)/1000 * 15; // 15 degrees each 1s
		LOGD("SampleRender3DMesh::OnDrawFrame view angle = %f", angle);

		//trans = 0.1f * (currentTime - m_Time)/1000;
	}
	//m_Model = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, -0.8f, 0.0f)); // translate it down so it's at the center of the scene
	//model = glm::translate(model, glm::vec3(trans, -0.8f, 0.0f)); // translate it down so it's at the center of the scene
	//model = glm::rotate(model, glm::radians(angle), glm::vec3 (0.f, 1.f, 0.f));
	//m_Model = glm::scale(m_Model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down

	//glm::vec3 Position = glm::vec3 (0.f, 0.f, 3.f);
	/*glm::vec3 Position = glm::vec3 (3.f * sinf(glm::radians(angle)), 0.f, 3.f* cosf(glm::radians(angle)));
	glm::vec3 Target = glm::vec3 (0.f, 0.f, 0.f);
	glm::vec3 Up = glm::vec3 (0.f, 1.f, 0.f);
	m_View= glm::lookAt(Position, Target, Up);*/

	makeViewMatrixByMotionState (m_View, m_View);
	//view = glm::translate(view, glm::vec3 (angle/150, angle/150, angle/150));
	//view = glm::translate(view, glm::vec3 (angle/150, 0, 0));
	//view = glm::rotate(model, glm::radians(angle), glm::vec3 (0.f, 1.f, 0.f));

	LOGOUT_MAT4_MATH(m_View, "SampleRender3DMesh::OnDrawFrame view");

	GLint viewPort[4]{0};
	glGetIntegerv(GL_VIEWPORT, viewPort);
	m_Projection = glm::perspective(glm::radians(45.f), (float)viewPort[2] / (float)viewPort[3], 0.1f, 100.0f);

	glm::mat4 MVP = m_Projection * m_View * m_Model;
	m_pShaderHelper->setMat4("MVP", MVP);

	if (m_pModel)
		m_pModel->Draw(m_pShaderHelper);

	glDisable(GL_DEPTH_TEST);
	return ERROR_OK;
}

void SampleRender3DMesh::createRenderGLBuffer ()
{
	LOGD("SampleRender3DMesh::createRenderGLBuffer");
	CreateGLBuffer();
}

void SampleRender3DMesh::destroyRenderGLBuffer ()
{
	LOGD("SampleRender3DMesh::destroyRenderGLBuffer");
	DestroyGLBuffer();
}

void SampleRender3DMesh::initMVPMatrix ()
{
	LOGD("SampleRender3DMesh::initMVPMatrix");
	m_Model = glm::mat4 (1.f);
	m_Model = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, -0.8f, 0.0f));
	m_Model = glm::scale(m_Model, glm::vec3(0.1f, 0.1f, 0.1f));

	m_View = glm::mat4 (1.f);
	glm::vec3 Position = glm::vec3 (0.f, 0.f, 3.f);
	glm::vec3 Target = glm::vec3 (0.f, 0.f, 0.f);
	glm::vec3 Up = glm::vec3 (0.f, 1.f, 0.f);
	m_View= glm::lookAt(Position, Target, Up);
	m_Projection = glm::mat4 (1.f);
}

void SampleRender3DMesh::makeMVPByMotionState (const glm::mat4 model, const glm::mat4 view,
		const glm::mat4 projection, glm::mat4 &dstMVP)
{
	LOGD("SampleRender3DMesh::makeMVPByMotionState");
	MotionState motionState = GetMotionState ();
	motionState.logMotionState("makeMVPByMotionState motionState");

	glm::vec3 transform = glm::vec3 (motionState.transform_x, motionState.transform_y, motionState.transform_z);
	switch (motionState.eMotionType)
	{
		case eMOTION_ROTATE:
			break;
		case eMOTION_SCALE:
			break;
		case eMOTION_TRANSLATE:
			glm::translate(view, transform);
			break;
		default:
			LOGE("makeMVPByMotionState eMotionType unsupported");
			break;
	}
	dstMVP = projection * view * model;
}

void SampleRender3DMesh::makeViewMatrixByMotionState (const glm::mat4 srcView, glm::mat4 &dstView)
{
	LOGD("SampleRender3DMesh::makeViewMatrixByMotionState");
	MotionState motionState = GetMotionState ();
	motionState.logMotionState("makeViewMatrixByMotionState motionState");

	glm::vec3 transform = glm::vec3 (motionState.transform_x, motionState.transform_y, motionState.transform_z);
	LOGD("makeViewMatrixByMotionState motionState transform (%f, %f, %f)", motionState.transform_x,
			motionState.transform_y, motionState.transform_z);
	switch (motionState.eMotionType)
	{
		case eMOTION_ROTATE:
			break;
		case eMOTION_SCALE:
			break;
		case eMOTION_TRANSLATE:
			dstView = glm::translate(srcView, transform);
			break;
		default:
			LOGE("makeMVPByMotionState eMotionType unsupported");
			break;
	}
}