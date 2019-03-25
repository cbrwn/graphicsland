#version 410
layout(location=0) in vec2 Pos;
layout(location=1) in vec2 TexCoord;

out vec2 uv;

void main()
{
    gl_Position = vec4(Pos.x, Pos.y, 0.0, 1.0); 
    uv = TexCoord;
}  