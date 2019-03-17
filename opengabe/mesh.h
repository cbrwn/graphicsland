#pragma once

#include <glm/ext.hpp>

class Mesh {
public:
	Mesh();
	virtual ~Mesh();

	struct Vertex {
		glm::vec4 position;
		glm::vec4 normal;
		glm::vec2 texCoord;
	};

	virtual void draw();

protected:
	unsigned int triCount;
	// opengl object pointers
	unsigned int vao, vbo, ibo;
};