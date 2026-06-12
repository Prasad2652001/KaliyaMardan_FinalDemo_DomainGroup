#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D lightningTex;
uniform float u_alpha;

void main()
{
    vec4 texColor = texture(lightningTex, TexCoords);
    FragColor = vec4(texColor.rgb, texColor.a * u_alpha);
}
