#include "stdafx.h"
#include "MainWindow.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////

using namespace CommonLibOgl;
using namespace TestWindow;

//////////////////////////////////////////////////////////////////////////


MainWindow::MainWindow(HINSTANCE hInstance, int nCmdShow, const WindowInfo& wndInfo, const OpenGLInfo& openGLInfo)
	: WindowBase(hInstance, nCmdShow, wndInfo, openGLInfo), 
	  m_vao{}, m_vbo{}, m_index{}, m_indexCount{}, m_unMvp(-1)
{
}

MainWindow::~MainWindow()
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

LRESULT MainWindow::windowProcDerived(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(m_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, aboutProc);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_KEYDOWN:
		{
			int key = (int)wParam;
			//LPARAM keyData = lParam;

			switch (key)
			{
			case VK_ESCAPE:
				DestroyWindow(hWnd);
				break;
			//////////////////////////////////////////////////////////////////////////
			// Translate the camera.
			//////////////////////////////////////////////////////////////////////////
			case 0x57:    // W key
				translateCameraY(0.1f);
				break;
			case 0x53:    // S key
				translateCameraY(-0.1f);
				break;
			case 0x41:    // A key
				translateCameraX(-0.1f);
				break;
			case 0x44:    // D key
				translateCameraX(0.1f);
				break;
			//////////////////////////////////////////////////////////////////////////
			// Rotate the camera.
			//////////////////////////////////////////////////////////////////////////
			case VK_UP:
				rotateCameraX(2.0f);
				break;
			case VK_DOWN:
				rotateCameraX(-2.0f);
				break;
			case VK_LEFT:
				rotateCameraY(-2.0f);
				break;
			case VK_RIGHT:
				rotateCameraY(2.0f);
				break;
			//////////////////////////////////////////////////////////////////////////
			// Zoom in/out for the camera.
			//////////////////////////////////////////////////////////////////////////
			case VK_NUMPAD0:
				scaleCamera(-0.05f);
				break;
			case VK_NUMPAD1:
				scaleCamera(0.05f);
				break;
			}
		}
		break;
	}

	return 0;
}

bool MainWindow::initialize()
{
	if (!m_hRC)
	{
		std::wcerr << L"No OpenGL rendering context\n";
		assert(false); return false;
	}

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
		 0.5f,  0.5f, 0.0f,    // top right
		 0.5f, -0.5f, 0.0f,    // bottom right
		-0.5f, -0.5f, 0.0f,    // bottom left
		-0.5f,  0.5f, 0.0f     // top left 
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

void MainWindow::updateViewMatrices() const
{
	assert(m_spProgram);
	assert(-1 != m_unMvp);

	// TODO: turn on
#if 0
	assert(-1 != m_unNormal);
#endif

	glUseProgram(m_spProgram->getProgram());

	glm::mat4 mvp = m_spCamera->getModelViewProjectionMatrix();

	glUniformMatrix4fv(m_unMvp, 1, GL_FALSE, glm::value_ptr(mvp));

	glm::mat4 modelView = m_spCamera->getModelViewMatrix();

	// WARNING: we are using the fact that there are no non-uniform scaling. If this will change, use the entire 4x4 matrix.
	glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelView)));

	//Normal = mat3(transpose(inverse(model))) * normal;

	// TODO: turn on
#if 0
	glUniformMatrix4fv(m_unNormal, 1, GL_FALSE, glm::value_ptr(normalMatrix));
#endif

	glUseProgram(0);
}

void MainWindow::render()
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

	SwapBuffers(GetDC(m_hWnd));
}
