#version 410
  
in vec2 uv;

uniform sampler2D screenTexture;
uniform vec2 resolution = vec2(100,100);

out vec4 fragColor;

void main()
{
	vec2 thisUv = floor(uv*resolution)/resolution;

    fragColor = texture(screenTexture, thisUv);
}  