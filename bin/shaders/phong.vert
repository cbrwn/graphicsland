#version 410

layout(location=0)in vec4 Position;
layout(location=1)in vec4 Normal;
layout(location=2)in vec2 TexCoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 MVP;
uniform float timer;

out vec3 vertNormal;
out vec3 vertPos;
out vec3 viewPos;
out vec2 uv;

void main() {
    vec4 pos = MVP * Position;

	mat3 rot = mat3(M);

    vertNormal = normalize(rot*Normal.xyz);

    gl_Position = pos;

    vertPos = (M*Position).xyz;

    viewPos = (inverse(V)[3]).xyz;

	uv = TexCoord;
}