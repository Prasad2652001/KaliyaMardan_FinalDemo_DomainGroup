#version 460 core

in vec4 a_color_out;
out vec4 FragColor;
in vec3 a_texcoords;
uniform samplerCube u_skyBox;
uniform int isBarsat = 0;
uniform float u_alpha;
uniform float u_time;
uniform float u_barasatBlend;

// Procedural 3D noise for clouds
float hash(vec3 p) {
    p = fract(p * 0.3183099 + .1);
    p *= 17.0;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

float noise(vec3 x) {
    vec3 i = floor(x);
    vec3 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);
    
    return mix(mix(mix(hash(i + vec3(0,0,0)), hash(i + vec3(1,0,0)), f.x),
                   mix(hash(i + vec3(0,1,0)), hash(i + vec3(1,1,0)), f.x), f.y),
               mix(mix(hash(i + vec3(0,0,1)), hash(i + vec3(1,0,1)), f.x),
                   mix(hash(i + vec3(0,1,1)), hash(i + vec3(1,1,1)), f.x), f.y), f.z);
}

float fbm(vec3 p) {
    float f = 0.0;
    float w = 0.5;
    for (int i=0; i<5; i++) {
        f += w * noise(p);
        p *= 2.0;
        w *= 0.5;
    }
    return f;
}

void main(void) {
	if(isBarsat == 1) {
		FragColor = texture(u_skyBox, a_texcoords) * vec4(0.6, 0.6, 0.6, 1.0) * u_alpha;
	} else if (isBarsat == 2) {
        // Procedural dark clouds
        vec3 dir = normalize(a_texcoords);
        float time = u_time * 0.5;
        
        // Use FBM for cloud density
        float n = fbm(dir * 4.0 + vec3(time, 0.0, time*0.5));
        
        // Threshold and smooth for cloud shapes (lowered threshold for MORE clouds)
        float density = smoothstep(0.1, 0.7, n);
        
        // Colors
        vec3 skyColor = vec3(0.02, 0.05, 0.08); // dark night sky
        vec3 cloudColor = vec3(0.08, 0.25, 0.12); // darker green-tinted clouds
        
        vec3 stormSky = mix(skyColor, cloudColor, density);
        vec3 originalSky = texture(u_skyBox, a_texcoords).rgb;
        
        // Smoothly blend between the base cubemap and the procedural storm based on u_barasatBlend
        FragColor = vec4(mix(originalSky, stormSky, u_barasatBlend), 1.0) * u_alpha;
    } else {
		FragColor = texture(u_skyBox, a_texcoords) * u_alpha;
	}
}
