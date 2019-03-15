#include "game.h"

#include <stdio.h>
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>

Game::Game()
{
	m_window = nullptr;
}

Game::~Game()
{
	if (m_window)
		glfwDestroyWindow(m_window);

	glfwTerminate();
}

int Game::init(char const* title, int width, int height)
{
	if (!glfwInit()) {
		return 1;
	}

	glfwWindowHint(GLFW_DECORATED, false);
	glfwWindowHint(GLFW_FLOATING, true);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
	m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!m_window) {
		glfwTerminate();
		return 2;
	}

	glfwMakeContextCurrent(m_window);

	if (ogl_LoadFunctions() != ogl_LOAD_SUCCEEDED) {
		glfwDestroyWindow(m_window);
		glfwTerminate();
		return 3;
	}

	int monitorCount;
	GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
	if (monitorCount > 1) {
		GLFWmonitor* monitor = monitors[0];
		const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
		glfwSetWindowPos(m_window, 1920 + videoMode->width / 2 - width / 2, videoMode->height / 2 - width / 2);
	}

	return 0;
}

void Game::loop()
{
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0f, 0.0f, 0.0f, 0.1f);

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
	glfwSwapBuffers(m_window);
}

void Game::update(float delta) {
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(m_window, true);
}
