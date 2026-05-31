#version 460 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec4 seedOut[];
in vec4 velocityOut[];

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
uniform vec3 eyePosition;
uniform float dt;

// for light
uniform vec4 u_lightPosition;

out vec2 fragmentTexCoords;
out vec3 particlePosition;
out vec3 transformedNormals;
out vec3 lightDirection;
out vec3 viewerVector;
out float particleAlpha;
out float bubbleSeedMask;

vec3 getAnimatedBubbleCenter(void)
{
    vec3 basePos = gl_in[0].gl_Position.xyz;
    vec3 velocity = velocityOut[0].xyz;

    float phase1 = seedOut[0].x * 6.2831853 + dt * (1.5 + seedOut[0].y * 2.0);
    float phase2 = seedOut[0].z * 6.2831853 + dt * (1.0 + seedOut[0].x * 1.7);

    vec3 wobble = vec3(
        sin(phase1) * 0.03 * max(seedOut[0].w, 0.2),
        0.0,
        cos(phase2) * 0.03 * max(seedOut[0].w, 0.2)
    );

    return basePos + velocity * dt + wobble;
}

void emitBubbleVertex(vec3 cornerPos, vec2 uv, vec3 centerPos, vec3 normalDir, vec4 eyeCoordinates, mat3 normalMatrix)
{
    fragmentTexCoords = uv;
    particlePosition = centerPos;
    transformedNormals = normalize(normalMatrix * normalDir);
    lightDirection = vec3(u_lightPosition) - eyeCoordinates.xyz;
    viewerVector = -eyeCoordinates.xyz;
    particleAlpha = clamp(velocityOut[0].w, 0.0, 1.0);
    bubbleSeedMask = seedOut[0].z;

    gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(cornerPos, 1.0);
    EmitVertex();
}

void main(void)
{
    vec3 center = getAnimatedBubbleCenter();

    float bubbleSize = clamp(seedOut[0].w, 0.15, 4.0) * 0.055;
    float distanceScale = clamp(length(eyePosition - center) * 0.04, 0.55, 2.0);
    bubbleSize *= distanceScale;
    //float bubbleSize = clamp(seedOut[0].w, 0.15, 4.0) * 220.0;
    //float distanceScale = clamp(length(eyePosition - center) * 0.0025, 0.8, 8.0);
    //bubbleSize *= distanceScale;

    vec3 toCamera = normalize(eyePosition - center);
    vec3 upReference = (abs(dot(toCamera, vec3(0.0, 1.0, 0.0))) > 0.95) ? vec3(1.0, 0.0, 0.0) : vec3(0.0, 1.0, 0.0);

    vec3 right = normalize(cross(upReference, toCamera)) * bubbleSize;
    vec3 up = normalize(cross(toCamera, right)) * bubbleSize;
    vec3 normal = toCamera;

    vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * vec4(center, 1.0);
    mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);

    emitBubbleVertex(center - right - up, vec2(0.0, 0.0), center, normal, eyeCoordinates, normalMatrix);
    emitBubbleVertex(center - right + up, vec2(0.0, 1.0), center, normal, eyeCoordinates, normalMatrix);
    emitBubbleVertex(center + right - up, vec2(1.0, 0.0), center, normal, eyeCoordinates, normalMatrix);
    emitBubbleVertex(center + right + up, vec2(1.0, 1.0), center, normal, eyeCoordinates, normalMatrix);

    EndPrimitive();
}