#version 460 core

in vec3 out_transformed_normal; 
in vec3 out_lightDirection; 
in vec3 out_viewerVector; 

uniform vec3 u_LA ; 
uniform vec3 u_LD; 
uniform vec3 u_LS ; 
uniform vec3 u_KA ; 
uniform vec3 u_KD; 
uniform vec3 u_KS; 

uniform float u_MaterialShininess; 
// uniform int u_KeyisPressed; 

out vec4 FragColor; 
void main(void) 
{
    vec3 fhong_ADS_Light; 
    // if(u_KeyisPressed == 1) 
    { 
        vec3 normalized_transformed_normal = normalize(out_transformed_normal); 
        vec3 normalized_lightDirection  = normalize(out_lightDirection); 
        vec3 normalized_viewerVector = normalize(out_viewerVector); 
        vec3 ambientLight = u_LA * u_KA; 
        // calcu_LAte the floor factor between level for diffuse light to render/color shade
        float diff = max(dot(normalized_lightDirection, normalized_transformed_normal) , 0.0); 

        // By floor calcu_LAtion -> uniform value
        float levels = 4.0; 
        diff = floor(diff *  levels) / levels; 

        // hard code method fixed
        // if(diff > 0.95) diff = 1.0; 
        // else if(diff > 0.5) diff = 0.7; 
        // else if(diff > 0.25) diff = 0.4; 
        // else diff = 0.1;  

        vec3 diffuseLight = u_LD * u_KD * diff; 

        // calcu_LAte rim for toon
        float rim = 1.0 - max(dot(normalized_viewerVector, normalized_transformed_normal), 0.0);
        rim = smoothstep(0.6, 1.0, rim);
        vec3 rimLight = vec3(1.0) * rim * 0.5; 

        // calcu_LAte specu_LAr light for proper toon effect
        vec3 reflectionVector = reflect(-normalized_lightDirection, normalized_transformed_normal); 
        float spec = pow(max(dot(reflectionVector, normalized_viewerVector), 0.0), u_MaterialShininess); 
        spec = spec > 0.5 ? 1.0 : 0.0; 
        vec3 specu_LArLight = u_LS * u_KS * spec; 

        fhong_ADS_Light = ambientLight + diffuseLight + specu_LArLight + rimLight; 
    // } 
    // else 
    // { 
    //     fhong_ADS_Light = vec3 (1.0f, 1.0f, 1.0f);  
    } 

    FragColor = vec4(fhong_ADS_Light, 1.0f); 
};


// cmt from vertex shader
// uniform int u_KeyisPressed;  
//   if(u_KeyisPressed == 1) {}