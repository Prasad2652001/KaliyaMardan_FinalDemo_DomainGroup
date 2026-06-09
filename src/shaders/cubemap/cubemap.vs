#version 460 core

in vec4 a_position;
uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
out vec3 a_texcoords;

void main(void)
{
	// Sample direction is the raw cube position (world space). The view matrix
	// rotates the geometry, so the sky stays fixed in the world as the camera turns.
	a_texcoords = vec3(a_position);
	gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(vec3(a_position), 1.0);
};
