#pragma once

#include "sceneobject.h"

class OBJMesh;
class LitShader;

class Drawable : public SceneObject
{
public:
	/*
		@brief Creates a new drawable object
		@param Local position of the object
		@param Optional rotation of the object
		@param Optional parent in the scene hierarchy
	*/
	Drawable(glm::vec3 const& pos, glm::vec3 const& rot = { 0,0,0 },
		SceneObject* parent = nullptr);

	/*
		@brief Assigns a mesh to this object
		@param Mesh for this object to show when drawn
		@return Pointer to this object, for use in chaining
	*/
	Drawable* setMesh(OBJMesh* mesh) { m_mesh = mesh; return this; }
	/*
		@brief Assigns a shader to this object
		@param Shader for this object to use when drawn
		@return Pointer to this object, for use in chaining
	*/
	Drawable* setShader(LitShader* s) { m_shader = s; return this; }

	/*
		@brief Draws the object using its shader
	*/
	void draw();

private:
	// mesh that is drawn in draw()
	OBJMesh* m_mesh;
	// shader used to draw the mesh
	LitShader* m_shader;
};
