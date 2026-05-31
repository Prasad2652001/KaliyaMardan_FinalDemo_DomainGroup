#version 460 core

in vec4 a_position; 
in vec2 a_TexCoord; 

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

out vec2 out_texCoord; 

void main(void) 
{ 
    // gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;  // use this for actual output in praportion but it will be in squar
    gl_Position =  a_position;  // use this to get full screen coverdd in FBO
    out_texCoord = a_TexCoord; 
} ;