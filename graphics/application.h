#pragma once

struct GLFWwindow;
class ShaderProgram;

class Application
{
public:
	Application();
	virtual ~Application();

	/*
		@brief Initializes all window-related things in the class
				This is used instead of the constructor in order to
				return an error code
		@param Title of the window
		@param Width of the window in pixels
		@param Height of the window in pixels
		@param Number of anti aliasing (MSAA) samples
		@param Whether or not postprocessing should be used
		@return An error code: 0=success, 1=glfw init fail,
				2=glfw create window fail, 3=opengl function binding fail
	*/
	int init(char const* title, int width = 854, int height = 480,
		int msaaSamples=2, bool postProcessing=false);

	/*
		@brief Runs the game loop while the window is open
	*/
	void loop();

	// glfw callbacks
	/*
		@brief Called when the window resizes in order to cache the new size
				Called by the GLFW window size callback, so must be public
		@param New width of the window
		@param New height of the window
	*/
	virtual void windowResized(int width, int height);

protected:
	GLFWwindow* m_window;
	int m_windowWidth, m_windowHeight;

	// post processing shader
	ShaderProgram* m_postShader;

	/*
		@brief Function that inherited classes override which acts
				like init(), used for initializing things and
				to have the ability to provide error codes
		@param Error code, or 0 for success
	*/
	virtual int setup() = 0;

	/*
		@brief Called every frame after update, draws the scene before
				any post-processing
	*/
	virtual void draw() = 0;
	/*
		@brief Called every frame and should contain the logic of the game
	*/
	virtual void update(float delta) = 0;

private:
	/*
		@brief Renders a quad showing the current frame using a shader
	*/
	void drawPost();
	/*
		@brief Initializes everything used in post-processing, including the
				framebuffer and render buffer as well as the quad to draw to.
				This is also called when the window is resized in order to make
				a new framebuffer with the correct resolution
	*/
	void setupPostProcessing();
	/*
		@brief A small helper function to clean up the things that need to be
				re-made when resized
	*/
	void cleanupPostProcessing();

	// MSAA samples
	int m_samples;

	///////////////////////////
	// POST PROCESSING STUFF //
	///////////////////////////
	// whether or not we use post processing
	bool m_postProcessing;

	// multisampled framebuffer
	unsigned int m_fbo;
	// multisampled texture
	unsigned int m_tex;
	// renderbuffer to hold depth/stencil info
	unsigned int m_rbo;

	// intermediate stuff for converting MSAA to regular image
	unsigned int m_ifbo;
	unsigned int m_itex;
	unsigned int m_depthTex;

	// quad for rendering the final texture to screen
	unsigned int m_quadVao, m_quadVbo;
};