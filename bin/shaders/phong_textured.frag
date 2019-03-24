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
in vec4 tangent;

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;

// material info
uniform vec3 Ka; // ambient
uniform vec3 Kd; // diffuse
uniform vec3 Ks; // specular
uniform vec3 Ke; // emission
uniform float opacity = 1.0;
uniform float specularPower = 16.0;

uniform float ambientLight = 0.0;

uniform LightInfo lights[8];
uniform uint lightCount;

out vec4 fragColor;

void main() {
	// make tangent basis matrix
	mat3 tbn;
	tbn[0] = tangent.xyz;
	tbn[1] = cross(vertNormal, tangent.xyz);
	tbn[2] = vertNormal;

	// transform normal map from 0-1 to -1-1
	vec3 normTexture = (texture(normalTexture, uv).xyz * 2.0)-(vec3(1));
	vec3 norm = tbn * normTexture;

    vec3 viewDir = normalize(viewPos - vertPos);
	vec4 texColor = texture(diffuseTexture, uv);
	vec4 specTex = texture(specularTexture, uv);

	vec4 col = vec4(Ka,1.0) * ambientLight;

	// using names from the equation at 
	// https://en.wikipedia.org/wiki/Phong_reflection_model
	for(int i = 0; i < lightCount; ++i) {
		// this light
		LightInfo light = lights[i];

		// direction from light
		vec3 Lm = normalize(light.pos - vertPos);
		float lightvsnormal = clamp(dot(Lm, norm), 0.0, 1.0);
		vec4 diffusePart = (vec4(Kd, opacity) * texColor) * lightvsnormal * light.diffuse;

		vec3 Rm = reflect(-Lm, norm);
		float reflection = pow(max(dot(Rm, viewDir), 0.0), specularPower);
		vec4 specularPart = (vec4(Ks,1.0) * specTex) * reflection * light.specular;

		col += (diffusePart);// + specularPart);
	}

	fragColor = vec4(Kd, 1.0);//col + vec4(Ke, opacity);
}