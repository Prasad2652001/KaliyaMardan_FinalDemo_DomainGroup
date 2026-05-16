#version 460 core
layout(location = 0) out vec4 FragColor;
uniform sampler2D u_textureSampler;
in vec4 v_color;

void main() {
    vec4 tex = texture(u_textureSampler, gl_PointCoord);
    FragColor = v_color * tex;
}
