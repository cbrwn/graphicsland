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
#include "shader.h"
#include "camera.h"
#include "OBJMesh.h"
#include "drawable.h"

Game::Game()
{
	m_scene = nullptr;
	m_window = nullptr;

	m_escapeDown = false;
	m_fbo = 0;

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

	glDeleteFramebuffers(1, &m_fbo);
	glDeleteVertexArrays(1, &m_quadVao);
	glDeleteBuffers(1, &m_quadVbo);
	glDeleteTextures(1, &m_tex);
	glDeleteRenderbuffers(1, &m_rbo);

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

	// create framebuffer used for post effects
	setupFramebuffer();

	int monitorCount;
	GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
	if (monitorCount > 1)
	{
		GLFWmonitor* monitor = monitors[0];
		const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
		glfwSetWindowPos(m_window, videoMode->width / 2 - width / 2, videoMode->height / 2 - width / 2);
	}

	// create shader
	m_shader = new PhongShader(true);
	m_shader->setLightCount(2)
		->setLight(0, {
			{20,20,20}, // pos
			glm::vec4{1, 1, 1, 1}/2.0f, // diffuse
			{1, 1, 1, 1} // specular
			})
		->setLight(1, {
			{20,20,20}, // pos
			{1, 1, 1, 1}, // diffuse
			{1, 1, 1, 1} // specular
			});
	m_shader->use();

	// load models
	m_bunny = new OBJMesh();
	m_dragon = new OBJMesh();
	m_buddha = new OBJMesh();
	m_bunny->load("models/spear/soulspear.obj", true, true);
	//m_dragon->load("models/Dragon.obj");
	//m_buddha->load("models/Buddha.obj");

	m_scene = new Scene();

	m_drawables.push_back(new Drawable({ 0,0,0 }));
	m_drawables.back()->setMesh(m_bunny)->setShader(m_shader);
	//m_drawables.push_back(new Drawable({ 10,0,0 }, { 0,0,0 }, m_drawables[0]));
	//m_drawables.back()->setMesh(m_dragon)->setShader(m_shader);
	//m_drawables.push_back(new Drawable({ -10,0,0 }, { 0,0,0 }, m_drawables[1]));
	//m_drawables.back()->setMesh(m_buddha)->setShader(m_shader);

	m_drawables[0]->scale(glm::vec3(5.0f));

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
		((Game*)glfwGetWindowUserPointer(win))->setupFramebuffer();
	});

	return 0;
}

void Game::loop()
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

		update(delta);

		draw();
		drawPost();

		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}
}

void Game::draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glEnable(GL_DEPTH_TEST);

	//int _w, _h;
	//glfwGetWindowSize(m_window, &_w, &_h);
	//glViewport(0, 0, _w/8, _h/8);

	glClearColor(0.3f, 0.6f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (Drawable* d : m_drawables)
		d->draw();
}

void Game::drawPost()
{
	// second pass
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default

	//int _w, _h;
	//glfwGetWindowSize(m_window, &_w, &_h);
	//glViewport(0, 0, _w, _h);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_postShader->use();
	glBindVertexArray(m_quadVao);
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_tex);
	glDrawArrays(GL_TRIANGLES, 0, 6);
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

	m_shader->setLightPos(1, {
		sinf(m_timer*2.0f) * 10.0f,
		4.0f,
		cosf(m_timer*2.0f)*10.0f
		});
}

void Game::setupFramebuffer()
{
	bool resized = (m_fbo > 0);

	if (resized)
	{
		glDeleteFramebuffers(1, &m_fbo);
		glDeleteTextures(1, &m_tex);
		glDeleteRenderbuffers(1, &m_rbo);
	}

	// create buffer
	glGenFramebuffers(1, &m_fbo);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// create texture for frame buffer
	glGenTextures(1, &m_tex);
	glBindTexture(GL_TEXTURE_2D, m_tex);

	int w, h;
	glfwGetWindowSize(m_window, &w, &h);
	//w /= 8;
	//h /= 8;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex, 0);

	// make render buffer for depth/stencil since we won't use them in post
	// processing
	glGenRenderbuffers(1, &m_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
	m_postShader->loadShader(ShaderStage::VERTEX, "shaders/post/post.vert");
	m_postShader->loadShader(ShaderStage::FRAGMENT, "shaders/post/post.frag");
	m_postShader->link();

	m_postShader->use();
	printf("screenTexture: %u\n", m_tex);
	m_postShader->bindUniform("screenTexture", 0U);
}
