#pragma once

#include "litshader.h"

class Texture;

class ToonShader : public LitShader
{
public:
	ToonShader();
	~ToonShader();

	/*
		@brief Called before use, used to bind the toon ramp texture
	*/
	void beforeDraw() override;

protected:
	Texture* m_rampTexture;
};