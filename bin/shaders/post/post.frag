#version 410
  
in vec2 uv;

uniform sampler2D screenTexture;

out vec4 fragColor;

void main()
{
    fragColor = texture(screenTexture, uv);
}  