#version 460 core

in vec2 a_texcoords_out;

uniform sampler2D u_textureSampler;
uniform float u_alphaVal = 1.0;
uniform float iTime;
uniform int bShine = 0;
out vec4 FragColor;

void main(void)
{
    vec4 frag = texture(u_textureSampler, a_texcoords_out);
    frag = vec4( frag.xyz, frag.a * u_alphaVal);

    if(bShine == 1)
    {
        frag = mix(frag, vec4(1.), step(.999, (sin(a_texcoords_out.x-a_texcoords_out.y-iTime*3.))));
        frag = mix(frag, vec4(1.), step(.97, (sin(a_texcoords_out.x-a_texcoords_out.y-(2,4.+iTime)*3.))));
    }   

    FragColor = frag;
}
