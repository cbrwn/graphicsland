#include "..\game\game.h"
#include "game.h"

#include <stdio.h>
#include <functional>
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>

// imgui
#include <imgui.h>
#include "imgui_glfw3.h"
//#include "imgui/imgui_impl_opengl3.h"

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <Windows.h>
#endif

#include "toon.h"
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

	aie::ImGui_Shutdown();
	//ImGui::DestroyContext(ImGui::GetCurrentContext());
	//ImGui_ImplOpenGL3_Shutdown();

	if (m_window)
		glfwDestroyWindow(m_window);

	glfwTerminate();
}

int Game::init(char const* title, int width, int height)
{
	if (!glfwInit())
		return 1;

	m_focalDist = 1.0f;

	//glfwWindowHint(GLFW_DECORATED, false);
	glfwWindowHint(GLFW_SAMPLES, 4);
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

	glEnable(GL_MULTISAMPLE);
	// create framebuffer used for post effects
	setupFramebuffer();

	//ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO();
	//io.MouseDrawCursor = false;
	//io.Fonts->Build();
	//ImGui_ImplOpenGL3_Init();
	aie::ImGui_Init(m_window, true);

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
	m_shader->setLightCount(2)
		->setLight(0, {
			{20,20,20}, // pos
			glm::vec4{1, 1, 1, 1} *0.5f, // diffuse
			glm::vec4{1, 1, 1, 1}*0.5f // specular
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
	//m_bunny->load("models/Dragon.obj", !!!!!!!!!!false, true);
	//m_dragon->load("models/Dragon.obj");
	//m_buddha->load("models/Buddha.obj");

	// Pos: (-49.69, 57.52, 50.18), Rot: (-0.48, -0.72)
	m_scene = new Scene();
	m_scene->getCamera()->setPosition(
		{ -49, 57, 50 }
	)->setRotation(
		{ -0.48f, -0.72f }
	);

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

	// camera stuff
	m_fstop = 2.0f;
	m_focalDist = 1.5f;
	m_focalLength = 12.5f;
	m_autoFocus = true;
	m_coc = 0.03f;

	m_samples = 9;
	m_rings = 5;

	m_bias = 0.5;
	m_fringe = 7.7;

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

		//ImGui_ImplOpenGL3_NewFrame();
		aie::ImGui_NewFrame();
		//ImGui::NewFrame();

		update(delta);

		draw();

		int _w, _h;
		glfwGetWindowSize(m_window, &_w, &_h);

		// blit multisampled buffer to regular one
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ifbo);
		glBlitFramebuffer(0, 0, _w, _h, 0, 0, _w, _h, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		drawPost();
		ImGui::Render();

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
	glBindTexture(GL_TEXTURE_2D, m_itex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_depthTex);
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

	if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_2)
		&& !cam->getLockCursor())
	{
		cam->setLockCursor(true);
	}

	if (glfwGetKey(m_window, GLFW_KEY_P))
		cam->printTransform();

	if (glfwGetKey(m_window, GLFW_KEY_R))
	{
		if (currentTexture == m_itex)
			currentTexture = m_depthTex;
		else
			currentTexture = m_itex;
	}

	m_timer += delta;

	//m_drawables[0]->rotate({ 0,1 * delta,0 });
	//m_drawables[1]->rotate({ 0,0,1*delta });
	//m_drawables[2]->rotate({ 1 * delta,0,0 });

	cam->update(delta);

	m_shader->setLightPos(0, cam->getTransform()[3]);

	if (m_autoFocus) {
		int _w, _h;
		glfwGetWindowSize(m_window, &_w, &_h);
		float* butts = new float[_w*_h];
		glBindTexture(GL_TEXTURE_2D, m_depthTex);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, butts);
		int index = ((_h / 2)*_w) + (_w / 2);
		float f = -100.0f * 0.1f / (butts[index] * (100.0f - 0.1f) - 100.0f);

		delete[] butts;

		m_focalDist -= (m_focalDist - f) * delta * 2.0f;
	}

	// imgui stuff
	ImGui::Checkbox("Auto Focus", &m_autoFocus);
	ImGui::SliderFloat("Focal Distance", &m_focalDist, 0.10f, 15);
	ImGui::SliderFloat("Focal Length", &m_focalLength, 0.1f, 20.0f);
	ImGui::SliderFloat("fstop", &m_fstop, 0.1f, 5.0f);

	ImGui::SliderInt("Rings", &m_rings, 1, 20);
	ImGui::SliderInt("Samples", &m_samples, 1, 20);

	ImGui::SliderFloat("CoC", &m_coc, 0.0f, 0.1f);

	ImGui::SliderFloat("Bias", &m_bias, 0.1f, 2.0f);
	ImGui::SliderFloat("Fringe", &m_fringe, 0.0f, 10.0f);

	m_postShader->bindUniform("focalDepth", m_focalDist);
	m_postShader->bindUniform("focalLength", m_focalLength);
	m_postShader->bindUniform("fstop", m_fstop);

	m_postShader->bindUniform("rings", m_rings);
	m_postShader->bindUniform("samples", m_samples);

	m_postShader->bindUniform("CoC", m_coc);
	
	m_postShader->bindUniform("bias", m_bias);
	m_postShader->bindUniform("fringe", m_fringe);
}

void Game::setupFramebuffer()
{
	bool resized = (m_fbo > 0);

	if (resized)
	{
		glDeleteFramebuffers(1, &m_fbo);
		glDeleteTextures(1, &m_tex);
		glDeleteRenderbuffers(1, &m_rbo);

		glDeleteFramebuffers(1, &m_ifbo);
		glDeleteTextures(1, &m_itex);
		//glDeleteRenderbuffers(1, &m_irbo);

		glDeleteTextures(1, &m_depthTex);
	}

	// create buffer
	glGenFramebuffers(1, &m_fbo);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// create texture for frame buffer
	glGenTextures(1, &m_tex);
	//glBindTexture(GL_TEXTURE_2D, m_tex);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_tex);

	int w, h;
	glfwGetWindowSize(m_window, &w, &h);
	//w /= 8;
	//h /= 8;
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, SAMPLES, GL_RGB, w, h, 1);

	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	//glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_tex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_depthTex, 0);

	// make render buffer for depth/stencil since we won't use them in post
	// processing
	glGenRenderbuffers(1, &m_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, SAMPLES, GL_DEPTH24_STENCIL8, w, h);
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
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

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
	m_postShader->loadShader(ShaderStage::FRAGMENT, "shaders/post/dof.frag");
	m_postShader->link();

	m_postShader->use();
	m_postShader->bindUniform("screenTexture", 0U);
	m_postShader->bindTexUniform("depthTexture", 1U);

	currentTexture = m_itex;
}

