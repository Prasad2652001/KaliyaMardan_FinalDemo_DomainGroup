#version 460 core

in vec4 a_position; 
in vec3 a_Normal; 

uniform mat4 u_modelMatrix; 
uniform mat4 u_viewMatrix; 
uniform mat4 u_projectionMatrix; 
uniform vec4 u_LightPosition; 

out vec3 out_transformed_normal; 
out vec3 out_lightDirection; 
out vec3 out_viewerVector; 

void main(void) 
{ 
    gl_Position = u_projectionMatrix  * u_viewMatrix * u_modelMatrix * a_position; 
        vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position; 
        mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix); 
        out_transformed_normal = normalMatrix * a_Normal;
        out_lightDirection = vec3(u_LightPosition - eyeCoordinates); 
        out_viewerVector = -eyeCoordinates.xyz; 
} ;
