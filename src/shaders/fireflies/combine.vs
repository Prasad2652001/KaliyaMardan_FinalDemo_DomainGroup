#version 460 core

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texcoord;

out vec2 out_texCoord;

void main(void)
{
    out_texCoord = a_texcoord;
    gl_Position = vec4(a_position, 0.0, 1.0);
}
