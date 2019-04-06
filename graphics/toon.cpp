#include "toon.h"

#include "Texture.h"

#include "gl_core_4_4.h"

ToonShader::ToonShader() : LitShader()
{
	// load ramp texture
	m_rampTexture = new Texture("./shaders/toon_ramp.png");
	m_rampTexture->bind(0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	loadShader(ShaderStage::VERTEX, "shaders/toon.vert");
	loadShader(ShaderStage::FRAGMENT, "shaders/toon.frag");
	link();

	use();

	setLightCount(1);

	setLight(0, { {10,10,10}, {1,1,1,1}, {1,1,1,1} });
}

ToonShader::~ToonShader()
{
	delete m_rampTexture;
}

void ToonShader::beforeDraw()
{
	bindTexUniform("toonRampTexture", 7);

	// bind ramp texture
	m_rampTexture->bind(7);
}
