#include "drawable.h"

#include "phong.h"
#include "scene.h"
#include "camera.h"
#include "OBJMesh.h"

using namespace glm;

Drawable::Drawable(glm::vec3 const& pos, glm::vec3 const& rot, 
	SceneObject* parent)
	: SceneObject(pos, rot, parent)
{
	m_mesh = nullptr;
	m_shader = nullptr;
}

void Drawable::draw()
{
	mat4 modelMatrix = getGlobalTransform();
	mat4 viewMatrix = m_scene->getCamera()->getView();
	mat4 projMatrix = m_scene->getCamera()->getProjection();

	m_shader->setModelMatrix(modelMatrix)
		->setViewMatrix(viewMatrix)
		->setMVP(projMatrix * viewMatrix * modelMatrix);
	m_shader->use();
	m_mesh->draw();
}
