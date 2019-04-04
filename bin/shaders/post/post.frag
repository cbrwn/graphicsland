#version 410
  
in vec2 uv;

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;

out vec4 fragColor;


float LinearizeDepth(float depth) 
{
	float n = 0.1f; // camera z near
    float f = 20.0f; // camera z far
    return 1.0 - ((2.0 * n) / (f + n - depth * (f - n)));
}

void main()
{
//    fragColor = texture(screenTexture, uv);// * (vec4(vec3(1.0-texture(depthTexture,uv).r),1.0));
    fragColor = vec4(texture(screenTexture, uv).rgb * vec3(LinearizeDepth(texture(depthTexture, uv).r)), 1);
}  