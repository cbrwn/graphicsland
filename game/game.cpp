#include "game.h"

#include <imgui.h>

#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include <phong.h>
#include <scene.h>
#include <camera.h>
#include <OBJMesh.h>
#include <drawable.h>

Game::Game() {}
Game::~Game() {
	delete m_scene;
	delete m_mesh;
	delete m_shader;
}

void Game::windowResized(int width, int height)
{
	Application::windowResized(width, height);

	m_scene->getCamera()->updateProjectionMatrix(width, height);
}

int Game::setup()
{
	m_scene = new Scene();
	windowResized(m_windowWidth, m_windowHeight);

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

	m_mesh = new OBJMesh();
	m_mesh->load("models/spear/soulspear.obj", true, true);

	m_drawables.push_back(new Drawable({ 0,0,0 }));
	m_drawables.back()->setMesh(m_mesh)->setShader(m_shader);

	m_drawables[0]->scale(glm::vec3(5.0f));

	m_scene->addObject(m_drawables[0]);

	return 0;
}

void Game::draw()
{
	for (Drawable* d : m_drawables)
		d->draw();
}

void Game::update(float delta)
{
	ImGui::Text("f word");

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
		&& !cam->getLockCursor()
		&& !ImGui::IsMouseHoveringAnyWindow())
		cam->setLockCursor(true);

	if (glfwGetKey(m_window, GLFW_KEY_P))
		cam->printTransform();

	cam->update(delta);

	m_shader->setLightPos(0, cam->getTransform()[3]);
}
