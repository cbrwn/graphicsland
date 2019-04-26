#include "sceneobject.h"

using namespace glm;

/*
	@brief Creates a SceneObject with identity matrix for transformation
			Optionally gives the object a parent
	@param Pointer to the parent of the new object
*/
SceneObject::SceneObject(SceneObject* parent)
{
	m_scene = nullptr;
	m_localTransform = mat4(1);
	setParent(parent);
}

/*
	@brief Creates a SceneObject with a specified transformation matrix
			Optionally gives the object a parent
	@param Desired transformation matrix of the new object
	@param Pointer to the parent of the new object
*/
SceneObject::SceneObject(mat4 const& transform, SceneObject* parent)
	: m_localTransform(transform)
{
	m_scene = nullptr;
	setParent(parent);
}

/*
	@brief Creates a SceneObject with a specified position and rotation
			Optionally gives the object a parent
	@param Desired local position of the new object
	@param Desired local rotation of the new object in radians
	@param Pointer to the parent of the new object
*/
SceneObject::SceneObject(vec3 const& pos, vec3 const& rot,
	SceneObject* parent)
	: m_parent(parent)
{
	m_scene = nullptr;
	setParent(parent);

	// make matrix out of the position and rotation

	// position matrix
	mat4 posMatrix(1);
	posMatrix = translate(posMatrix, pos);

	// rotation matrix
	// individual matrices for each axis
	mat4 rotX(1), rotY(1), rotZ(1);
	rotX = glm::rotate(rotX, rot.x, { 1,0,0 });
	rotY = glm::rotate(rotY, rot.y, { 0,1,0 });
	rotZ = glm::rotate(rotZ, rot.z, { 0,0,1 });
	// combine into one matrix
	mat4 rotMatrix(1);
	rotMatrix = rotZ * rotY * rotX;

	// resulting matrix
	m_localTransform = rotMatrix * posMatrix;
}

/*
	@brief Destroys all children along with this object
*/
SceneObject::~SceneObject()
{
	// delete all children
	for (SceneObject *child : m_children)
		delete child;
}

/*
	@brief Sets the parent scene of the object
	@param Pointer to the scene that the object should be a part of
*/
void SceneObject::setScene(Scene* s)
{
	if (!m_scene)
		for (SceneObject* c : m_children)
			c->setScene(s);
	m_scene = s;
}

/*
	@brief Rotates the object
	@param Euler rotation to apply in radians
	@return Pointer to this object, for use in chaining
*/
SceneObject* SceneObject::rotate(glm::vec3 const & rot)
{
	mat4 matrix(1);
	matrix = glm::rotate(matrix, rot.x, { 1,0,0 });
	matrix = glm::rotate(matrix, rot.y, { 0,1,0 });
	matrix = glm::rotate(matrix, rot.z, { 0,0,1 });

	m_localTransform = m_localTransform * matrix;
	return this;
}

/*
	@brief Scales the object
	@param Amount to scale the object by
	@return Pointer to this object, for use in chaining
*/
SceneObject* SceneObject::scale(glm::vec3 const& scale)
{
	mat4 m(1);
	m = glm::scale(m, scale);

	m_localTransform = m_localTransform * m;
	return this;
}

/*
	@brief Gets the global transformation matrix of the object, taking into
			account the scene hierarchy
	@return Copy of the 4x4 global transformation matrix
*/
mat4 SceneObject::getGlobalTransform() const
{
	if (m_parent)
		return m_parent->getGlobalTransform() * m_localTransform;
	return m_localTransform;
}

/*
	@brief Sets the parent of this object in the scene hierarchy
	@param Pointer to the new parent of this object
	@param Whether or not we should add this object to the new
			parent's list of children
*/
void SceneObject::setParent(SceneObject* p, bool child)
{
	if (p && p->getScene())
		m_scene = p->getScene();

	if (m_parent)
		m_parent->removeChild(this);

	m_parent = p;

	if (child && m_parent)
		m_parent->addChild(this, false);
}

/*
	@brief Adds an object as a child of this object in the hierarchy
	@param New child of this object
	@param Whether or not we should set this object as the child's parent
*/
void SceneObject::addChild(SceneObject* c, bool parent)
{
	if (parent)
		c->setParent(this, false);

	m_children.push_back(c);
}

/*
	@brief Removes a child from this object
	@param Object to remove from list of children
*/
void SceneObject::removeChild(SceneObject* c)
{
	auto iterator = std::find(m_children.begin(), m_children.end(), c);
	if (iterator != m_children.end())
		m_children.erase(iterator);
}
