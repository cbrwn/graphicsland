#version 410

layout(location=0)in vec4 Position;
layout(location=1)in vec3 Normal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 LightPos;
uniform float timer;

out vec3 vertPos;
out vec3 lightPos;
out vec3 viewPos;

void main() {
    mat4 MVP = P * V * M;

    vec4 pos = MVP * Position;

    vertNormal = normalize((V*M*vec4(Normal,1)).xyz);

    gl_Position = pos;

    vertPos = gl_Position.xyz;

    lightPos = (V*vec4(LightPos,1)).xyz;

    viewPos = (inverse(V)[3]).xyz;
    // // stuff for diffuse lighting
    // vec3 posWorld = (M*Position).xyz;

    // vec3 vertCam = (V*M*Position).xyz;
    // vec3 eyeDir = vec3(0)-vertCam;

    // vec3 lightPos = vec3(10,10,10);

    // vec3 lightPosCam = (V*vec4(lightPos,1)).xyz;
    // vec3 lightDirCam = lightPosCam + eyeDir;

    // vec3 normCam = (V*M*vec4(Normal,1)).xyz;
}