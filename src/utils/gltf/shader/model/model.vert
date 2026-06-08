#version 460 core

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texcoord;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform vec4 u_LightPosition;

uniform mat4 u_lightSpaceMatrix;

out vec3 v_Position;
out vec3 v_Normal;
out vec2 v_texcoord;
out vec3 v_LightDirection; 
out vec3 v_ViewerVector; 
out vec4 FragPosLightSpace;


void main(void)
{
   v_texcoord = a_texcoord;

   vec4 eyeCoordinates = u_view * u_model * vec4(a_position, 1.0f); 

   mat3 normalMatrix = transpose(inverse(mat3(u_view * u_model)));
   v_Normal = normalize(normalMatrix * a_normal);

   vec4 pos = u_model * vec4(a_position, 1.0);

   v_Position = eyeCoordinates.xyz;
   v_LightDirection = vec3(u_LightPosition - eyeCoordinates);
   v_ViewerVector = -eyeCoordinates.xyz;

   FragPosLightSpace = u_lightSpaceMatrix * pos;

   gl_Position = u_projection * eyeCoordinates;
}