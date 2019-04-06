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

	void windowResized(int width, int height) override;

protected:
	int setup() override;
	void draw() override;
	void update(float delta) override;

private:
	std::vector<Drawable*> m_drawables;

	Scene* m_scene;
	OBJMesh* m_mesh;
	LitShader* m_shader;

	bool m_escapeDown;
};
