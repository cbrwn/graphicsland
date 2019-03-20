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

	glfwGetCursorPos(m_window, &m_px, &m_py);

	//int monitorCount;
	//GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
	//if (monitorCount > 1)
	//{
	//	GLFWmonitor* monitor = monitors[0];
	//	const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
	//	glfwSetWindowPos(m_window, videoMode->width / 2 - width / 2, videoMode->height / 2 - width / 2);
	//}

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

	glfwSetWindowSizeCallback(m_window, [](GLFWwindow* a, int w, int h) {
		glViewport(0, 0, w, h);
	});

	m_shader->use();
	m_shader->bindUniform(m_shader->getUniform("LightPos"), { 5.0f, 10.0f, 5.0f });

	camMatrix = glm::mat4(1);
	camPos = { 0,4,0 };

	m_shader->bindUniform("CamTransform", camMatrix);

	glm::mat4 quadTransform = glm::mat4(1);
	quadTransform = glm::scale(quadTransform, { 7,7,7 });
	//quadTransform = glm::rotate(quadTransform, m_timer*1.2f, { 1, 0, 0 });

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

	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

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

	//glm::mat4 test(1);
	//test = glm::rotate(test, 0.01f, { 0,1,0 });
	//camMatrix = test * camMatrix;

	int w, h;
	glfwGetWindowSize(m_window, &w, &h);
	m_shader->bindUniform("Resolution", glm::vec2(w, h));
	m_shader->bindUniform(m_shader->getUniform("Time"),
		m_timer);
	m_shader->bindUniform("CamTransform", camMatrix);

	m_cube->draw();

	glfwSwapBuffers(m_window);
}

void Game::update(float delta)
{
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(m_window, true);

	m_timer += delta;

	// camera movement
	glm::vec4 movement = { 0,0,0, 0 };
	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
		movement += camMatrix[2];
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
		movement -= camMatrix[2];
	if (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS)
		movement += camMatrix[1];
	if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS)
		movement -= camMatrix[1];
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
		movement += camMatrix[0];
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
		movement -= camMatrix[0];
	if (glm::length(movement) > 0.01f)
		movement = glm::normalize(movement);
	//camMatrix = glm::translate(camMatrix, glm::vec3(movement) * delta * 10.0f);
	camPos += glm::vec3(movement) * delta * 10.0f;

	double _mx, _my;
	glfwGetCursorPos(m_window, &_mx, &_my);

	if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		const float sens = 0.005f;
		m_rotY += (float)(_mx - m_px)*sens;
		m_rotX += (float)(_my - m_py)*sens;
	}

	m_px = _mx;
	m_py = _my;

	glm::mat4 rotx(1);
	glm::mat4 roty(1);
	rotx = glm::rotate(rotx, m_rotX, { 1,0,0 });
	roty = glm::rotate(roty, m_rotY, { 0,1,0 });
	glm::mat4 rot = roty * rotx;

	glm::mat4 trans(1);
	trans = glm::translate(trans, camPos);
	camMatrix = trans * rot;

}
