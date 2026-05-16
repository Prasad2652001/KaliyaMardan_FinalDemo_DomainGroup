#version 460 core
in vec2 a_texcoords_out;

out vec4 FragColor;

uniform float u_time;
uniform float u_alpha;

float random(in vec2 _st) {
    return fract(sin(dot(_st.xy, vec2(12.9898, 78.233))) *
        43758.5453123);
}

float noise(in vec2 _st) {
    vec2 i = floor(_st);
    vec2 f = fract(_st);

    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
           (c - a) * u.y * (1.0 - u.x) +
           (d - b) * u.x * u.y;
}

#define NUM_OCTAVES 5

float fbm(in vec2 _st) {
    float v = 0.0;
    float a = 0.5;
    vec2 shift = vec2(100.0);
    mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.5));
    for(int i = 0; i < NUM_OCTAVES; ++i) {
        v += a * noise(_st);
        _st = rot * _st * 2.0 + shift;
        a *= 0.5;
    }
    return v;
}

// Nebula color palette function
vec3 nebulaColor(float v) {
    if (v < 0.4)
        return mix(vec3(0.0, 1.0, 1.0), vec3(0.5, 0.0, 1.0), v / 0.4); // cyan → purple over 0.0 to 0.4
    else if (v < 0.7)
        return mix(vec3(0.5, 0.0, 1.0), vec3(0.8, 0.0, 0.8), (v - 0.4) / 0.3); // purple → lighter purple 0.4 to 0.7
    else if (v < 0.9)
        return mix(vec3(0.8, 0.0, 0.8), vec3(0.0, 1.0, 1.0), (v - 0.7) / 0.2); // purple → cyan again 0.7 to 0.9
    else
        return mix(vec3(0.0, 1.0, 1.0), vec3(0.5, 0.0, 1.0), (v - 0.9) / 0.1); // cyan → purple at the end 0.9 to 1.0
}


void main() {
    vec2 st = gl_FragCoord.xy / vec2(1920.0 , 1080.0) * 3.0;

    vec2 q;
    q.x = fbm(st);
    q.y = fbm(st + vec2(1.0));

    vec2 r;
    r.x = fbm(st + 1.0 * q + vec2(1.7, 9.2) + 0.15 * u_time);
    r.y = fbm(st + 1.0 * q + vec2(8.3, 2.8) + 0.126 * u_time);

    float f = fbm(st + r);

    vec3 color = nebulaColor(f);

    // Apply brightness shaping
    float brightness = (f * f * f + 0.6 * f * f + 0.5 * f);
    FragColor = vec4(brightness * color, u_alpha);
}
