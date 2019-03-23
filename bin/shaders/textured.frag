#version 410

struct LightInfo {
	vec3 pos;
	vec4 diffuse;
	vec4 specular;
};

in vec3 viewPos;
in vec3 vertNormal;
in vec3 vertPos;
in vec2 uv;

uniform sampler2D friggenTexture;

// material info
uniform vec3 Kd; // diffuse
uniform vec3 Ka; // ambient
uniform vec3 Ks; // specular
uniform vec3 Ke; // emission
uniform float opacity = 1.0;
uniform float specularPower = 16.0;

uniform float ambientLight = 0.0;

uniform LightInfo lights[8];
uniform uint lightCount;

out vec4 fragColor;

void main() {
    vec3 norm = normalize(vertNormal);
    vec3 viewDir = normalize(viewPos - vertPos);

	vec4 col = vec4(Ka,1.0) * ambientLight;

	// using names from the equation at 
	// https://en.wikipedia.org/wiki/Phong_reflection_model
	for(int i = 0; i < lightCount; ++i) {
		// this light
		LightInfo light = lights[i];

		// direction from light
		vec3 Lm = normalize(light.pos - vertPos);
		float lightvsnormal = clamp(dot(Lm, norm), 0.0, 1.0);
		vec4 diffusePart = vec4(Kd,opacity) * lightvsnormal * light.diffuse;

		vec3 Rm = reflect(-Lm, norm);
		float reflection = pow(max(dot(Rm, viewDir), 0.0), specularPower);
		vec4 specularPart = vec4(Ks,1.0) * reflection * light.specular;

		col += (diffusePart + specularPart);
	}

	fragColor = col + vec4(Ke, 1.0);
}