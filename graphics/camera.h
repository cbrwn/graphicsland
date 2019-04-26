#pragma once

#include <glm/ext.hpp>

// class which acts as a camera object with a position and rotation
// also handles input for movement and rotation
class Camera {
public:
	Camera(glm::vec3 pos = { 0,0,0 }, glm::vec2 rot = { 0,0 });

	// called every frame to process mouse and keyboard inputs
	// and update the view matrix
	void update(float delta);

	// the current view matrix
	glm::mat4 getView() const { return m_view; }
	// the transformation matrix
	glm::mat4 getTransform() const { return glm::inverse(m_view); }
	glm::mat4 getProjection() const { return m_proj; }

	// add to rotation
	void rotate(glm::vec2 r) { m_rot += r; }
	// add to position
	void move(glm::vec3 d) { m_pos += d; }

	// sets camera position 
	Camera* setPosition(glm::vec3 p) { m_pos = p; return this; }
	// sets camera rotation
	// takes in a vector2, with x being yaw and y being pitch
	Camera* setRotation(glm::vec2 r) { m_rot = r; return this; }

	// sets whether or not the cursor is captured by the game
	void setLockCursor(bool l);
	// returns whether or not the cursor is currently locked to the window
	bool getLockCursor() const { return m_lockCursor; }

	// re-creates the projection matrix
	// used when window is resized to update the aspect ratio
	void updateProjectionMatrix(int windowWidth, int windowHeight,
		float verticalFov = 0.25f, float near = 0.1f, float far = 1000.0f);

private:
	// how fast move.
	float m_moveSpeed;

	// view matrix
	glm::mat4 m_view;
	// projection matrix
	glm::mat4 m_proj;
	// individual transformations
	glm::vec3 m_pos;
	glm::vec2 m_rot;

	// whether or not the cursor is locked to the window
	bool m_lockCursor;
};
