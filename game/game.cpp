#include "game.h"

#include <imgui.h>

#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include <toon.h>
#include <phong.h>
#include "fadeout.h"

#include <scene.h>
#include <camera.h>
#include <OBJMesh.h>
#include <drawable.h>

Game::Game() {}
Game::~Game()
{
	delete m_scene;

	for (int i = 0; i < (int)m_shaders.size(); ++i)
		delete m_shaders[i];
	for (int i = 0; i < (int)m_meshes.size(); ++i)
		delete m_meshes[i];
}

/*
	@brief Override window resize callback in order to update the camera's
			projection matrix
	@param width of the window
	@param height of the window
*/
void Game::windowResized(int width, int height)
{
	Application::windowResized(width, height);

	m_scene->getCamera()->updateProjectionMatrix(width, height,
		0.25f, 0.1f, 500.0f);
}

/*
	@brief Function that inherited classes override which acts
			like init(), used for initializing things and
			to have the ability to provide error codes
	@param Error code, or 0 for success
*/
int Game::setup()
{
	m_scene = new Scene();
	// make sure the projection matrix is correct
	m_scene->getCamera()->updateProjectionMatrix(m_windowWidth, m_windowHeight);

	// make all our model shaders
	m_shaders.push_back(new PhongShader());
	m_shaders.push_back(new ToonShader());
	m_shaders.push_back(new FadeoutShader());

	m_currentShader = m_shaders[0];

	// setup lights in all model shaders
	for (LitShader* l : m_shaders)
	{
		l->setLightCount(2)
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
	}

	// allocate meshes in the vector for our models
	m_meshes.push_back(new OBJMesh());
	m_meshes.push_back(new OBJMesh());
	m_meshes.push_back(new OBJMesh());
	m_meshes.push_back(new OBJMesh());

	// load all models we can choose from
	m_meshes[0]->load("models/spear/soulspear.obj", true, true);
	m_meshes[1]->load("models/Dragon.obj", true, true);
	m_meshes[2]->load("models/Bunny.obj", true, true);
	m_meshes[3]->load("models/Lucy.obj", true, true);

	m_drawables.push_back(new Drawable({ 0,0,0 }));
	m_drawables[0]->setMesh(m_meshes[0])->setShader(m_currentShader);
	// scale it to suit the camera's speed
	m_drawables[0]->scale(glm::vec3(5.0f));
	// make sure the object is part of the scene
	m_scene->addObject(m_drawables[0]);

	m_selectedShader = 0;
	m_lastSelectedShader = 0;

	return 0;
}

/*
	@brief Called every frame after update, draws the scene before
			any post-processing
*/
void Game::draw()
{
	// draw all drawable objects
	for (Drawable* d : m_drawables)
		d->draw();
}

