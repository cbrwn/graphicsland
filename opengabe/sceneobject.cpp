#include "sceneobject.h"

using namespace glm;

SceneObject::SceneObject(SceneObject* parent)
{
	m_scene = nullptr;
	m_localTransform = mat4(1);
	setParent(parent);
}

SceneObject::SceneObject(mat4 const& transform, SceneObject* parent)
	: m_localTransform(transform)
{
	m_scene = nullptr;
	setParent(parent);
}

SceneObject::SceneObject(vec3 const& pos, vec3 const& rot,
	SceneObject* parent)
	: m_parent(parent)
{
	m_scene = nullptr;
	setParent(parent);

	mat4 posMatrix(1);
	posMatrix = translate(posMatrix, pos);

	mat4 rotX(1), rotY(1), rotZ(1);
	rotX = glm::rotate(rotX, rot.x, { 1,0,0 });
	rotY = glm::rotate(rotY, rot.y, { 0,1,0 });
	rotZ = glm::rotate(rotZ, rot.z, { 0,0,1 });

	mat4 rotMatrix(1);
	rotMatrix = rotZ * rotY * rotX;

	m_localTransform = rotMatrix * posMatrix;
}

SceneObject::~SceneObject()
{
	for (SceneObject *child : m_children)
		delete child;
}

void SceneObject::setScene(Scene* s)
{
	if (!m_scene)
		for (SceneObject* c : m_children)
			c->setScene(s);
	m_scene = s;
}

SceneObject* SceneObject::rotate(glm::vec3 const & rot)
{
	mat4 matrix(1);
	matrix = glm::rotate(matrix, rot.x, { 1,0,0 });
	matrix = glm::rotate(matrix, rot.y, { 0,1,0 });
	matrix = glm::rotate(matrix, rot.z, { 0,0,1 });

	m_localTransform = m_localTransform * matrix;
	return this;
}

mat4 SceneObject::getGlobalTransform() const
{
	if (m_parent)
		return m_parent->getGlobalTransform() * m_localTransform;
	return m_localTransform;
}

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

void SceneObject::addChild(SceneObject* c, bool parent)
{
	if (parent)
		c->setParent(this, false);

	m_children.push_back(c);
}

void SceneObject::removeChild(SceneObject* c)
{
	auto iterator = std::find(m_children.begin(), m_children.end(), c);
	if (iterator != m_children.end())
		m_children.erase(iterator);
}
