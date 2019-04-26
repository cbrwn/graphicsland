#include "application.h"

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include "imgui_glfw3.h"

#include "shader.h"

Application::Application()
{
	m_postShader = nullptr;

	// set FBO to 0 so we can know the first time we set up post processing
	m_fbo = 0;
}

Application::~Application()
{
	if (m_postProcessing)
		cleanupPostProcessing();
	delete m_postShader;
	// delete the quad, since it's not deleted in cleanupPostProcessing()
	glDeleteVertexArrays(1, &m_quadVao);
	glDeleteBuffers(1, &m_quadVbo);

	ImGui_Shutdown();

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

/*
	@brief Initializes all window-related things in the class
			This is used instead of the constructor in order to
			return an error code
	@param Title of the window
	@param Width of the window in pixels
	@param Height of the window in pixels
	@param Number of anti aliasing (MSAA) samples
	@param Whether or not postprocessing should be used
	@return An error code: 0=success, 1=glfw init fail,
			2=glfw create window fail, 3=opengl function binding fail
*/
int Application::init(char const* title, int width, int height,
	int msaaSamples, bool postProcessing)
{
	m_windowWidth = width;
	m_windowHeight = height;

	m_samples = msaaSamples;

	if (!glfwInit())
		return 1;

	// create glfw window
	glfwWindowHint(GLFW_SAMPLES, m_samples);
	m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!m_window)
	{
		glfwTerminate();
		return 2;
	}

	glfwMakeContextCurrent(m_window);

	// bind opengl functions
	if (ogl_LoadFunctions() != ogl_LOAD_SUCCEEDED)
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
		return 3;
	}

	glEnable(GL_MULTISAMPLE);
	m_postProcessing = postProcessing;
	if (postProcessing)
		setupPostProcessing();

	ImGui_Init(m_window, true);

	// stick window in the center of the screen
	int monitorCount;
	GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
	if (monitorCount > 0)
	{
		GLFWmonitor* monitor = monitors[0];
		const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
		glfwSetWindowPos(
			m_window,
			videoMode->width / 2 - (width / 2),
			videoMode->height / 2 - (height / 2)
		);
	}

	// set user pointer to this class so we can access it in glfw callbacks
	glfwSetWindowUserPointer(m_window, this);

	// attach resize callback
	// I use the user pointer here instead of simply capturing this class
	// because apparently this doesn't work if you use any captures???
	glfwSetWindowSizeCallback(m_window,
		[](GLFWwindow* win, int w, int h)
	{
		Application* app = (Application*)glfwGetWindowUserPointer(win);
		app->windowResized(w, h);
	});

	return setup();
}

/*
	@brief Runs the game loop while the window is open
*/
void Application::loop()
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	double lastTime = glfwGetTime();

	while (!glfwWindowShouldClose(m_window))
	{
		double currentTime = glfwGetTime();
		float delta = (float)(currentTime - lastTime);
		lastTime = currentTime;

		ImGui_NewFrame();

		update(delta);

		// bind to off-screen framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.3f, 0.6f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		draw();

		// blit multisampled buffer to regular one
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
		// bind to intermediate framebuffer or default framebuffer
		if (m_postProcessing)
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ifbo);
		else
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0,
			m_windowWidth, m_windowHeight,
			0, 0,
			m_windowWidth, m_windowHeight,
			GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		if (m_postProcessing)
			drawPost();
		ImGui::Render();

		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}
}

/*
	@brief Called when the window resizes in order to cache the new size
			Called by the GLFW window size callback, so must be public
	@param New width of the window
	@param New height of the window
*/
void Application::windowResized(int width, int height)
{
	// update gl viewport to fill the whole window
	glViewport(0, 0, width, height);
	// cache window size since we use it often
	m_windowWidth = width;
	m_windowHeight = height;

	// re-make postprocessing stuff using the new window size
	if (m_postProcessing)
		setupPostProcessing();
}

/*
	@brief Renders a quad showing the current frame using a shader
*/
void Application::drawPost()
{
	// bind default framebuffer to draw to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_postShader->use();
	glBindVertexArray(m_quadVao);

	// bind framebuffer texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_itex);

	// bind depth texture to slot 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_depthTex);
	// draw quad
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

/*
	@brief Initializes everything used in post-processing, including the
			framebuffer and render buffer as well as the quad to draw to.
			This is also called when the window is resized in order to make
			a new framebuffer with the correct resolution
*/
void Application::setupPostProcessing()
{
	bool resized = (m_fbo > 0);

	if (resized)
		cleanupPostProcessing();

	// shorter names for window size
	int w = m_windowWidth;
	int h = m_windowHeight;

	// create buffer
	glGenFramebuffers(1, &m_fbo);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// create texture for frame buffer
	glGenTextures(1, &m_tex);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_tex);

	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_samples, GL_RGB, w, h, 1);

	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_tex, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_depthTex, 0);

	// make render buffer for depth/stencil since we won't use them in post
	// processing
	glGenRenderbuffers(1, &m_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_samples, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	// ===========================
	// intermediate framebuffer creation
	// ===========================
	// intermediate framebuffer
	glGenFramebuffers(1, &m_ifbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ifbo);

	// intermediate texture
	glGenTextures(1, &m_itex);
	glBindTexture(GL_TEXTURE_2D, m_itex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// and depth buffer
	glGenTextures(1, &m_depthTex);
	glBindTexture(GL_TEXTURE_2D, m_depthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_itex, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (resized)
		return;

	// make quad for rendering framebuffer to
	float vertices[] = {
		// pos        // uv
		-1.0f,  1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,

		-1.0f,  1.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f
	};

	glGenVertexArrays(1, &m_quadVao);
	glGenBuffers(1, &m_quadVbo);
	glBindVertexArray(m_quadVao);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	m_postShader = new ShaderProgram();
	// write shaders inline so we don't have to find files
	m_postShader->createShader(
		ShaderStage::VERTEX,
		"#version 440 \n\n\
layout(location=0) in vec2 Pos;\
layout(location=1) in vec2 TexCoord;\
out vec2 uv;\
void main(){\
	gl_Position = vec4(Pos.x, Pos.y, 0.0, 1.0);\
	uv = TexCoord;\
}"
);
	m_postShader->createShader(
		ShaderStage::FRAGMENT,
		"#version 440\n\n\
in vec2 uv;\
uniform sampler2D screenTexture;\
out vec4 fragColor;\
void main(){\
	fragColor=texture(screenTexture,uv);\
}"
);
	m_postShader->link();

	m_postShader->use();
	// screen texture will be bound to slot 0
	m_postShader->bindUniform("screenTexture", 0U);
	// and depth texture to slot 1
	m_postShader->bindTexUniform("depthTexture", 1U);
}

/*
	@brief A small helper function to clean up the things that need to be
			re-made when resized
*/
void Application::cleanupPostProcessing()
{
	// delete multisampled framebuffer
	glDeleteFramebuffers(1, &m_fbo);
	// delete intermediate framebuffer
	glDeleteFramebuffers(1, &m_ifbo);
	// delete multisampled screen texture
	glDeleteTextures(1, &m_tex);
	// delete intermediate screen texture
	glDeleteTextures(1, &m_itex);
	// delete depth texture
	glDeleteTextures(1, &m_depthTex);
	// delete render buffer
	glDeleteRenderbuffers(1, &m_rbo);
}
