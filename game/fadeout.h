#pragma once

#include <litshader.h>

class Texture;

class FadeoutShader : public LitShader
{
public:
	FadeoutShader();
	~FadeoutShader();

	void beforeDraw() override;

protected:
	Texture* m_fadeTexture;
};