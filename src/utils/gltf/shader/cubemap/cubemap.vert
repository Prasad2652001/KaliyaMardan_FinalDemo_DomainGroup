#version 460 core

in vec4 a_position;

uniform mat4 u_model; 
uniform mat4 u_view; 
uniform mat4 u_projection; 

out vec3 a_texcoord_out;

void main(void)
{
	vec4 worldPos = u_model * a_position;

	a_texcoord_out = worldPos.xyz;
	mat4 rotationView = mat4(mat3(u_view));
	
	gl_Position = u_projection * rotationView * u_model * a_position;
}
