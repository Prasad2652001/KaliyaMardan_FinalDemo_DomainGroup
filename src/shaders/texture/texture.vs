#version 460 core

in vec4 a_position;
in vec2 a_texcoord;
in vec3 a_normal;


uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
uniform vec2 u_texRepeat;
uniform vec4 u_LightPosition;

out vec2 a_texcoords_out;
out vec3 v_Normal;
out vec3 v_LightDirection;
out vec3 v_ViewerVector;

void main(void)
{
	vec4 eyeCoordinates =  u_viewMatrix *  u_modelMatrix * a_position;
	mat3 normalMatrix = transpose( inverse(mat3(u_viewMatrix * u_modelMatrix)));
	v_Normal = normalize(normalMatrix * a_normal);
	v_LightDirection = vec3(u_LightPosition - eyeCoordinates);
	v_ViewerVector = -eyeCoordinates.xyz;

	a_texcoords_out = a_texcoord * u_texRepeat;
	gl_Position = u_projectionMatrix * eyeCoordinates ;
}




