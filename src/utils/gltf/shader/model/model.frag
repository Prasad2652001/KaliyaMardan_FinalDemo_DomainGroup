#version 460 core

in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_texcoord;

out vec4 FragColor;

uniform float u_EnvIntensity;

// IBL
uniform int u_MipCount;

// General Material
uniform sampler2D u_BaseColorSampler;
uniform sampler2D u_NormalSampler;
// Metallic Roughness Material
uniform sampler2D u_MetallicRoughnessSampler;
uniform sampler2D u_EmissiveSampler;
uniform sampler2D u_AmbientSampler;

uniform sampler2D u_GGXLUT;

uniform bool isBlack = false;

uniform samplerCube u_LambertianEnvSampler;
uniform samplerCube u_GGXEnvSampler;

uniform float u_Exposure;

// Metallic Roughness
uniform float u_MetallicFactor;
uniform float u_RoughnessFactor;
uniform float u_alpha = 1.0;

struct PBRInfo {
    float NdotL;                  // cos angle between normal and light direction
    float NdotV;                  // cos angle between normal and view direction
    float NdotH;                  // cos angle between normal and half vector
    float LdotH;                  // cos angle between light direction and half vector
    float VdotH;                  // cos angle between view direction and half vector
    float perceptualRoughness;    // roughness value, as authored by the model creator (input to shader)
    float metalness;              // metallic value at the surface
    vec3 reflectance0;            // full reflectance color (normal incidence angle)
    vec3 reflectance90;           // reflectance color at grazing angle
    float alphaRoughness;         // roughness mapped to a more linear change in the roughness (proposed by [2])
    vec3 diffuseColor;            // color contribution from diffuse lighting
    vec3 specularColor;           // color contribution from specular lighting
};

const float M_PI = 3.141592653589793;
const float c_MinRoughness = 0.04;

vec4 SRGBtoLINEAR(vec4 srgbIn) {
    vec3 linOut = pow(srgbIn.xyz, vec3(2.2));
    return vec4(linOut, srgbIn.w);
}

// Find the normal for this fragment, pulling either from a predefined normal map
// or from the interpolated mesh normal and tangent attributes.
vec3 getNormal() {
    vec3 pos_dx = dFdx(v_Position);
    vec3 pos_dy = dFdy(v_Position);
    vec3 tex_dx = dFdx(vec3(v_texcoord, 0.0));
    vec3 tex_dy = dFdy(vec3(v_texcoord, 0.0));
    vec3 t = (tex_dy.t * pos_dx - tex_dx.t * pos_dy) / (tex_dx.s * tex_dy.t - tex_dy.s * tex_dx.t);

    vec3 ng = v_Normal;

    t = normalize(t - ng * dot(ng, t));
    vec3 b = normalize(cross(ng, t));
    mat3 tbn = mat3(t, b, ng);

    vec3 n = texture(u_NormalSampler, v_texcoord).rgb;
    n = normalize(tbn * (2.0 * n - 1.0));

    return n;
}

vec3 getIBLContribution(PBRInfo pbrInputs, vec3 n, vec3 reflection) {
    float mipCount = 7.0; // resolution of 256x256
    float lod = pbrInputs.perceptualRoughness * mipCount;
    // retrieve a scale and bias to F0. See [1], Figure 3
    vec3 brdf = SRGBtoLINEAR(texture(u_GGXLUT, vec2(pbrInputs.NdotV, 1.0 - pbrInputs.perceptualRoughness))).rgb;
    vec3 diffuseLight = SRGBtoLINEAR(texture(u_LambertianEnvSampler, n)).rgb;
    vec3 specularLight = SRGBtoLINEAR(texture(u_GGXEnvSampler, reflection, lod)).rgb;

    vec3 diffuse = diffuseLight * pbrInputs.diffuseColor;
    vec3 specular = specularLight * (pbrInputs.specularColor * brdf.x + brdf.y);

    return diffuse + specular;
}

// Basic Lambertian diffuse
vec3 diffuse(PBRInfo pbrInputs) {
    return pbrInputs.diffuseColor / M_PI;
}

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
vec3 specularReflection(PBRInfo pbrInputs) {
    return pbrInputs.reflectance0 + (pbrInputs.reflectance90 - pbrInputs.reflectance0) * pow(clamp(1.0 - pbrInputs.VdotH, 0.0, 1.0), 5.0);
}

// This calculates the specular geometric attenuation (aka G())
float geometricOcclusion(PBRInfo pbrInputs) {
    float NdotL = pbrInputs.NdotL;
    float NdotV = pbrInputs.NdotV;
    float r = pbrInputs.alphaRoughness;

    float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
    float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));
    return attenuationL * attenuationV;
}

// The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())
float microfacetDistribution(PBRInfo pbrInputs) {
    float roughnessSq = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;
    float f = (pbrInputs.NdotH * roughnessSq - pbrInputs.NdotH) * pbrInputs.NdotH + 1.0;
    return roughnessSq / (M_PI * f * f);
}

vec3 RRTAndODTFit(vec3 color) {
    vec3 a = color * (color + 0.0245786) - 0.000090537;
    vec3 b = color * (0.983729 * color + 0.4329510) + 0.238081;
    return a / b;
}

void main(void) {
    if(isBlack) {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);

    } else {

    // Sample textures
        vec4 baseColor = SRGBtoLINEAR(texture(u_BaseColorSampler, v_texcoord));
        vec4 mrSample = texture(u_MetallicRoughnessSampler, v_texcoord);

    // Calculate perceptual roughness and metallic values
        float perceptualRoughness = clamp(mrSample.g * u_RoughnessFactor, c_MinRoughness, 1.0);
        float metallic = clamp(mrSample.b * u_MetallicFactor, 0.0, 1.0);
        float alphaRoughness = perceptualRoughness * perceptualRoughness;

    // Compute albedo and specular colors
        vec3 f0 = vec3(0.04);
        vec3 diffuseColor = baseColor.rgb;

        FragColor = vec4((diffuseColor * u_Exposure), baseColor.a * u_alpha);
    }
}
