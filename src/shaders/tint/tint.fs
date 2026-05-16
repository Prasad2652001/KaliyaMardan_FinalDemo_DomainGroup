#version 460 core

in vec2 out_texCoord;

uniform sampler2D uTextureSampler;
uniform int u_TintMode;        // 0=None, 1=Vintage Sepia, 2=Grey/BW, 3=Pastel Happy, 4=Warm Film
uniform float u_VignettePower; // 1.0 to 3.0
uniform float u_TintStrength;  // 0.0 to 1.0

out vec4 FragColor;

float rand(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453);
}

void main(void)
{
    vec4 texcolor = texture(uTextureSampler, out_texCoord);
    vec3 color = texcolor.rgb;

    if (u_TintMode == 0)
    {
        FragColor = texcolor;
        return;
    }

    vec2 uv = out_texCoord;

    // aspect rounddd
    vec2 p = uv - vec2(0.5);
    p.x *= 1.35;

    float d = length(p);

    // center visivle cornere dark
    float vignette = 1.0 - smoothstep(0.38, 0.78, d);
    vignette = pow(vignette, u_VignettePower);

    // more darkness athi
    float cornerMask = smoothstep(0.42, 0.78, d);
    vec3 cornerColor = vec3(0.05, 0.035, 0.02);

    vec3 tinted = color;

    if (u_TintMode == 1)
    {
        // traditional brownish yellow vintage wibe
        vec3 sepia;
        sepia.r = dot(color, vec3(0.393, 0.769, 0.189));
        sepia.g = dot(color, vec3(0.349, 0.686, 0.168));
        sepia.b = dot(color, vec3(0.272, 0.534, 0.131));

        sepia = mix(sepia, vec3(1.0, 0.82, 0.48), 0.18);
        tinted = mix(color, sepia, u_TintStrength);
    }
    else if (u_TintMode == 2)
    {
        // grey / black-white old filmyy vibes 
        float grey = dot(color, vec3(0.299, 0.587, 0.114));
        tinted = mix(color, vec3(grey), u_TintStrength);
    }
    else if (u_TintMode == 3)
    {
        // pastel happy look happyyy 
        vec3 pastel = mix(color, vec3(1.0, 0.78, 0.58), 0.22);
        pastel = mix(pastel, vec3(0.75, 0.88, 1.0), uv.y * 0.18);
        tinted = mix(color, pastel, u_TintStrength);
    }
    else if (u_TintMode == 4)
    {
        // warm old film tp
        vec3 warm = color * vec3(1.12, 0.92, 0.70);
        tinted = mix(color, warm, u_TintStrength);
    }

    tinted *= mix(0.55, 1.08, vignette);
    tinted = mix(tinted, cornerColor, cornerMask * 0.72);

    // hyane old time movie types dista  
    // float grain = rand(uv * 900.0) * 0.055;
    // tinted += grain;
    
    // hyane plain as previously made 
    float grain = rand(uv * 1200.0) * 0.015;
    tinted += grain;

    FragColor = vec4(clamp(tinted, 0.0, 1.0), texcolor.a);
}
