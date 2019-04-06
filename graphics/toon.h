#pragma once

#include "litshader.h"

class Texture;

class ToonShader : public LitShader
{
public:
	ToonShader();
	~ToonShader();

	void beforeDraw() override;

protected:
	Texture* m_rampTexture;
};