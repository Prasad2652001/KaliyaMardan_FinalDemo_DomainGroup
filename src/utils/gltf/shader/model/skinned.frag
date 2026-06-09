#version 460 core

in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_texcoord;
in vec3 v_LightDirection;
in vec3 v_ViewerVector;
in vec3 v_debug;

uniform sampler2D u_BaseColorSampler;
uniform bool      isBlack;
// 0 = normal shading
// 1 = false-color by dominant bone id (uses v_debug)
uniform int       u_DebugMode;
// Overall opacity for fade in / fade out cross-dissolves (1.0 = fully opaque).
uniform float     u_Alpha;

out vec4 FragColor;

void main(void)
{
    if (u_DebugMode == 1) {
        FragColor = vec4(v_debug, 1.0);
        return;
    }

    vec3 N = normalize(v_Normal);
    vec3 L = normalize(v_LightDirection);
    vec3 V = normalize(v_ViewerVector);

    vec3 base = texture(u_BaseColorSampler, v_texcoord).rgb;
    if (dot(base, vec3(1.0)) < 0.01)
        base = vec3(0.35, 0.55, 0.85);  // Krishna blue fallback

    float diff = max(dot(N, L), 0.0);
    vec3  H    = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), 32.0);

    vec3 color = base * (0.2 + 0.8 * diff) + vec3(0.3) * spec;
    if (isBlack) color = vec3(0.0);

    FragColor = vec4(color, u_Alpha);
}
