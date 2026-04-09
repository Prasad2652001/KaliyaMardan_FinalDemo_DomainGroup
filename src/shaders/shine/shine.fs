#version 460 core 

in vec2 a_texcoord_out;
uniform float iTime;

out vec4 FragColor; 
float easeInOutQuad(float t) {
    return t < 0.5 ? 2.0 * t * t : -1.0 + (4.0 - 2.0 * t) * t;
}

float easeInCubic(float t) {
    return t * t * t;
}

float easeOutCubic(float t) {
    float f = t - 1.0;
    return f * f * f + 1.0;
}

float easeInOutCubic(float t) {
    return t < 0.5 ? 4.0 * t * t * t : (t - 1.0) * (2.0 * t - 2.0) * (2.0 * t - 2.0) + 1.0;
}

float easeInQuart(float t) {
    return t * t * t * t;
}

float easeOutQuart(float t) {
    float f = t - 1.0;
    return 1.0 - f * f * f * f;
}

float easeInOutQuart(float t) {
    return t < 0.5 ? 8.0 * t * t * t * t : 1.0 - 8.0 * (t - 1.0) * (t - 1.0) * (t - 1.0) * (t - 1.0);
}

float easeInSine(float t) {
    return 1.0 - cos((t * 3.141592653589793) / 2.0);
}

float easeOutSine(float t) {
    return sin((t * 3.141592653589793) / 2.0);
}

float easeInOutSine(float t) {
    return -0.5 * (cos(3.141592653589793 * t) - 1.0);
}

float easeInExpo(float t) {
    return t == 0.0 ? 0.0 : pow(2.0, 10.0 * (t - 1.0));
}

float easeOutExpo(float t) {
    return t == 1.0 ? 1.0 : 1.0 - pow(2.0, -10.0 * t);
}

float easeInOutExpo(float t) {
    if (t == 0.0) return 0.0;
    if (t == 1.0) return 1.0;
    return t < 0.5 ? 0.5 * pow(2.0, 20.0 * t - 10.0) : 1.0 - 0.5 * pow(2.0, -20.0 * t + 10.0);
}

float easeInBack(float t) {
    const float c1 = 1.70158;
    return (c1 + 1.0) * t * t * t - c1 * t * t;
}

float easeOutBack(float t) {
    const float c1 = 1.70158;
    float f = t - 1.0;
    return 1.0 + (c1 + 1.0) * f * f * f + c1 * f * f;
}

float easeInOutBack(float t) {
    const float c1 = 1.70158;
    const float c2 = c1 * 1.525;
    return t < 0.5
        ? (pow(2.0 * t, 2.0) * ((c2 + 1.0) * 2.0 * t - c2)) / 2.0
        : (pow(2.0 * t - 2.0, 2.0) * ((c2 + 1.0) * (t * 2.0 - 2.0) + c2) + 2.0) / 2.0;
}

float ZeroStartEnd(float x)
{
    return pow(x*2.0 -1.0,8.0) * -1.0  + 1.0;
}

float remap(float value, float oldMin, float oldMax, float newMin, float newMax) {
    return newMin + (value - oldMin) * (newMax - newMin) / (oldMax - oldMin);
}

vec3 offsetHue(vec3 color, float hueOffset) {
    float maxC = max(max(color.r, color.g), color.b);
    float minC = min(min(color.r, color.g), color.b);
    float delta = maxC - minC;

    float hue = 0.0;
    if (delta > 0.0) {
        if (maxC == color.r) {
            hue = mod((color.g - color.b) / delta, 6.0);
        } else if (maxC == color.g) {
            hue = (color.b - color.r) / delta + 2.0;
        } else {
            hue = (color.r - color.g) / delta + 4.0;
        }
    }
    hue /= 6.0;

    float saturation = (maxC > 0.0) ? (delta / maxC) : 0.0;
    float value = maxC;

    hue = mod(hue + hueOffset, 1.0);

    float c = value * saturation;
    float x = c * (1.0 - abs(mod(hue * 6.0, 2.0) - 1.0));
    float m = value - c;

    vec3 rgb;
    if (hue < 1.0 / 6.0) {
        rgb = vec3(c, x, 0.0);
    } else if (hue < 2.0 / 6.0) {
        rgb = vec3(x, c, 0.0);
    } else if (hue < 3.0 / 6.0) {
        rgb = vec3(0.0, c, x);
    } else if (hue < 4.0 / 6.0) {
        rgb = vec3(0.0, x, c);
    } else if (hue < 5.0 / 6.0) {
        rgb = vec3(x, 0.0, c);
    } else {
        rgb = vec3(c, 0.0, x);
    }
    return rgb + m;
}

vec2 rotateUV(vec2 uv, float rotation)
{
    float mid = 0.5;
    return vec2(
        cos(rotation) * (uv.x - mid) + sin(rotation) * (uv.y - mid) + mid,
        cos(rotation) * (uv.y - mid) - sin(rotation) * (uv.x - mid) + mid
    );
}

vec2 rotateUV(vec2 uv, float rotation, vec2 mid)
{
    return vec2(
        cos(rotation) * (uv.x - mid.x) + sin(rotation) * (uv.y - mid.y) + mid.x,
        cos(rotation) * (uv.y - mid.y) - sin(rotation) * (uv.x - mid.x) + mid.y
    );
}

vec2 rotateUV(vec2 uv, float rotation, float mid)
{
    return vec2(
        cos(rotation) * (uv.x - mid) + sin(rotation) * (uv.y - mid) + mid,
        cos(rotation) * (uv.y - mid) - sin(rotation) * (uv.x - mid) + mid
    );
}

void main(void) 
{ 
    float T_MAX = 5.0;
    float time = mod(iTime, T_MAX);
    float t = time/ T_MAX;
    t = mix(0.1,0.0,t);
    
    float progress = t;
    
    vec2 iResolution = vec2(1920.0,1080.0);
    float aspect = iResolution.x/iResolution.y;
    
    vec2 uv = a_texcoord_out;
    
    vec2 tuv = uv;
    tuv -= 0.5;
    tuv += 0.5;
    
    uv = tuv;
    uv = rotateUV(uv,6.28*t*1.0);
    
    float p = mix(-1.0,1000.0,easeInExpo(progress));
    
    float m = mix(
    0.0,
    1.0,
    clamp(
        pow(abs(uv.x-0.5)*2.0,p) + pow(abs(uv.y-0.5)*2.0,p),0.0,1.0
    )
    );
    
    float mask = 1.0 - (m - progress);
    mask = easeInSine(mask);
    
    FragColor = vec4(1.0 * mask);
}
	