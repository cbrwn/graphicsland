#pragma once

#include <application.h>

#include <vector>

class Scene;
class OBJMesh;
class Drawable;
class LitShader;

class Game : public Application
{
public:
	Game();
	~Game();

	/*
		@brief Override window resize callback in order to update the camera's
				projection matrix
		@param width of the window
		@param height of the window
	*/
	void windowResized(int width, int height) override;

protected:
	/*
		@brief Function that inherited classes override which acts
				like init(), used for initializing things and
				to have the ability to provide error codes
		@param Error code, or 0 for success
	*/
	int setup() override;
	/*
		@brief Called every frame after update, draws the scene before
				any post-processing
	*/
	void draw() override;
	/*
		@brief Called every frame and should contain the logic of the game
	*/
	void update(float delta) override;

private:
	// list of all drawable objects
	std::vector<Drawable*> m_drawables;

	// scene containing hierarchy
	Scene* m_scene;
	// all loaded meshes
	std::vector<OBJMesh*> m_meshes;
	// all loaded object shaders
	std::vector<LitShader*> m_shaders;
	// reference to current object shader to change uniforms
	LitShader* m_currentShader;

	// indices of selected shader and last selected shader as an 
	// easy way to check if it has changed
	int m_selectedShader;
	int m_lastSelectedShader;

	// whether not escape is down, temporary thing since
	// I haven't written a proper input handler
	bool m_escapeDown;

	/*
		@brief Changes the post-processing shader when selected
	*/
	void updateSelectedShader();
};
