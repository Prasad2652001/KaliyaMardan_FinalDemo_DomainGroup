#version 460 core

layout(location = 0) in vec3 a_position;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec2 a_texcoord;
layout(location = 4) in vec3 a_tangent;

layout(location = 6) in ivec4 a_boneIDs;
layout(location = 7) in vec4  a_boneWeights;

uniform mat4 u_BoneMatrices[100];

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform vec4 u_LightPosition;

out vec3 v_Position;
out vec3 v_Normal;
out vec2 v_texcoord;
out vec3 v_LightDirection;
out vec3 v_ViewerVector;
out vec3 v_debug;

void main(void)
{
    v_texcoord = a_texcoord;

    vec4 skinnedPos    = vec4(0.0);
    vec3 skinnedNormal = vec3(0.0);
    float totalW       = 0.0;

    for (int i = 0; i < 4; ++i)
    {
        float w = a_boneWeights[i];
        if (w <= 0.0)
            continue;

        int id = clamp(a_boneIDs[i], 0, 99);
        mat4 M = u_BoneMatrices[id];

        skinnedPos    += w * (M * vec4(a_position, 1.0));
        skinnedNormal += w * (mat3(M) * a_normal);
        totalW        += w;
    }

    if (totalW <= 0.0)
    {
        skinnedPos    = vec4(a_position, 1.0);
        skinnedNormal = a_normal;
    }
    else if (abs(totalW - 1.0) > 0.001)
    {
        skinnedPos    /= totalW;
        skinnedNormal /= totalW;
    }

    vec4 eyeCoordinates = u_view * u_model * skinnedPos;

    mat3 normalMatrix = transpose(inverse(mat3(u_view * u_model)));
    v_Normal          = normalize(normalMatrix * normalize(skinnedNormal));

    v_Position       = eyeCoordinates.xyz;
    v_LightDirection = vec3(u_LightPosition - eyeCoordinates);
    v_ViewerVector   = -eyeCoordinates.xyz;

    // bone-id debug: colour by the dominant bone index (debug mode 1)
    int dominantBone = 0;
    float maxW = 0.0;
    for (int i = 0; i < 4; ++i)
        if (a_boneWeights[i] > maxW) { maxW = a_boneWeights[i]; dominantBone = a_boneIDs[i]; }
    v_debug = vec3(float(dominantBone % 8) / 7.0,
                   float((dominantBone / 8) % 8) / 7.0,
                   float(dominantBone / 64) / 1.0);

    gl_Position = u_projection * eyeCoordinates;
}
