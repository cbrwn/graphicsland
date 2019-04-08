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
Game::~Game()
{
	delete m_scene;
	delete m_shader;

	for (int i = 0; i < (int)m_meshes.size(); ++i)
		delete m_meshes[i];
}

void Game::windowResized(int width, int height)
{
	Application::windowResized(width, height);

	m_scene->getCamera()->updateProjectionMatrix(width, height,
		0.25f, 0.1f, 500.0f);
}

int Game::setup()
{
	m_scene = new Scene();
	m_scene->getCamera()->updateProjectionMatrix(m_windowWidth, m_windowHeight);
	//windowResized(m_windowWidth, m_windowHeight);

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

	m_meshes.push_back(new OBJMesh());
	m_meshes.push_back(new OBJMesh());
	m_meshes.push_back(new OBJMesh());
	m_meshes.push_back(new OBJMesh());

	m_meshes[0]->load("models/spear/soulspear.obj", true, true);
	m_meshes[1]->load("models/Dragon.obj", true, true);
	m_meshes[2]->load("models/Bunny.obj", true, true);
	m_meshes[3]->load("models/Lucy.obj", true, true);

	m_drawables.push_back(new Drawable({ 0,0,0 }));
	m_drawables[0]->setMesh(m_meshes[0])->setShader(m_shader);

	m_drawables[0]->scale(glm::vec3(5.0f));

	m_scene->addObject(m_drawables[0]);

	m_selectedShader = 0;
	m_lastSelectedShader = 0;

	return 0;
}

void Game::draw()
{
	for (Drawable* d : m_drawables)
		d->draw();
}

void Game::update(float delta)
{
	// post processing shader selection
	ImGui::Begin("Post-Processing");
	{
		ImGui::Text("Shader");
		const char* items[] = {
			"None", "Swirl", "Pixelate", "Emboss", "Depth of Field"
		};
		ImGui::Combo("", &m_selectedShader, items, 5);
		updateSelectedShader();

		if (m_selectedShader == 1)
		{
			// SWIRL
			static float swirl_radius = 0.25f;
			static float swirl_amount = 0.2f;
			static glm::vec2 swirl_center = { 0.5f,0.5f };

			ImGui::TextColored(ImVec4{ 0,1,1,1 }, "Swirl Properties");

			ImGui::SliderFloat2("Center", &swirl_center.x, 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("Radius", &swirl_radius, 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("Strength", &swirl_amount, -1.0f, 1.0f, "%.2f");

			m_postShader->bindUniform("swirlAmount", swirl_amount);
			m_postShader->bindUniform("swirlRadius", swirl_radius);
			m_postShader->bindUniform("swirlCenter", swirl_center);
		}
		else if (m_selectedShader == 2)
		{
			// PIXELATE
			static glm::vec2 pix_resolution = { 100,100 };
			static bool pix_squarePixels = true;

			ImGui::TextColored(ImVec4{ 0,1,1,1 }, "Pixelate Properties");

			ImGui::Checkbox("Square Pixels", &pix_squarePixels);
			if (!pix_squarePixels)
			{
				ImGui::SliderFloat2("Resolution", &pix_resolution.x, 1.0f, 512.0f, "%.0f");
			}
			else
			{
				float aspect = (float)m_windowWidth / (float)m_windowHeight;
				ImGui::SliderFloat("Resolution", &pix_resolution.x, 1.0f, 512.0f, "%.0f");
				pix_resolution.y = pix_resolution.x / aspect;
			}

			m_postShader->bindUniform("resolution", pix_resolution);
		}
		else if (m_selectedShader == 3)
		{
			// EMBOSS
			static float emboss_offset = 300.0f;

			ImGui::TextColored(ImVec4{ 0,1,1,1 }, "Emboss Properties");

			ImGui::SliderFloat("Kernel Offset", &emboss_offset, 5.0f, 500.0f);

			m_postShader->bindUniform("kernelOffset", emboss_offset);
		}
		else if (m_selectedShader == 4)
		{
			// DEPTH OF FIELD
			static float dof_depth = 1.0f;

			ImGui::TextColored(ImVec4{ 0,1,1,1 }, "Depth of Field Properties");

			ImGui::DragFloat("Focus Depth", &dof_depth);

			m_postShader->bindUniform("focalDepth", dof_depth);
		}
	}
	ImGui::End();

	ImGui::Begin("Controls");
	{
		ImGui::Bullet();
		if (m_scene->getCamera()->getLockCursor())
		{
			ImGui::Text("Escape to release mouse");
		}
		else
		{
			ImGui::Text("Click to capture mouse");
			ImGui::Bullet();
			ImGui::Text("Escape again to exit");
		}
		ImGui::Bullet();
		ImGui::Text("W/A/S/D/Q/E to move");
	}
	ImGui::End();

	ImGui::Begin("Model");
	{
		static int selectedModel = 0;

		const char* items[] = {
			"Soulspear", "Dragon", "Bunny", "Lucy"
		};
		ImGui::Combo("", &selectedModel, items, 4);

		m_drawables[0]->setMesh(m_meshes[selectedModel]);
	}
	ImGui::End();

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
		&& !ImGui::IsMouseHoveringAnyWindow()
		&& !ImGui::IsMouseDragging())
		cam->setLockCursor(true);

	if (glfwGetKey(m_window, GLFW_KEY_P))
		cam->printTransform();

	cam->update(delta);

	m_shader->setLightPos(0, cam->getTransform()[3]);
}

void Game::updateSelectedShader()
{
	if (m_lastSelectedShader == m_selectedShader)
		return;

	m_lastSelectedShader = m_selectedShader;

	// order: "None", "Swirl", "Pixelate", "Emboss", "Depth of Field" 
	const char* c = "shaders/post/post.frag";
	switch (m_selectedShader)
	{
	case 1:
		c = "shaders/post/swirl.frag";
		break;
	case 2:
		c = "shaders/post/pixelate.frag";
		break;
	case 3:
		c = "shaders/post/emboss.frag";
		break;
	case 4:
		c = "shaders/post/mydof.frag";
		break;
	}
	m_postShader->loadShader(ShaderStage::FRAGMENT, c);
	m_postShader->link();
}
