#pragma once

#include <litshader.h>

class Texture;

// demonstrating writing a custom shader class using the library
class FadeoutShader : public LitShader
{
public:
	FadeoutShader();
	~FadeoutShader();

	/*
		@brief Called before use, used to bind the fade texture
	*/
	void beforeDraw() override;

protected:
	Texture* m_fadeTexture;
};