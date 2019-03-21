#version 410

layout(location=0)in vec4 Position;
layout(location=1)in vec3 Normal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 MVP;
uniform vec3 LightPos;
uniform float timer;

out vec3 vertNormal;
out vec3 vertPos;
out vec3 lightPos;
out vec3 viewPos;

void main() {
    vec4 pos = MVP * Position;

    vertNormal = normalize((V*M*vec4(Normal,1)).xyz);

    gl_Position = pos;

    vertPos = gl_Position.xyz;

    lightPos = (V*vec4(LightPos,1)).xyz;

    viewPos = (inverse(V)[3]).xyz;
}