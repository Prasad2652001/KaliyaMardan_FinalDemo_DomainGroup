#version 460 core

in vec3 inputPosition;
in vec2 inputTexCoord;
in vec3 inputNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float frameTime;
uniform vec3 scrollSpeeds;
uniform vec3 scales;

out vec2 texCoord;
out vec2 texCoords1;
out vec2 texCoords2;
out vec2 texCoords3;

void main(void)
{
	// Calculate the position of the vertex against the model, view, and projection matrices.
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(inputPosition, 1.0f);
	
    // Store the texture coordinates for the pixel shader.
    texCoord = inputTexCoord;
	
  // Compute texture coordinates for first noise texture using the first scale and upward scrolling speed values.
    texCoords1 = (inputTexCoord * scales.x);
    texCoords1.y = texCoords1.y - (frameTime * scrollSpeeds.x);
	
    // Compute texture coordinates for second noise texture using the second scale and upward scrolling speed values.
    texCoords2 = (inputTexCoord * scales.y);
    texCoords2.y = texCoords2.y - (frameTime * scrollSpeeds.y);
	
    // Compute texture coordinates for third noise texture using the third scale and upward scrolling speed values.
    texCoords3 = (inputTexCoord * scales.z);
    texCoords3.y = texCoords3.y - (frameTime * scrollSpeeds.z);

}
