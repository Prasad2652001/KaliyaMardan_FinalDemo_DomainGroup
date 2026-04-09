#version 460 core

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texcoord;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 v_Position;
out vec3 v_Normal;
out vec2 v_texcoord;

void main(void)
{
   v_texcoord = a_texcoord;
   mat4 u_MVNormal = transpose(inverse(u_view * u_model));
   v_Normal = normalize((u_MVNormal * vec4(a_normal, 0)).xyz);

   vec4 pos = u_model * vec4(a_position, 1.0);
   v_Position = vec3(pos.xyz)/pos.w;

   gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);

}