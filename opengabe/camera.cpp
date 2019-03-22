#include "camera.h"

#include <GLFW/glfw3.h>

Camera::Camera()
{
	m_pos = glm::vec3(0);
	m_rot = glm::vec3(0);
	m_view = glm::mat4(1);

	m_moveSpeed = 10.0f;

	setLockCursor(true);
}

// upd8
void Camera::update(float delta) {

	// get pointer to context for input stuff
	GLFWwindow* win = glfwGetCurrentContext();

	// movement
	glm::vec4 move(0);
	// get current transformation matrix for the up/right/forward vectors
	glm::mat4 trans = getTransform();
	// forward
	if (glfwGetKey(win, GLFW_KEY_W))
		move -= trans[2];
	// backward
	if (glfwGetKey(win, GLFW_KEY_S))
		move += trans[2];
	// left
	if (glfwGetKey(win, GLFW_KEY_A))
		move -= trans[0];
	// right
	if (glfwGetKey(win, GLFW_KEY_D))
		move += trans[0];
	// up
	if (glfwGetKey(win, GLFW_KEY_E))
		move += trans[1];
	// down
	if (glfwGetKey(win, GLFW_KEY_Q))
		move -= trans[1];
	this->move(glm::vec3(move)*m_moveSpeed*delta);


	if (m_lockCursor) {
		// rotation

		// get window size for locking and stuff homie
		int wx, wy;
		glfwGetWindowSize(win, &wx, &wy);

		// get current mouse position
		double _mx, _my;
		glfwGetCursorPos(win, &_mx, &_my);

		// make a vector from the difference between 
		// the last and current positions
		glm::vec2 rotate(0);
		rotate.y = -(float)(_mx - (wx / 2));
		rotate.x = -(float)(_my - (wy / 2));

		// multiply by something tiny because it is crazy
		const float sens = 0.001f;
		// add to rotation :D
		this->rotate(rotate*sens);

		// locking cursor to the middle
		glfwSetCursorPos(win, wx / 2, wy / 2);
	}

	// update view matrix

	// make position matrix
	glm::mat4 posMatrix(1);
	posMatrix = glm::translate(posMatrix, m_pos);

	// make individual rotation matrices
	glm::mat4 rotx(1);
	glm::mat4 roty(1);
	rotx = glm::rotate(rotx, m_rot.x, { 1,0,0 });
	roty = glm::rotate(roty, m_rot.y, { 0,1,0 });
	// combine into one rotation matrix
	glm::mat4 rotMatrix = roty * rotx;

	// update view matrix :D
	m_view = glm::inverse(posMatrix*rotMatrix);
}

// does things
void Camera::setLockCursor(bool l)
{
	GLFWwindow* win = glfwGetCurrentContext();

	m_lockCursor = l;
	// show/hide cursor based on bool
	// GLFW_CURSOR_DISABLED hides the cursor as well as locking it
	// to the window
	glfwSetInputMode(win, GLFW_CURSOR, l ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

	// reset the mouse position to the center of the window when disabled
	if (!l) {
		int wx, wy;
		glfwGetWindowSize(win, &wx, &wy);
		glfwSetCursorPos(win, wx / 2, wy / 2);
	}

}
