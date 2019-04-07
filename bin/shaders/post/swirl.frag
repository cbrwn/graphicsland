#version 410
  
in vec2 uv;

uniform sampler2D screenTexture;
uniform vec2 swirlCenter = vec2(0.5, 0.5);
uniform float swirlAmount = 0.2;
uniform float swirlRadius = 0.25;

out vec4 fragColor;

void main()
{
	vec2 screenSize = textureSize(screenTexture, 0);
	float swirlRadiusPx = min(screenSize.x, screenSize.y) * swirlRadius;

    vec2 tc = uv * screenSize;
    tc -= (screenSize*swirlCenter);
    float dist = length(tc);

    if (dist < swirlRadiusPx)
    {
        float percent = (swirlRadiusPx - dist) / swirlRadiusPx;
        float theta = percent * percent * swirlAmount * 8.0f;
        float s = sin(theta);
        float c = cos(theta);
        tc = vec2(dot(tc, vec2(c, -s)), dot(tc, vec2(s, c)));
    }
    tc += (screenSize*swirlCenter);
    vec3 col = texture(screenTexture, tc / screenSize).rgb;
    fragColor = vec4(col, 1.0f);
}  
