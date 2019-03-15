#include "quad.h"

#include "gl_core_4_4.h"

Quad::Quad()
{
	if (vao != 0)
		return;

	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	Vertex vertices[4];
	vertices[0].position = { -0.5f, 0.0f, -0.5f, 1.0f };
	vertices[1].position = { -0.5f, 0.0f, 0.5f, 1.0f };
	vertices[2].position = { 0.5f, 0.0f, 0.5f, 1.0f };
	vertices[3].position = { 0.5f, 0.0f, -0.5f, 1.0f };

	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	int indices[6] = {
		0, 1, 2,
		0, 2, 3
	};

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(int), indices, GL_STATIC_DRAW);

	triCount = 2;
}