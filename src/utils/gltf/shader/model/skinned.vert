#version 460 core

// Standard vertex attributes (same locations as model.vert)
layout(location = 0) in vec3 a_position;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec2 a_texcoord;
layout(location = 4) in vec3 a_tangent;

// Bone skinning attributes
layout(location = 6) in ivec4 a_boneIDs;
layout(location = 7) in vec4  a_boneWeights;

// Bone matrices (one per bone, uploaded every frame by AnimatedModel::Draw)
uniform mat4 u_BoneMatrices[100];
// Diagnostic switch: when nonzero the vertex shader skips skinning and
// renders the mesh in its raw bind pose. Lets us tell whether the mesh
// data + transforms are fine and only the skinning math is broken.
uniform int  u_DisableSkinning;

// Standard transform uniforms
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

    // Build skinning matrix from up to 4 weighted bones.
    mat4 skinMat = mat4(0.0);
    float total = 0.0;
    for (int i = 0; i < 4; ++i)
    {
        int   id = clamp(a_boneIDs[i], 0, 99);
        float w  = a_boneWeights[i];
        if (w > 0.0)
        {
            skinMat += u_BoneMatrices[id] * w;
            total   += w;
        }
    }
    if (total <= 0.0)
        skinMat = mat4(1.0); // fall back to bind pose
    else
        skinMat = skinMat * (1.0 / total); // renormalize weights -> sum to 1

    if (u_DisableSkinning != 0)
        skinMat = mat4(1.0); // diagnostic: skip skinning entirely

    // Visual diag: encode the highest-weighted bone id for this vertex into v_debug.
    // Frag shader can then false-color each bone region distinctly.
    int   topId = 0;
    float topW  = -1.0;
    for (int i = 0; i < 4; ++i) {
        if (a_boneWeights[i] > topW) { topW = a_boneWeights[i]; topId = a_boneIDs[i]; }
    }
    // Hash bone id into RGB so neighbouring ids look very different.
    float h = float(max(topId, 0));
    v_debug = fract(vec3(h * 0.1373, h * 0.2589, h * 0.4861));

    vec4 skinnedPos    = skinMat * vec4(a_position, 1.0);
    // All Mixamo bone scales are (1,1,1) here, so the upper-left 3x3 of
    // skinMat is a rotation-ish matrix. Avoid inverse(skinMat) which is
    // numerically fragile when bone matrices have very different magnitudes.
    vec3 skinnedNormal = normalize(mat3(skinMat) * a_normal);

    vec4 eyeCoordinates = u_view * u_model * skinnedPos;

    mat3 normalMatrix = transpose(inverse(mat3(u_view * u_model)));
    v_Normal          = normalize(normalMatrix * skinnedNormal);

    v_Position        = eyeCoordinates.xyz;
    v_LightDirection  = vec3(u_LightPosition - eyeCoordinates);
    v_ViewerVector    = -eyeCoordinates.xyz;

    gl_Position = u_projection * eyeCoordinates;
}
