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

/*
texture flag stuff
0b0001 = diffuse texture
0b0010 = normal texture
0b0100 = specular texture
*/
uniform uint textureFlags = 0;

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D specularTexture;

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
	vec3 norm = vertNormal;
	if((textureFlags & 0x2) > 0) {
		// make tangent basis matrix
		vec3 t = tangent.xyz;
		vec3 b = cross(vertNormal, tangent.xyz)*tangent.w ;
		mat3 tbn = mat3(t, b, vertNormal);
//
		// transform normal map from 0-1 to -1-1
		vec3 normTexture = (texture(normalTexture, uv).xyz * 2.0)-1;
		norm = tbn * normTexture;
	}

    vec3 viewDir = normalize(viewPos - vertPos);
	vec4 texColor = texture(diffuseTexture, uv);
	vec4 specTex = texture(specularTexture, uv);

	if((textureFlags & 0x1) == 0)
		texColor = vec4(1.0);
	if((textureFlags & 0x4) == 0)
		specTex = vec4(1.0);

	vec4 col = vec4(Ka,1.0) * ambientLight;

	// using names from the equation at 
	// https://en.wikipedia.org/wiki/Phong_reflection_model
	for(int i = 0; i < lightCount; ++i) {
		// this light
		LightInfo light = lights[i];

		// direction from light
		vec3 Lm = normalize(light.pos - vertPos);
		float lightvsnormal = clamp(dot(Lm, norm), 0.0, 1.0);

		vec4 diffusePart = (vec4(Kd, opacity) * texColor ) * lightvsnormal * light.diffuse;

		vec3 Rm = reflect(-Lm, norm);
		float reflection = pow(max(dot(Rm, viewDir), 0.0), specularPower);
		vec4 specularPart = (vec4(Ks,1.0) * specTex) * reflection * light.specular;

		col += (diffusePart + specularPart);
	}

	fragColor = vec4(norm,1.0);//col + vec4(Ke, opacity);
}
