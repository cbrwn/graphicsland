#include "game.h"

#include <stdio.h>
#include <functional>
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <Windows.h>
#endif

#include "phong.h"
#include "scene.h"
#include "camera.h"
#include "OBJMesh.h"
#include "drawable.h"

Game::Game()
{
	m_scene = nullptr;
	m_window = nullptr;

	m_escapeDown = false;

	m_timer = 0.0f;
}

Game::~Game()
{
	// will delete all objects in the scene
	delete m_scene;

	delete m_bunny;
	delete m_dragon;
	delete m_buddha;

	delete m_shader;

	if (m_window)
		glfwDestroyWindow(m_window);

	glfwTerminate();
}

int Game::init(char const* title, int width, int height)
{
	if (!glfwInit())
		return 1;

	//glfwWindowHint(GLFW_DECORATED, false);
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

	// create shader
	m_shader = new PhongShader();
	m_shader->setLightCount(1)
		->setLight(0, {
			{20,20,20}, // pos
			{1, 1, 1, 1}, // diffuse
			{1, 1, 1, 1} // specular
			});

	// load models
	m_bunny = new OBJMesh();
	m_dragon = new OBJMesh();
	m_buddha = new OBJMesh();
	m_bunny->load("models/Trumpet.obj", true, true);
	m_dragon->load("models/Dragon.obj");
	m_buddha->load("models/Buddha.obj");

	m_scene = new Scene();

	m_drawables.push_back(new Drawable({ -2,0,0 }));
	m_drawables.back()->setMesh(m_bunny)->setShader(m_shader);
	//m_drawables.push_back(new Drawable({ 10,0,0 }, { 0,0,0 }, m_drawables[0]));
	//m_drawables.back()->setMesh(m_dragon)->setShader(m_shader);
	//m_drawables.push_back(new Drawable({ -10,0,0 }, { 0,0,0 }, m_drawables[1]));
	//m_drawables.back()->setMesh(m_buddha)->setShader(m_shader);

	//m_drawables[0]->rotate({ -3.14159f/2000000.0f,0,0 });
	m_drawables[0]->scale(glm::vec3(1.0f));

	m_scene->addObject(m_drawables[0]);

	int w, h;
	glfwGetWindowSize(m_window, &w, &h);
	m_scene->getCamera()->updateProjectionMatrix(w, h);

	glfwSetWindowUserPointer(m_window, this);
	glfwSetWindowSizeCallback(m_window,
		[](GLFWwindow* win, int w, int h)
	{
		glViewport(0, 0, w, h);
		((Game*)glfwGetWindowUserPointer(win))->getScene()
			->getCamera()->updateProjectionMatrix(w, h);
	});

	return 0;
}

void Game::loop()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.3f, 0.6f, 0.8f, 1.0f);

	double lastTime = glfwGetTime();

	while (!glfwWindowShouldClose(m_window))
	{
		double currentTime = glfwGetTime();
		float delta = (float)(currentTime - lastTime);
		lastTime = currentTime;

		update(delta);
		draw();
		glfwPollEvents();
	}
}

void Game::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (Drawable* d : m_drawables)
		d->draw();

	glfwSwapBuffers(m_window);
}

void Game::update(float delta)
{
	Camera* cam = m_scene->getCamera();
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		if (!m_escapeDown)
		{
			if (!cam->getLockCursor())
			{
				glfwSetWindowShouldClose(m_window, true);
			}
			else
			{
				glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				cam->setLockCursor(false);
			}
		}
		m_escapeDown = true;
	}
	else
	{
		m_escapeDown = false;
	}

	if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_1)
		&& !cam->getLockCursor())
	{
		cam->setLockCursor(true);
	}

	m_timer += delta;

	//m_drawables[0]->rotate({ 0,1 * delta,0 });
	//m_drawables[1]->rotate({ 0,0,1*delta });
	//m_drawables[2]->rotate({ 1 * delta,0,0 });

	cam->update(delta);

	m_shader->setLightPos(0, cam->getTransform()[3]);
}
