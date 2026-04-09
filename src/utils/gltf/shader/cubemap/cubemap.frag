#version 460 core

in vec3 a_texcoord_out; 

uniform samplerCube u_CubeSampler; 

out vec4 FragColor; 

void main(void) 
{ 
    FragColor = texture(u_CubeSampler, a_texcoord_out); 
}
