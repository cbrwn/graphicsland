#pragma once

struct GLFWwindow;

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
};
