#pragma once

#include <glm/ext.hpp>

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

	Camera* setPosition(glm::vec3 p) { m_pos = p; return this; }
	Camera* setRotation(glm::vec2 r) { m_rot = r; return this; }

	// sets whether or not the cursor is captured by the game
	void setLockCursor(bool l);
	// returns whether or not the cursor is currently locked to the window
	bool getLockCursor() const { return m_lockCursor; }

	void updateProjectionMatrix(int windowWidth, int windowHeight,
		float verticalFov = 0.25f, float near = 0.1f, float far = 1000.0f);

	void printTransform();
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
