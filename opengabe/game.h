#pragma once

#include <vector>
#include <glm/ext.hpp>

struct GLFWwindow;

class Scene;
class OBJMesh;
class Drawable;
class LitShader;
class ShaderProgram;

#define SAMPLES 8 

class Game {
public:
	Game();
	~Game();

	int init(char const* title, int width, int height);

	void loop();

	void draw();
	void update(float delta);

	Scene* getScene() const { return m_scene; }

	void setupFramebuffer();

private:
	GLFWwindow *m_window;

	float m_timer;

	Scene* m_scene;

	bool m_escapeDown;

	OBJMesh *m_bunny;
	OBJMesh *m_dragon;
	OBJMesh *m_buddha;

	LitShader* m_shader;

	std::vector<Drawable*> m_drawables;

	///////////////////////////
	// POST PROCESSING STUFF //
	///////////////////////////
	// frame buffer stuff
	unsigned int m_fbo, m_tex, m_rbo;
	// depth buffer test
	unsigned int m_depthTex;
	// intermediate rendering stuff for MSAA
	unsigned int m_ifbo, m_itex;//, m_irbo;
	// quad for rendering framebuffer
	unsigned int m_quadVao, m_quadVbo;
	// shader
	ShaderProgram* m_postShader;

	unsigned int currentTexture;

	float focalDist;

	void drawPost();
};
