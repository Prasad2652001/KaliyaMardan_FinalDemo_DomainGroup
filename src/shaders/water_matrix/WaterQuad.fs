#version 460 core

in vec4 clipSpaceCoords;
in vec2 a_texcoords_out;
in vec3 toCameraVector;
in vec3 lightDirection;

//For Vignette
uniform vec2 u_resolution;

uniform float u_vignetteOuterRadius;
uniform float u_vignetteInnerRadius;

in vec4 a_color_out;

uniform sampler2D u_reflectionTextureSampler;
uniform sampler2D u_refractionTextureSampler;
uniform sampler2D u_waterDUDVMapTextureSampler;
uniform sampler2D u_waterNormalMapTextureSampler;

uniform float u_moveFactorOffset;
uniform vec3 u_lightColor;
uniform float interpolateDarkToBright = 0.0;

uniform int waterColor_JisDesh = 0;

// 0.0 = calm (default, unchanged look); 1.0 = full storm chop.
uniform float u_stormStrength = 0.0;

float waveStrength = 0.04;
float shininess = 10.0;
float reflectivity = 0.001;
out vec4 FragColor;

float vignette_main(void) {
    // A - Final Fragment Color Before Vignette
    vec4 color = a_color_out;

    // B - Applying Vignette ...
    vec2 relativePosition;
    relativePosition.x = (gl_FragCoord.x / u_resolution.x);
    relativePosition.x = relativePosition.x - 0.5;

    relativePosition.y = (gl_FragCoord.y / u_resolution.y);
    relativePosition.y = relativePosition.y - 0.5;

    float len = length(relativePosition);
    float vignette = smoothstep(u_vignetteOuterRadius, u_vignetteInnerRadius, len);

    // C - Final Fragment Color WITH Vignette
    return (vignette);
}

