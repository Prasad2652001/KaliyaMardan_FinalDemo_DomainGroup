#version 460 core

in vec2 fragmentTexCoords;
in vec3 particlePosition;

// for light
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

vec3 lightFunc(void)
{
    vec3 ambient = u_la * u_ka;

    vec3 normalized_transformed_normals = normalize(transformedNormals);
    vec3 normalized_light_direction = normalize(lightDirection);
    vec3 normalized_viewer_vector = normalize(viewerVector);

    vec3 diffuse = u_ld * u_kd * max(dot(normalized_light_direction, normalized_transformed_normals), 0.0);

    vec3 reflectionVector = reflect(-normalized_light_direction, normalized_transformed_normals);
    vec3 specular = u_ls * u_ks * pow(max(dot(reflectionVector, normalized_viewer_vector), 0.0), u_materialShiness);

    return ambient + diffuse + specular;
}

void main(void)
{
    vec4 texColor = texture(bubbleTex, fragmentTexCoords);

    float distFromCenter = distance(fragmentTexCoords, vec2(0.5, 0.5));

    float outerFade = 1.0 - smoothstep(0.34, 0.50, distFromCenter);
    float innerFade = 1.0 - smoothstep(0.02, 0.20, distFromCenter);
    float rim = clamp(outerFade - innerFade, 0.0, 1.0);

    float softFill = 1.0 - smoothstep(0.00, 0.50, distFromCenter);
    float highlightSpot = pow(max(0.0, 1.0 - distance(fragmentTexCoords, vec2(0.32, 0.70)) * 2.4), 4.0);

    float textureAlpha = max(texColor.a, texColor.r);
    float alphaMask = max(textureAlpha, rim * 0.85 + softFill * 0.12);

    if (alphaMask < 0.02)
        discard;

    vec3 phongLight = lightFunc();
    vec3 baseBubbleColor = vec3(0.72, 0.88, 1.0);
    vec3 rimColor = vec3(0.90, 0.97, 1.00) * rim;
    vec3 highlightColor = vec3(1.0) * highlightSpot * (0.25 + 0.25 * bubbleSeedMask);

    vec3 finalColor = (baseBubbleColor * phongLight) + rimColor + highlightColor;

    float finalAlpha = alphaMask * particleAlpha * u_alpha * 0.85;
    FragColor = vec4(finalColor, finalAlpha);
}