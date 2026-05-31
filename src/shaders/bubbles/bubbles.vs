#version 460 core

in vec4 positionMC;
in vec4 vertexSeed;
in vec4 vertexVelo;

out vec4 seedOut;
out vec4 velocityOut;

void main(void)
{
    seedOut = vertexSeed;
    velocityOut = vertexVelo;
    gl_Position = positionMC;
}