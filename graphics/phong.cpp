#include "phong.h"

PhongShader::PhongShader() : LitShader()
{
	loadShader(ShaderStage::VERTEX, "shaders/phong.vert");
	loadShader(ShaderStage::FRAGMENT, "shaders/phong.frag");
	link();

	use();

	setLightCount(1);

	setLight(0, { {10,10,10}, {1,1,1,1}, {1,1,1,1} });
}
