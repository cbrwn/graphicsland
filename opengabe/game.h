#pragma once

#include <vector>
#include <glm/ext.hpp>

struct GLFWwindow;

class Scene;
class OBJMesh;
class Drawable;
class PhongShader;

class Game {
public:
	Game();
	~Game();

	int init(char const* title, int width, int height);

	void loop();

	void draw();
	void update(float delta);

	Scene* getScene() const { return m_scene; }

private:
	GLFWwindow *m_window;

	float m_timer;

	Scene* m_scene;

	bool m_escapeDown;

	OBJMesh *m_bunny;
	OBJMesh *m_dragon;
	OBJMesh *m_buddha;

	PhongShader* m_shader;

	std::vector<Drawable*> m_drawables;
};
