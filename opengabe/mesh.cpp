#include "mesh.h"

#include "gl_core_4_4.h"

Mesh::Mesh() { 
	vao = 0;
	vbo = 0;
	ibo = 0;
	triCount = 0;
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}

void Mesh::draw()
{
	glBindVertexArray(vao);

	if (ibo == 0)
		glDrawArrays(GL_TRIANGLES, 0, 3 * triCount);
	else
		glDrawElements(GL_TRIANGLES, 3 * triCount, GL_UNSIGNED_INT, 0);
}
