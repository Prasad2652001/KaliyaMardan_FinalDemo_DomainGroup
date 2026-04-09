#version 460 core

in vec4 a_position;
in vec2 a_texcoord;

out vec2 a_texcoords_out;

void main(void) {
    gl_Position = a_position;
    a_texcoords_out = a_texcoord; 
}