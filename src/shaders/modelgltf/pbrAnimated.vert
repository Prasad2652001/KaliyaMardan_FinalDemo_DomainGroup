#version 460 core

layout(location = 0) in vec4 vPos;
layout(location = 1) in vec3 vNor;
layout(location = 2) in vec2 vTex;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBitangent;
layout(location = 5) in ivec4 vBoneIDs;
layout(location = 6) in vec4 vWeights;

uniform mat4 pMat;
uniform mat4 vMat;
uniform mat4 mMat;
uniform float clipy;

uniform mat4 finalBonesMatrices[100];

out VS_OUT
{
    vec3 P;
    vec3 N;
    vec2 Tex;
    mat3 TBN;
} vs_out;

void main(void)
{
    mat4 boneTransform =
          finalBonesMatrices[vBoneIDs[0]] * vWeights[0]
        + finalBonesMatrices[vBoneIDs[1]] * vWeights[1]
        + finalBonesMatrices[vBoneIDs[2]] * vWeights[2]
        + finalBonesMatrices[vBoneIDs[3]] * vWeights[3];

    vec4 localPos = boneTransform * vPos;
    vec3 localNor = mat3(boneTransform) * vNor;
    vec3 localTan = mat3(boneTransform) * vTangent;
    vec3 localBitan = mat3(boneTransform) * vBitangent;

    vec4 worldPos = mMat * localPos;

    gl_Position = pMat * vMat * worldPos;

    vs_out.P = vec3(worldPos);
    vs_out.N = mat3(mMat) * localNor;
    vs_out.TBN = mat3(mMat) * mat3(localTan, localBitan, localNor);
    vs_out.Tex = vTex;

    vec4 clipingPlaneReflection = vec4(0.0, 1.0, 0.0, -clipy);
    vec4 clipingPlaneRefraction = vec4(0.0, -1.0, 0.0, clipy);

    gl_ClipDistance[0] = dot(worldPos, clipingPlaneReflection);
    gl_ClipDistance[1] = dot(worldPos, clipingPlaneRefraction);
}
