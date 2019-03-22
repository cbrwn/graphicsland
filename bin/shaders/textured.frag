#version 410

struct LightInfo {
	vec3 pos;
	vec4 color;
};

in vec3 viewPos;
in vec3 vertNormal;
in vec3 vertPos;
in vec2 uv;

uniform sampler2D friggenTexture;
uniform vec3 Kd;
uniform vec3 Ka;
//uniform vec3 LightPos;
uniform float ambientStrength;

uniform LightInfo lights[100];
uniform int lightCount;

out vec4 fragColor;

void main() {
    vec3 norm = normalize(vertNormal);

	vec4 col = vec4(Ka, 1.0) * ambientStrength;

//    vec3 lightCol = vec3(1,1,1);
//    vec3 lightDif = normalize(LightPos-vertPos);
//    float cosTheta = clamp(dot(norm, lightDif), 0.0, 1.0);
//
//    vec3 viewDir = normalize(viewPos - vertPos);
//    vec3 reflectDir = reflect(-lightDif, norm);
//
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
//    vec3 specular = 0.4f * spec * lightCol;
//
//    vec3 diffuse = cosTheta * lightCol;
//
//	float ambientStrength = 0.2;
//
//    vec4 col = vec4(Kd, 1.0);
//
//    float dist = 1.0;
//
//    fragColor =  vec4(diffuse + specular + vec3(ambientStrength), 1.0) * col;
//}