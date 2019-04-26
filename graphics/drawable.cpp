#include "drawable.h"

#include "phong.h"
#include "scene.h"
#include "camera.h"
#include "OBJMesh.h"

using namespace glm;

/*
	@brief Creates a new drawable object
	@param Local position of the object
	@param Optional rotation of the object
	@param Optional parent in the scene hierarchy
*/
Drawable::Drawable(glm::vec3 const& pos, glm::vec3 const& rot,
	SceneObject* parent)
	: SceneObject(pos, rot, parent)
{
	m_mesh = nullptr;
	m_shader = nullptr;
}

/*
	@brief Draws the object using its shader
*/
void Drawable::draw()
{
	// get matrices to pass into shader
	mat4 modelMatrix = getGlobalTransform();
	mat4 viewMatrix = m_scene->getCamera()->getView();
	mat4 projMatrix = m_scene->getCamera()->getProjection();

	m_shader->use();
	// pass matrices as well as the combined MVP into the shader
	m_shader->setModelMatrix(modelMatrix)
		->setViewMatrix(viewMatrix)
		->setMVP(projMatrix * viewMatrix * modelMatrix);
	// shaders can have a beforeDraw function which can be implemented
	// to do things like bind custom textures
	m_shader->beforeDraw();
	// use OBJMesh's draw function
	m_mesh->draw(false);
}
