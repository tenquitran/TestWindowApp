#pragma once


namespace TestWindow
{
	class MainWindow
		: public CommonLibOgl::WindowBase
	{
	public:
		// Parameters: hInstance - application instance;
		//             nCmdShow - controls how the window is to be shown;
		//             wndInfo - information about the window;
		//             openGLInfo - OpenGL settings.
		// Throws: Exception, std::bad_alloc
		MainWindow(HINSTANCE hInstance, int nCmdShow, 
			const CommonLibOgl::WindowInfo& wndInfo, const CommonLibOgl::OpenGLInfo& openGLInfo);

		virtual ~MainWindow();

	private:
		// Initialize the derived window.
		// Usually used to set up OpenGL scene: load objects, etc.
		virtual bool initialize() override;

		// Render the derived window contents.
		virtual void render() override;

		// The derived window procedure.
		// The base class window procedure processes WM_CREATE, WM_DESTROY, WM_SIZE and redirects WM_KEYDOWN here.
		virtual LRESULT windowProcDerived(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

		// Update Model-View-Projection (MVP) and other matrices in the GLSL program.
		void updateViewMatrices() const;

	private:
		std::unique_ptr<CommonLibOgl::ProgramGLSL> m_spProgram;

		GLuint m_vao;
		GLuint m_vbo;

		GLuint m_index;          // index buffer
		GLsizei m_indexCount;    // number of indices

		GLint m_unMvp;           // MVP matrix uniform
	};
}
