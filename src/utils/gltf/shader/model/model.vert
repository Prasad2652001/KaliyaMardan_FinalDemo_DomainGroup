#version 460 core

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texcoord;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform vec4 u_LightPosition;
uniform mat4 u_lightSpaceMatrix;
uniform bool u_isDepthPass = false;

out vec3 v_Position;
out vec3 v_Normal;
out vec2 v_texcoord;
out vec3 v_LightDirection; 
out vec3 v_ViewerVector; 
out vec4 v_FragPosLightSpace;
out vec3 v_WorldPosition;

void main(void)
{
   vec4 worldPos = u_model * vec4(a_position, 1.0);
   
   if (u_isDepthPass) {
       gl_Position = u_lightSpaceMatrix * worldPos;
       return;
   }

   v_texcoord = a_texcoord;

   vec4 eyeCoordinates = u_view * worldPos; 

   mat3 normalMatrix = transpose(inverse(mat3(u_view * u_model)));
   v_Normal = normalize(normalMatrix * a_normal);

   v_Position = eyeCoordinates.xyz;
   v_WorldPosition = worldPos.xyz;
   v_LightDirection = vec3(u_LightPosition - eyeCoordinates);
   v_ViewerVector = -eyeCoordinates.xyz;

   // Shadow: transform world position to light space
   v_FragPosLightSpace = u_lightSpaceMatrix * worldPos;

   gl_Position = u_projection * eyeCoordinates;
}