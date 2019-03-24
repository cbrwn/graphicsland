#pragma once

class Camera;
class SceneObject;

class Scene
{
public:
	Scene();
	~Scene();

	Scene* addObject(SceneObject* o);

	Camera* getCamera() const { return m_camera; }

private:
	Camera* m_camera;
	SceneObject* m_root;
};