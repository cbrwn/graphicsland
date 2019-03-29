#version 410
  
in vec2 uv;

uniform sampler2D screenTexture;
uniform float pixelateAmount = 100.0;

out vec4 fragColor;

void main()
{
    fragColor = texture(screenTexture, floor(uv*pixelateAmount)/pixelateAmount);
}  