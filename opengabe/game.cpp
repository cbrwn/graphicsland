#include "game.h"

#include <stdio.h>
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <Windows.h>
#endif

#include "cube.h"
#include "shader.h"

Game::Game()
{
	m_window = nullptr;
	m_cube = nullptr;

	m_timer = 0.0f;
}

Game::~Game()
{
	delete m_cube;

	if (m_window)
		glfwDestroyWindow(m_window);

	glfwTerminate();
}

int Game::init(char const* title, int width, int height)
{
	if (!glfwInit())
		return 1;

	//glfwWindowHint(GLFW_DECORATED, false);
	//glfwWindowHint(GLFW_FLOATING, true);
	//glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
	m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!m_window)
	{
		glfwTerminate();
		return 2;
	}

	glfwMakeContextCurrent(m_window);

	if (ogl_LoadFunctions() != ogl_LOAD_SUCCEEDED)
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
		return 3;
	}

	int monitorCount;
	GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
	if (monitorCount > 1)
	{
		GLFWmonitor* monitor = monitors[0];
		const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
		glfwSetWindowPos(m_window, videoMode->width / 2 - width / 2, videoMode->height / 2 - width / 2);
	}

	//#ifdef _WIN32
	//	// hide window from taskbar
	//	HWND winHandle = glfwGetWin32Window(m_window);
	//
	//	long style = GetWindowLong(winHandle, GWL_STYLE);
	//	style |= WS_EX_TOOLWINDOW;
	//
	//	ShowWindow(winHandle, SW_HIDE);
	//	SetWindowLong(winHandle, GWL_STYLE, style);
	//	ShowWindow(winHandle, SW_SHOW);
	//#endif

	// create framebuffer

	m_viewMatrix = glm::lookAt(
		glm::vec3(7, 0, 0), glm::vec3(0), glm::vec3(0, 1, 0)
	);

	int w, h;
	glfwGetWindowSize(m_window, &w, &h);
	m_projectionMatrix =
		glm::perspective(
			glm::pi<float>() * 0.25f,
			w / (float)h,
			0.1f,
			1000.f
		);

	m_cube = new Cube();

	m_shader = new ShaderProgram();
	m_shader->loadShader(ShaderStage::VERTEX, "shaders/raymarch.vert");
	m_shader->loadShader(ShaderStage::FRAGMENT, "shaders/raymarch.frag");
	m_shader->link();

	m_shader->use();
	m_shader->bindUniform(m_shader->getUniform("LightPos"), { 10.0f, 10.0f, 10.0f });
	m_shader->bindUniform(m_shader->getUniform("CamPos"), { 10.0f, 10.0f, 10.0f });

	glm::mat4 quadTransform = glm::mat4(1);
	quadTransform = glm::scale(quadTransform, { 7,7,7 });
	quadTransform = glm::rotate(quadTransform, m_timer*1.2f, { 1, 0, 0 });

	m_shader->bindUniform("M", quadTransform)
		->bindUniform("V", m_viewMatrix)
		->bindUniform("P", m_projectionMatrix)
		->bindUniform("Resolution", glm::vec2(width, height));

	return 0;
}

void Game::loop()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	while (!glfwWindowShouldClose(m_window))
	{
		draw();
		update(0.016f);
		glfwPollEvents();
	}
}

void Game::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glm::vec3 camPos = glm::vec3(sinf(m_timer)*15.0f, 15.0f, cosf(m_timer)*15.0f);
	////glm::vec3 camPos = glm::vec3(15.0f);
	//m_viewMatrix = glm::lookAt(
	//	camPos, glm::vec3(0), glm::vec3(0, 1, 0)
	//);

	//glm::mat4 quadTransform = glm::mat4(1);
	//quadTransform = glm::scale(quadTransform, { 7,7,7 });
	//quadTransform = glm::rotate(quadTransform, m_timer*1.2f, { 1, 0, 0 });

	//m_shader->bindUniform("M", quadTransform)
	//	->bindUniform("V", m_viewMatrix)
	//	->bindUniform("P", m_projectionMatrix)
	//	->bindUniform("timer", m_timer);

	m_shader->bindUniform(m_shader->getUniform("LightPos"),
		{ sinf(m_timer*2.0f)*10.0f, 10.0f, cosf(m_timer*2.0f)*10.0f
		});
	m_shader->bindUniform(m_shader->getUniform("Time"),
		m_timer);

	m_cube->draw();

	glfwSwapBuffers(m_window);
}

void Game::update(float delta)
{
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(m_window, true);

	m_timer += delta;
}
