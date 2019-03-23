#pragma once

#include <glm/ext.hpp>

struct GLFWwindow;

class Cube;
class Camera;
class Texture;
class OBJMesh;
class PhongShader;

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

	Camera* m_cam;
	OBJMesh* m_mesh;
	PhongShader *m_shader;

	bool m_escapeDown;
};
