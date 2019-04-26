#pragma once

class Camera;
class SceneObject;

class Scene
{
public:
	/*
		@brief Creates an empty scene with a camera and a root node
	*/
	Scene();
	/*
		@brief Destroys all objects in the scene as well as the camera
	*/
	~Scene();

	/*
		@brief Add an object as a child of the root node
		@param Object to add to the scene
		@return Pointer to this object, for use in chaining
	*/
	Scene* addObject(SceneObject* o);

	/*
		@brief Gets the camera used in this scene
		@return Pointer to the camera object
	*/
	Camera* getCamera() const { return m_camera; }

private:
	// each scene has a camera used for drawing
	Camera* m_camera;
	// root node
	SceneObject* m_root;
};