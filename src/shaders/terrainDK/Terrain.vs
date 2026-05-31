#version 460 core

in vec4 a_position;
in vec2 a_texcoord;
in vec3 a_normal;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

uniform vec3 lightPosition;

uniform vec4 u_plane;

out vec2 a_texcoord_out;
out vec3 surfaceNormal;
out vec3 toLightVector;
out vec3 toCameraVector;
out float gl_ClipDistance[1];

void main(void)
{

vec4 worldPosition = u_modelMatrix * a_position;

a_texcoord_out = a_texcoord;
gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;
//gl_ClipDistance[0] = dot(u_modelMatrix * a_position, u_plane);

surfaceNormal = (u_modelMatrix * vec4(a_normal,0.0)).xyz;
toLightVector = lightPosition - worldPosition.xyz;
toCameraVector = (inverse(u_viewMatrix) * vec4(0.0,0.0,0.0,1.0)).xyz - worldPosition.xyz;
}
