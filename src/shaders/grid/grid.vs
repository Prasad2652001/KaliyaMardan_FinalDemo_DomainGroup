#version 460 core

in vec4 a_position;

uniform mat4 u_projectionMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_modelMatrix;

out vec4 a_pos_out;

void main(void)
{
    vec4 worldPos = u_modelMatrix * a_position;
    gl_Position = u_projectionMatrix * u_viewMatrix * worldPos;
    a_pos_out = worldPos;
    gl_PointSize = 6.0;
}
