#pragma once

#include <glm/ext.hpp>

struct GLFWwindow;

class Cube;
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

	glm::mat4 m_viewMatrix;
	glm::mat4 m_projectionMatrix;

	float m_timer;

	Cube *m_cube;
	ShaderProgram *m_shader;

	glm::mat4 camMatrix;
	double m_px, m_py;
	glm::vec3 camPos;
	float m_rotX, m_rotY;
};
