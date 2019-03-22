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
#include "camera.h"
#include "shader.h"
#include "Texture.h"
#include "OBJMesh.h"

Game::Game()
{
	m_window = nullptr;
	m_cube = nullptr;

	m_escapeDown = false;

	m_timer = 0.0f;
}

Game::~Game()
{
	delete m_cube;
	delete m_tex;
	delete m_mesh;
	delete m_cam;

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

	m_cam = new Camera();

	int w, h;
	glfwGetWindowSize(m_window, &w, &h);
	m_projectionMatrix =
		glm::perspective(
			glm::pi<float>() * 0.25f,
			w / (float)h,
			0.1f,
			1000.f
		);

	glfwSetWindowSizeCallback(m_window, [](GLFWwindow* win, int w, int h) {
		glViewport(0, 0, w, h);
	});

	m_cube = new Cube();

	m_tex = new Texture("pooki.png");
	m_tex->bind(0);


	m_shader = new ShaderProgram();
	m_shader->loadShader(ShaderStage::VERTEX, "shaders/textured.vert");
	m_shader->loadShader(ShaderStage::FRAGMENT, "shaders/textured.frag");
	m_shader->link();

	m_shader->use();
	m_shader->bindUniform(m_shader->getUniform("LightPos"), { 15.0f, 15.0f, 15.0f })
		->bindUniform("friggenTexture", 0u);

	m_mesh = nullptr;
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	draw();

	m_mesh = new OBJMesh();
	m_mesh->load("models/model.obj", true, false);

	return 0;
}

void Game::loop()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glClearColor(0.3f, 0.6f, 0.8f, 1.0f);

	double lastTime = glfwGetTime();

	while (!glfwWindowShouldClose(m_window))
	{
		draw();

		double currentTime = glfwGetTime();
		float delta = (float)(currentTime - lastTime);
		lastTime = currentTime;

		update(delta);
		glfwPollEvents();
	}
}

void Game::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glm::vec3 camPos = glm::vec3(15, 10, 15);// sinf(m_timer)*15.0f, 15.0f, cosf(m_timer)*15.0f);
	//glm::vec3 camPos = glm::vec3(15.0f);

	glm::mat4 quadTransform = glm::mat4(1);
	quadTransform = glm::translate(quadTransform, { 0, 0.0f,0 });
	quadTransform = glm::scale(quadTransform, { 20,20,20 });
	//quadTransform = glm::rotate(quadTransform, m_timer*1.2f, { 0, 1, 0 });

	glm::vec3 lightpos = glm::vec3(m_cam->getTransform()[3]);
	m_shader->bindUniform(m_shader->getUniform("LightPos"), lightpos);

	glm::mat4 v = m_cam->getView();

	m_shader->bindUniform("M", quadTransform)
		->bindUniform("V", v)
		->bindUniform("MVP", m_projectionMatrix * v * quadTransform)
		->bindUniform("timer", m_timer);

	if (m_mesh)
		m_mesh->draw();

	quadTransform = glm::mat4(1);
	quadTransform = glm::translate(quadTransform, lightpos);
	m_shader->bindUniform("M", quadTransform)
		->bindUniform("MVP", m_projectionMatrix * v* quadTransform);
	m_cube->draw();

	glfwSwapBuffers(m_window);
}

void Game::update(float delta)
{
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		if (!m_escapeDown) {
			if (!m_cam->getLockCursor())
			{
				glfwSetWindowShouldClose(m_window, true);
			}
			else
			{
				glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				m_cam->setLockCursor(false);
			}
		}
		m_escapeDown = true;
	}
	else {
		m_escapeDown = false;
	}

	if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_1) && !m_cam->getLockCursor()) {
		m_cam->setLockCursor(true);
	}

	m_timer += delta;

	m_cam->update(delta);
}
