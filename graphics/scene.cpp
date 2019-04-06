#include "scene.h"

#include "camera.h"
#include "sceneobject.h"

Scene::Scene()
{
	m_camera = new Camera();
	m_root = new SceneObject(nullptr);
	m_root->setScene(this);
}

Scene::~Scene()
{
	delete m_root;
	delete m_camera;
}

Scene* Scene::addObject(SceneObject* o)
{
	o->setScene(this);
	m_root->addChild(o);
	return this;
}
