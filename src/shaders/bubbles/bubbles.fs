#version 460 core

in vec2 fragmentTexCoords;
in vec3 particlePosition;

// kept from current pipeline
in vec3 transformedNormals;
in vec3 lightDirection;
in vec3 viewerVector;

in float particleAlpha;
in float bubbleSeedMask;

uniform sampler2D bubbleTex;
uniform vec3 eyePosition;

// for Phong light
uniform vec3 u_la;
uniform vec3 u_ld;
uniform vec3 u_ls;
uniform vec3 u_ka;
uniform vec3 u_kd;
uniform vec3 u_ks;
uniform float u_materialShiness;
uniform float u_alpha = 1.0;

out vec4 FragColor;

void main(void)
{
    // Convert billboard UV to sphere local coordinates (-1 to 1)
    vec2 p = fragmentTexCoords * 2.0 - 1.0;
    float r2 = dot(p, p);

    // outside sphere silhouette
    if (r2 > 1.0)
        discard;

    // Reconstruct sphere normal in view-facing impostor space
    float z = sqrt(1.0 - r2);
    vec3 sphereNormal = normalize(vec3(p.x, p.y, z));

    vec3 L = normalize(lightDirection);
    vec3 V = normalize(viewerVector);
    vec3 H = normalize(L + V);

    float NdotL = max(dot(sphereNormal, L), 0.0);
    float NdotH = max(dot(sphereNormal, H), 0.0);
    float NdotV = max(dot(sphereNormal, V), 0.0);

    // Bubble look: faint body + bright rim + small highlight
    float fresnel = pow(1.0 - NdotV, 3.5);
    float specular = pow(NdotH, max(u_materialShiness, 24.0));
    float rim = pow(1.0 - abs(sphereNormal.z), 2.2);

    // Slight internal refraction-style tint
    vec3 baseColor = vec3(0.62, 0.82, 1.00);
    vec3 ambient = u_la * (u_ka * 0.15);
    vec3 diffuse = u_ld * baseColor * (0.10 + 0.18 * NdotL);
    vec3 spec = u_ls * vec3(1.0) * specular * 0.85;
    vec3 rimColor = vec3(0.85, 0.96, 1.0) * (0.45 * rim + 0.95 * fresnel);

    // Optional subtle texture modulation so existing texture binding stays useful
    vec4 texColor = texture(bubbleTex, fragmentTexCoords);
    float texMod = mix(0.92, 1.08, texColor.r);

    // Small moving/internal highlight variation from seed
    float seedGlow = 0.85 + 0.25 * bubbleSeedMask;

    vec3 finalColor = (ambient + diffuse) * texMod + spec + rimColor * seedGlow;

    // Bubble alpha: stronger at rim, softer in center
    float centerSoftness = 1.0 - smoothstep(0.0, 0.85, sqrt(r2));
    float alphaBody = 0.05 * centerSoftness;
    float alphaRim = 0.42 * rim + 0.55 * fresnel;
    float finalAlpha = (alphaBody + alphaRim) * particleAlpha * u_alpha;

    // Avoid fully opaque look
    finalAlpha = clamp(finalAlpha, 0.0, 0.82);

    FragColor = vec4(finalColor, finalAlpha);
}