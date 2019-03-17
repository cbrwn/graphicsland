#include "cube.h"

#include "gl_core_4_4.h"

Cube::~Cube()
{
}

Cube::Cube()
{
	if (vao != 0)
		return;

	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	float vertices[] = {
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f

	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	// normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	triCount = 12;
}

//Cube::Cube()
//{
//	if (vao != 0)
//		return;
//
//	glGenBuffers(1, &vbo);
//	glGenBuffers(1, &ibo);
//	glGenVertexArrays(1, &vao);
//
//	glBindVertexArray(vao);
//
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//
//	Vertex vertices[8];
//	vertices[0].position = { -0.5f, -0.5f, -0.5f, 1.0f };
//	vertices[1].position = { -0.5f, -0.5f, 0.5f, 1.0f };
//	vertices[2].position = { 0.5f, -0.5f, 0.5f, 1.0f };
//	vertices[3].position = { 0.5f, -0.5f, -0.5f, 1.0f };
//
//	vertices[4].position = { -0.5f, 0.5f, -0.5f, 1.0f };
//	vertices[5].position = { -0.5f, 0.5f, 0.5f, 1.0f };
//	vertices[6].position = { 0.5f, 0.5f, 0.5f, 1.0f };
//	vertices[7].position = { 0.5f, 0.5f, -0.5f, 1.0f };
//
//	for (int i = 0; i < 8; ++i)
//		vertices[i].normal = vertices[i].position;
//
//	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(Vertex), vertices, GL_STATIC_DRAW);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)(sizeof(float) * 4));
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
//
//	int indices[36] = {
//		// bottom face
//		2, 1, 0,
//		3, 2, 0,
//
//		// top face
//		4, 5, 6,
//		4, 6, 7,
//
//		// left face
//		0, 1, 5,
//		0, 5, 4,
//
//		// right face
//		2, 3, 7,
//		2, 7, 6,
//
//		// front face
//		1, 2, 5,
//		6, 5, 2,
//
//		// back face
//		4, 3, 0,
//		7, 3, 4,
//	};
//
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(int), indices, GL_STATIC_DRAW);
//
//	triCount = 12;
//}
