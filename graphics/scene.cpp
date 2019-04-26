#include "scene.h"

#include "camera.h"
#include "sceneobject.h"

/*
	@brief Creates an empty scene with a camera and a root node
*/
Scene::Scene()
{
	m_camera = new Camera();
	m_root = new SceneObject(nullptr);
	m_root->setScene(this);
}

/*
	@brief Destroys all objects in the scene as well as the camera
*/
Scene::~Scene()
{
	delete m_root;
	delete m_camera;
}

/*
	@brief Add an object as a child of the root node
	@param Object to add to the scene
	@return Pointer to this object, for use in chaining
*/
Scene* Scene::addObject(SceneObject* o)
{
	// make sure the object knows it's part of this scene
	o->setScene(this);
	// add as a child of the root node
	m_root->addChild(o);
	return this;
}
