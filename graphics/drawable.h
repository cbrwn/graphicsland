#pragma once

#include "sceneobject.h"

class OBJMesh;
class LitShader;

class Drawable : public SceneObject
{
public:
	Drawable(glm::vec3 const& pos, glm::vec3 const& rot = { 0,0,0 },
		SceneObject* parent = nullptr);

	Drawable* setMesh(OBJMesh* mesh) { m_mesh = mesh; return this; }
	Drawable* setShader(LitShader* s) { m_shader = s; return this; }

	void draw();

private:
	OBJMesh* m_mesh;
	LitShader* m_shader;
};
