#version 460 core

in vec2 v_uv;
out vec4 FragColor;

uniform sampler2D u_scene;   // rendered scene color
uniform float u_time;        // seconds
uniform float u_strength;    // 0..1 overall underwater intensity

// --- small hash / noise helpers -------------------------------------------
float hash21(vec2 p)
{
    p = fract(p * vec2(123.34, 345.45));
    p += dot(p, p + 34.345);
    return fract(p.x * p.y);
}

float noise(vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    float a = hash21(i);
    float b = hash21(i + vec2(1.0, 0.0));
    float c = hash21(i + vec2(0.0, 1.0));
    float d = hash21(i + vec2(1.0, 1.0));
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

// Animated caustics: layered sine ripples that look like light bands.
float caustics(vec2 uv, float t)
{
    vec2 p = uv * 6.0;
    float v = 0.0;
    v += sin(p.x * 1.7 + t * 1.3) * sin(p.y * 1.9 - t * 1.1);
    v += sin((p.x + p.y) * 1.3 - t * 0.9);
    v += sin(length(p - vec2(3.0, 2.0)) * 2.2 - t * 1.7);
    v = v / 3.0;
    v = pow(clamp(v * 0.5 + 0.5, 0.0, 1.0), 3.0); // sharpen into bands
    return v;
}

void main(void)
{
    vec2 uv = v_uv;
    float t = u_time;

    // --- wavy refraction: gently distort the sampling coordinates ----------
    vec2 wave;
    wave.x = sin(uv.y * 18.0 + t * 1.5) + sin(uv.y * 7.0 - t * 0.8);
    wave.y = cos(uv.x * 16.0 - t * 1.2) + cos(uv.x * 6.0 + t * 0.6);
    vec2 distUv = uv + wave * 0.0035 * u_strength;

    vec3 color = texture(u_scene, distUv).rgb;

    // --- underwater color grade: kill red, lift blue/green ----------------
    vec3 deep = vec3(0.03, 0.16, 0.22);   // deep water color
    vec3 tint = vec3(0.45, 0.85, 0.95);   // water transmission tint
    color *= tint;

    // depth fog toward the bottom of the screen (further/deeper = murkier)
    float depthFog = smoothstep(0.65, -0.15, uv.y);
    color = mix(color, deep, depthFog * 0.55 * u_strength);

    // overall blue cast
    color = mix(color, color * vec3(0.6, 0.95, 1.1) + deep * 0.25, 0.5 * u_strength);

    // --- caustics: brighter near the top (closer to the surface) ----------
    float caus = caustics(uv + wave * 0.01, t);
    float surfaceMask = smoothstep(0.1, 1.0, uv.y);
    color += caus * surfaceMask * vec3(0.18, 0.28, 0.30) * u_strength;

    // --- god rays: soft vertical light shafts from the surface -------------
    float rays = 0.0;
    for (int i = 0; i < 3; ++i)
    {
        float fi = float(i);
        float x = uv.x + 0.12 * sin(t * 0.25 + fi * 2.1);
        float band = abs(fract(x * (2.0 + fi) - t * 0.03) - 0.5);
        rays += smoothstep(0.45, 0.0, band);
    }
    rays *= surfaceMask * 0.06 * u_strength;
    color += rays * vec3(0.5, 0.85, 0.95);

    // --- floating particles / marine snow ---------------------------------
    vec2 pp = uv * vec2(40.0, 30.0);
    pp.y += t * 0.6;
    float particles = step(0.985, noise(pp));
    color += particles * 0.25 * u_strength;

    // --- vignette ---------------------------------------------------------
    vec2 q = uv - 0.5;
    q.x *= 1.35;
    float vig = 1.0 - smoothstep(0.35, 0.85, length(q));
    color *= mix(1.0, vig, 0.6 * u_strength);

    FragColor = vec4(clamp(color, 0.0, 1.0), 1.0);
}
