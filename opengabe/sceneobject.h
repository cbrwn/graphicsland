#pragma once

#include <vector>
#include <glm/ext.hpp>

class Scene;

class SceneObject
{
public:
	SceneObject(SceneObject* parent = nullptr);
	SceneObject(glm::mat4 const& transform, SceneObject* parent = nullptr);
	SceneObject(glm::vec3 const& pos, glm::vec3 const& rot,
		SceneObject* parent = nullptr);
	~SceneObject();

	Scene* getScene() const { return m_scene; }
	void setScene(Scene* s);

	SceneObject* rotate(glm::vec3 const& rot);

	glm::mat4 getLocalTransform() const { return m_localTransform; }
	glm::mat4 getGlobalTransform() const;

	SceneObject* getParent() const { return m_parent; }
	void setParent(SceneObject* p, bool child = true);

	void addChild(SceneObject* c, bool parent = true);
	void removeChild(SceneObject* c);

	char name[128];

protected:
	Scene* m_scene;
	SceneObject* m_parent;

	glm::mat4 m_localTransform;

	std::vector<SceneObject*> m_children;
};