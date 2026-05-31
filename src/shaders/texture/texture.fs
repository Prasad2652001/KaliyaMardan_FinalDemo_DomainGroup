#version 460 core

in vec2 a_texcoords_out;
in vec3 v_Normal;
in vec3 v_LightDirection;
in vec3 v_ViewerVector;

uniform sampler2D u_textureSampler;
uniform float u_alphaVal = 1.0;
uniform float iTime;
uniform int bShine = 0;

uniform float u_MaterialShininess;
uniform bool u_ApplyToon;
uniform bool u_ApplySpecular;

out vec4 FragColor;

void main(void)
{
    vec4 frag = texture(u_textureSampler, a_texcoords_out);
    frag.a = frag.a * u_alphaVal;

    if(u_ApplyToon == true)
    {
        vec3 N = normalize(v_Normal);
        vec3 L = normalize(v_LightDirection);
        vec3 V = normalize(v_ViewerVector);

        vec3 posterizedTexture = floor(frag.rgb * 4.0) / 4.0;

        float diff = max(dot(N, L), 0.0);
        float levels = 4.0;
        diff = floor(diff * levels) / levels;

        float ambient = 0.35;
        vec3 toonDiffuse = posterizedTexture * max(diff, ambient);

        vec3 toonSpecular = vec3(0.0);
        
        if(u_ApplySpecular == true)
        {
            vec3 R = reflect(-L, N);
            float spec = pow(max(dot(R, V), 0.0), u_MaterialShininess);
            spec = spec > 0.55 ? 0.35 : 0.0;
            toonSpecular = vec3(spec);
        }

        frag.rgb = toonDiffuse + toonSpecular;
    }

    if(bShine == 1)
    {
        frag = mix(frag, vec4(1.0), step(0.999, sin(a_texcoords_out.x - a_texcoords_out.y - iTime * 3.0)));
        frag = mix(frag, vec4(1.0), step(0.970, sin(a_texcoords_out.x - a_texcoords_out.y - (2.4 + iTime) * 3.0)));
    }

    FragColor = frag;
}