/*
	@brief Called every frame and should contain the logic of the game
*/
void Game::update(float delta)
{
	// post processing shader selection
	ImGui::Begin("Post-Processing");
	{
		ImGui::Text("Shader");
		// post processing shader dropdown
		const char* items[] = {
			"None", "Swirl", "Pixelate", "Emboss"
		};
		ImGui::Combo("", &m_selectedShader, items, 4);
		updateSelectedShader();

		if (m_selectedShader == 1)
		{
			// SWIRL
			// values to be used by ImGui
			static float swirl_radius = 0.25f;
			static float swirl_amount = 0.2f;
			static glm::vec2 swirl_center = { 0.5f,0.5f };

			ImGui::TextColored(ImVec4{ 0,1,1,1 }, "Swirl Properties");

			// show sliders
			ImGui::SliderFloat2("Center", &swirl_center.x, 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("Radius", &swirl_radius, 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("Strength", &swirl_amount, -1.0f, 1.0f, "%.2f");

			// apply values to uniforms
			m_postShader->bindUniform("swirlAmount", swirl_amount);
			m_postShader->bindUniform("swirlRadius", swirl_radius);
			m_postShader->bindUniform("swirlCenter", swirl_center);
		}
		else if (m_selectedShader == 2)
		{
			// PIXELATE
			// values to be used by ImGui
			static glm::vec2 pix_resolution = { 100,100 };
			static bool pix_squarePixels = true;

			ImGui::TextColored(ImVec4{ 0,1,1,1 }, "Pixelate Properties");

			// toggle for whether or not pixels should be square
			ImGui::Checkbox("Square Pixels", &pix_squarePixels);
			if (!pix_squarePixels)
			{
				// 2 sliders for width and height
				ImGui::SliderFloat2("Resolution", &pix_resolution.x, 1.0f, 512.0f, "%.0f");
			}
			else
			{
				// one slider for width
				ImGui::SliderFloat("Resolution", &pix_resolution.x, 1.0f, 512.0f, "%.0f");

				// set the height based on the aspect ratio of the window
				float aspect = (float)m_windowWidth / (float)m_windowHeight;
				pix_resolution.y = pix_resolution.x / aspect;
			}

			// apply the values using the uniform
			m_postShader->bindUniform("resolution", pix_resolution);
		}
		else if (m_selectedShader == 3)
		{
			// EMBOSS
			static float emboss_offset = 300.0f;

			ImGui::TextColored(ImVec4{ 0,1,1,1 }, "Emboss Properties");

			// slider for offset
			ImGui::SliderFloat("Kernel Offset", &emboss_offset, 5.0f, 500.0f);

			// apply offset via uniform
			m_postShader->bindUniform("kernelOffset", emboss_offset);
		}
	}
	ImGui::End();

	// print relevant controls in a window
	ImGui::Begin("Controls");
	{
		ImGui::Bullet();

		if (m_scene->getCamera()->getLockCursor())
		{
			// cursor is locked, let user know they can release it
			ImGui::Text("Escape to release mouse");
		}
		else
		{
			// cursor is unlocked, let user know they can grab it again
			ImGui::Text("Click to capture mouse");
			ImGui::Bullet();
			// or exit the program with escape
			ImGui::Text("Escape again to exit");
		}
		// you can always move, so always display this
		ImGui::Bullet();
		ImGui::Text("W/A/S/D/Q/E to move");
	}
	ImGui::End();

	// ability to change model that's being drawn
	ImGui::Begin("Model");
	{
		// static values to be used by ImGui
		static int selectedModel = 0;
		static int selectedShader = 0;
		static float fade_amount = 0.5f;
		static float fade_power = 2.0f;

		// draw dropdown for model selection
		const char* modelItems[] = {
			"Soulspear", "Dragon", "Bunny", "Lucy"
		};
		ImGui::Combo("Model", &selectedModel, modelItems, (int)m_meshes.size());

		// draw dropdown for shader selection
		const char* shaderItems[] = {
			"Phong", "Toon", "Fade Out"
		};
		ImGui::Combo("Shader", &selectedShader, shaderItems, (int)m_shaders.size());

		m_currentShader = m_shaders[selectedShader];

		// set the mesh of the only object in the scene
		m_drawables[0]->setMesh(m_meshes[selectedModel])
			->setShader(m_currentShader);

		// only the fade out shader has uniforms we can change
		if (selectedShader == 2)
		{
			ImGui::SliderFloat("Amount", &fade_amount, 0.2f, 0.5f);

			m_currentShader->bindUniform("fadeAmount", fade_amount);
		}
	}
	ImGui::End();

	// camera controls
	Camera* cam = m_scene->getCamera();
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		if (!m_escapeDown)
		{
			if (!cam->getLockCursor())
			{
				// close the program if the cursor is already free
				glfwSetWindowShouldClose(m_window, true);
			}
			else
			{
				// unlock the cursor if it's currently locked
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

	// lock the cursor if the game view was clicked
	if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_1)
		&& !cam->getLockCursor()
		&& !ImGui::IsMouseHoveringAnyWindow()
		&& !ImGui::IsMouseDragging())
		cam->setLockCursor(true);

	// update camera which processes its own input
	cam->update(delta);

	// update the camera's light
	m_currentShader->setLightPos(0, cam->getTransform()[3]);
}

/*
	@brief Changes the post-processing shader when selected
*/
void Game::updateSelectedShader()
{
	if (m_lastSelectedShader == m_selectedShader)
		return;

	m_lastSelectedShader = m_selectedShader;

	// order: "None", "Swirl", "Pixelate", "Emboss"
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
	}
	m_postShader->loadShader(ShaderStage::FRAGMENT, c);
	m_postShader->link();
}
