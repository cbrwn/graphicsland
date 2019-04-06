#pragma once

struct GLFWwindow;
class ShaderProgram;

class Application
{
public:
	Application();
	virtual ~Application();

	int init(char const* title, int width = 854, int height = 480,
		int msaaSamples=2, bool postProcessing=false);

	void loop();

	// glfw callbacks
	virtual void windowResized(int width, int height);

protected:
	GLFWwindow* m_window;
	int m_windowWidth, m_windowHeight;

	// post processing shader
	ShaderProgram* m_postShader;

	virtual int setup() = 0;

	virtual void draw() = 0;
	virtual void update(float delta) = 0;

private:
	void drawPost();
	void setupPostProcessing();
	void cleanupPostProcessing();

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