#version 410

layout(location=0)in vec4 Position;
layout(location=1)in vec3 Normal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main() {
    mat4 MVP = P * V * M;
    vec4 pos = MVP * Position;
    gl_Position = pos;
}