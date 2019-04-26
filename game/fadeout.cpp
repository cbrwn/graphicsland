#include "fadeout.h"

#include <Texture.h>
#include <gl_core_4_4.h>

FadeoutShader::FadeoutShader() : LitShader()
{
	// load ramp texture
	m_fadeTexture = new Texture("./shaders/noisetex.png");
	m_fadeTexture->bind(0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	loadShader(ShaderStage::VERTEX, "shaders/fadeout.vert");
	loadShader(ShaderStage::FRAGMENT, "shaders/fadeout.frag");
	link();

	use();

	// set default light
	setLightCount(1);
	setLight(0, { {10,10,10}, {1,1,1,1}, {1,1,1,1} });
}

FadeoutShader::~FadeoutShader()
{
	delete m_fadeTexture;
}

void FadeoutShader::beforeDraw()
{
	// tell the shader it will be bound to slot 7
	bindTexUniform("fadeTexture", 7);

	// bind fade texture to slot 7
	m_fadeTexture->bind(7);
}
