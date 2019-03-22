#pragma once

#include <glm/ext.hpp>

struct GLFWwindow;

class Cube;
class Camera;
class Texture;
class OBJMesh;
class ShaderProgram;

class Game {
public:
	Game();
	~Game();

	int init(char const* title, int width, int height);

	void loop();

	void draw();
	void update(float delta);

private:
	GLFWwindow *m_window;

	glm::mat4 m_projectionMatrix;

	float m_timer;

	Cube *m_cube;
	ShaderProgram *m_shader;

	Texture* m_tex;
	OBJMesh* m_mesh;

	Camera* m_cam;

	bool m_escapeDown;
};
