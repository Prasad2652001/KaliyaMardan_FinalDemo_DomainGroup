#version 460 core

in vec4 a_pos_out;
out vec4 FragColor;

float gridLine(float coord)
{
    float g = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);
    return 1.0 - clamp(g, 0.0, 1.0);
}

void main(void)
{
    float minorScale = 1.0;   // 1 unit grid
    float majorScale = 0.1;   // every 10 units (1/10)

    // grid
    float minorX = gridLine(a_pos_out.x * minorScale);
    float minorZ = gridLine(a_pos_out.z * minorScale);
    float minorGrid = max(minorX, minorZ);

    float majorX = gridLine(a_pos_out.x * majorScale);
    float majorZ = gridLine(a_pos_out.z * majorScale);
    float majorGrid = max(majorX, majorZ);

    // colors
    vec3 color = vec3(0.08);                 // background
    color = mix(color, vec3(0.25), minorGrid); // minor lines
    color = mix(color, vec3(0.8), majorGrid);  // major lines (10,20,30)

    // axes
    if (abs(a_pos_out.z) < 0.02)
        color = vec3(1.0, 0.0, 0.0); // X axis

    if (abs(a_pos_out.x) < 0.02)
        color = vec3(0.0, 0.0, 1.0); // Z axis

    FragColor = vec4(color, 1.0);
}