void main(void) {
    if(waterColor_JisDesh == 0) {
        waveStrength = 0.01;
        shininess = 10.0;
        reflectivity = 0.1;
    } else {
        waveStrength = 0.04;
        shininess = 10.0;
        reflectivity = 0.001;
    }

    // Storm amplifies surface chop. Keep specular broad so the fixed light
    // does not bake a hard steady streak.
    float storm = clamp(u_stormStrength, 0.0, 1.0);
    waveStrength *= (1.0 + storm * 6.0);
    shininess     = mix(shininess, 18.0, storm);
    reflectivity  = mix(reflectivity, 0.12, storm);

    //Convert Clip-space coordinates to Screen-space coordinates
    vec2 screenSpaceCoords;
    screenSpaceCoords.x = clipSpaceCoords.x / clipSpaceCoords.w;
    screenSpaceCoords.y = clipSpaceCoords.y / clipSpaceCoords.w;

    //Convert Screen-space coordinates to normalized device coordinates
    vec2 ndcCoords;
    ndcCoords.x = screenSpaceCoords.x / 2.0 + 0.5;
    ndcCoords.y = screenSpaceCoords.y / 2.0 + 0.5;

    vec2 distortedTexCoords = texture(u_waterDUDVMapTextureSampler, vec2(a_texcoords_out.x + u_moveFactorOffset, a_texcoords_out.y)).rg * 0.1;
    distortedTexCoords = a_texcoords_out + vec2(distortedTexCoords.x, distortedTexCoords.y + u_moveFactorOffset);
    vec2 totalDistortions = (texture(u_waterDUDVMapTextureSampler, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;

    // Second, finer-and-faster DUDV layer drifting the other way: stacks with the
    // base layer to create the chaotic, choppy look of a stormy surface.
    // Second, finer-and-faster DUDV layer drifting the other way: stacks with the
    // base layer to create the chaotic, choppy look of a stormy surface.
    if (storm > 0.0) {
        vec2 dc2 = texture(u_waterDUDVMapTextureSampler,
                           vec2(a_texcoords_out.x * 2.7 - u_moveFactorOffset * 1.9,
                                a_texcoords_out.y * 2.7)).rg * 0.1;
        dc2 = a_texcoords_out * 2.7 + vec2(dc2.x, dc2.y - u_moveFactorOffset * 1.9);
        vec2 td2 = (texture(u_waterDUDVMapTextureSampler, dc2).rg * 2.0 - 1.0) * waveStrength * 1.6;
        totalDistortions += td2 * storm;
    }

    //Reflection texcoords
    vec2 reflectTexcoords = vec2(ndcCoords.x, -ndcCoords.y);
    reflectTexcoords = reflectTexcoords + totalDistortions * 2.0;

    //Clamp reflection Texture coordinated between 0.001 and 0.999
    reflectTexcoords.x = clamp(reflectTexcoords.x, 0.001, 0.999);
    reflectTexcoords.y = clamp(reflectTexcoords.y, -0.999, -0.001);

    vec4 reflectColor = texture(u_reflectionTextureSampler, reflectTexcoords);

    //Refraction texcoords
    vec2 refractTexcoords = vec2(ndcCoords.x, ndcCoords.y);
    refractTexcoords = refractTexcoords + totalDistortions;

    //Clamp refraction Texture coordinated between 0.001 and 0.999
    refractTexcoords.x = clamp(refractTexcoords.x, 0.001, 0.999);
    refractTexcoords.y = clamp(refractTexcoords.y, 0.001, 0.999);

    vec4 refractColor = texture(u_refractionTextureSampler, refractTexcoords);

    //For Fresnel Effect
    vec3 viewVector = normalize(toCameraVector);
    vec3 waterSurfaceNormal = vec3(0.0, 1.0, 0.0);
    float refractiveFactor = dot(viewVector, waterSurfaceNormal);
    refractiveFactor = pow(refractiveFactor, 2.0);

    vec4 color = mix(reflectColor, refractColor, refractiveFactor);

    //For Specular HighLights
    vec4 normalMapColor = texture(u_waterNormalMapTextureSampler, distortedTexCoords);
    // Slightly steepen the normals for a choppier read, but keep the up (b)
    // component dominant so the surface stays roughly upward-facing (a near-flat
    // normal across the whole quad is what produced the steady specular line).
    vec3 normal = vec3((normalMapColor.r * 2.0 - 1.0) * (1.0 + storm * 0.6),
                       normalMapColor.b,
                       (normalMapColor.g * 2.0 - 1.0) * (1.0 + storm * 0.6));
    normal = normalize(normal);

    vec3 reflectedLight = reflect(normalize(lightDirection), normal);
    float specular = max(dot(reflectedLight, viewVector), 0.0);
    specular = pow(specular, shininess);
    vec3 specularHighlights = u_lightColor * specular * reflectivity;

    float red = 1.0;
    float green = 1.0;
    float blue = 1.0;
    vec4 waterColor;

    red = 10.0 / 255.0;
    green = 15.0 / 255.0;
    blue = 60.0 / 255.0; // Little dark blue
    vec4 darkColor = vec4(red, green, blue, 0.5);
    vec4 brightBlue = vec4(0.05, 0.20, 0.45, 1.0);
    vec4 finalWaterColor = mix(darkColor, brightBlue, interpolateDarkToBright);

    // Pull the tint toward a cold, dark grey-green for an overcast storm mood.
    vec4 stormColor = vec4(0.06, 0.11, 0.12, 1.0);
    finalWaterColor = mix(finalWaterColor, stormColor, storm * 0.65);

    // Make waterMixFactor lower so the reflection (baseColor) is more visible (70% reflection, 30% water color tint)
    float waterMixFactor = mix(0.3, 0.2 + storm * 0.15, interpolateDarkToBright);
    
    // Don't darken the reflection too much, so the clouds are clearly visible
    vec4 baseColor = color * mix(0.7, 1.0, interpolateDarkToBright);
    vec3 dimmedSpecular = specularHighlights * mix(0.5, 1.0, interpolateDarkToBright);

    waterColor = mix(baseColor, finalWaterColor, waterMixFactor) + vec4(dimmedSpecular, 1.0);

    FragColor = mix(waterColor, waterColor, 1.0);
}
