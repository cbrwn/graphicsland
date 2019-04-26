#pragma once

#include <vector>
#include <glm/ext.hpp>

class Scene;

// base class for all objects which will be part of a scene
// includes functionality for a hierarchy, with a parent and children
class SceneObject
{
public:
	/*
		@brief Creates a SceneObject with identity matrix for transformation
				Optionally gives the object a parent
		@param Pointer to the parent of the new object
	*/
	SceneObject(SceneObject* parent = nullptr);
	/*
		@brief Creates a SceneObject with a specified transformation matrix
				Optionally gives the object a parent
		@param Desired transformation matrix of the new object
		@param Pointer to the parent of the new object
	*/
	SceneObject(glm::mat4 const& transform, SceneObject* parent = nullptr);
	/*
		@brief Creates a SceneObject with a specified position and rotation
				Optionally gives the object a parent
		@param Desired local position of the new object
		@param Desired local rotation of the new object in radians
		@param Pointer to the parent of the new object
	*/
	SceneObject(glm::vec3 const& pos, glm::vec3 const& rot,
		SceneObject* parent = nullptr);
	/*
		@brief Destroys all children along with this object
	*/
	~SceneObject();

	/*
		@brief Gets a pointer to the scene which contains this object
		@return Pointer to the scene containing this object
	*/
	Scene* getScene() const { return m_scene; }
	/*
		@brief Sets the parent scene of the object
		@param Pointer to the scene that the object should be a part of
	*/
	void setScene(Scene* s);

	/*
		@brief Rotates the object 
		@param Euler rotation to apply in radians
		@return Pointer to this object, for use in chaining
	*/
	SceneObject* rotate(glm::vec3 const& rot);
	/*
		@brief Scales the object
		@param Amount to scale the object by
		@return Pointer to this object, for use in chaining
	*/
	SceneObject* scale(glm::vec3 const& scale);

	/*
		@brief Gets the local transformation matrix of the object
		@return Copy of the 4x4 transformation matrix
	*/
	glm::mat4 getLocalTransform() const { return m_localTransform; }
	/*
		@brief Gets the global transformation matrix of the object, taking into
				account the scene hierarchy
		@return Copy of the 4x4 global transformation matrix
	*/
	glm::mat4 getGlobalTransform() const;

	/*
		@brief Gets a pointer to the object's parent in the scene hierarchy
		@return Pointer to a scene object, nullptr if no parent
	*/
	SceneObject* getParent() const { return m_parent; }
	/*
		@brief Sets the parent of this object in the scene hierarchy
		@param Pointer to the new parent of this object
		@param Whether or not we should add this object to the new
				parent's list of children
	*/
	void setParent(SceneObject* p, bool child = true);

	/*
		@brief Adds an object as a child of this object in the hierarchy
		@param New child of this object
		@param Whether or not we should set this object as the child's parent
	*/
	void addChild(SceneObject* c, bool parent = true);
	/*
		@brief Removes a child from this object
		@param Object to remove from list of children
	*/
	void removeChild(SceneObject* c);

protected:
	// scene that this object is a part of
	Scene* m_scene;
	// parent of this object
	SceneObject* m_parent;

	// local transformation matrix
	glm::mat4 m_localTransform;

	// list of children in the scene hierarchy
	std::vector<SceneObject*> m_children;
};