#include "stdafx.h"
#include "Scene.h"

//////////////////////////////////////////////////////////////////////////

using namespace CommonLibOgl;
using namespace TestWindow;

//////////////////////////////////////////////////////////////////////////


Scene::Scene()
	: m_vao{}, m_vbo{}, m_index{}, m_indexCount{}, m_unMvp(-1)
{
}

Scene::~Scene()
{
	if (0 != m_index)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &m_index);
	}

	if (0 != m_vbo)
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &m_vbo);
	}

	if (0 != m_vao)
	{
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &m_vao);
	}
}

bool Scene::initialize(GLfloat aspectRatio, const OpenGLInfo& openGlInfo)
{
	// Initial scale factor for the camera.
	const GLfloat CameraScaleFactor = 1.0f;

	m_spCamera = std::make_unique<Camera>(aspectRatio, CameraScaleFactor,
		openGlInfo.FieldOfView, openGlInfo.FrustumNear, openGlInfo.FrustumFar);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.8f, 0.93f, 0.96f, 1.0f);    // very light blue
	//glClearColor(0.0f, 0.64f, 0.91f, 1.0f);    // light blue

	// Initialize the program wrapper.

	const ShaderCollection shaders = {
		{ GL_VERTEX_SHADER,   "shaders\\testApp.vert" },
		{ GL_FRAGMENT_SHADER, "shaders\\testApp.frag" }
	};

	m_spProgram = std::make_unique<ProgramGLSL>(shaders);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// A quad of two triangles.
	GLfloat vertices[] = {
		0.5f, 0.5f, 0.0f,    // top right
		0.5f, -0.5f, 0.0f,    // bottom right
		-0.5f, -0.5f, 0.0f,    // bottom left
		-0.5f, 0.5f, 0.0f     // top left 
	};

	// Generate VBO and fill it with the data.

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, _countof(vertices) * sizeof(vertices[0]), vertices, GL_STATIC_DRAW);

	// Fill in the vertex position attribute.
	const GLuint attrVertexPosition = 0;
	glVertexAttribPointer(attrVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(attrVertexPosition);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Fill the index buffer.

	GLuint indices[] = {
		0, 1, 3,    // first triangle
		1, 2, 3     // second triangle
	};

	m_indexCount = _countof(indices);

	glGenBuffers(1, &m_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _countof(indices) * sizeof(indices[0]), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	const GLuint program = m_spProgram->getProgram();

	m_unMvp = glGetUniformLocation(program, "mvp");
	if (-1 == m_unMvp)
	{
		std::wcerr << L"Failed to get uniform location: mvp\n";
		assert(false); return false;
	}

	return true;
}

void Scene::updateViewMatrices() const
{
	assert(m_spProgram);
	assert(-1 != m_unMvp);

	glUseProgram(m_spProgram->getProgram());

	glm::mat4 mvp = m_spCamera->getModelViewProjectionMatrix();

	glUniformMatrix4fv(m_unMvp, 1, GL_FALSE, glm::value_ptr(mvp));

	glm::mat4 modelView = m_spCamera->getModelViewMatrix();

	glUseProgram(0);
}

void Scene::translateCameraX(GLfloat diff)
{
	m_spCamera->translateX(diff);
}

void Scene::translateCameraY(GLfloat diff)
{
	m_spCamera->translateY(diff);
}

void Scene::translateCameraZ(GLfloat diff)
{
	m_spCamera->translateZ(diff);
}

void Scene::rotateCameraX(GLfloat angleDegrees)
{
	m_spCamera->rotateX(angleDegrees);
}

void Scene::rotateCameraY(GLfloat angleDegrees)
{
	m_spCamera->rotateY(angleDegrees);
}

void Scene::rotateCameraZ(GLfloat angleDegrees)
{
	m_spCamera->rotateZ(angleDegrees);
}

void Scene::rotateCameraXY(GLfloat xAngleDegrees, GLfloat yAngleDegrees)
{
	m_spCamera->rotateXY(xAngleDegrees, yAngleDegrees);
}

GLfloat Scene::getCameraScale() const
{
	return m_spCamera->getScale();
}

void Scene::scaleCamera(GLfloat amount)
{
	m_spCamera->scale(amount);
}

void Scene::resize(GLfloat aspectRatio)
{
	m_spCamera->resize(aspectRatio);
}

void Scene::render() const
{
	updateViewMatrices();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	assert(m_spProgram);

	glUseProgram(m_spProgram->getProgram());
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index);

	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}
