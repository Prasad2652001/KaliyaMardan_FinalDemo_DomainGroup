#include "AnimatedModel.h"
#include "TextureModel.h"
#include <assimp/postprocess.h>
#include <assimp/config.h>
#include "../OpenGL/GLLog.h"
#include "../../includes/stb_image.h"
#include <glm/gtc/quaternion.hpp>
#include <cmath>
#include <map>

static glm::mat4 AiMat4ToGlm(const aiMatrix4x4& m)
{
    return glm::mat4(m.a1, m.b1, m.c1, m.d1,
                     m.a2, m.b2, m.c2, m.d2,
                     m.a3, m.b3, m.c3, m.d3,
                     m.a4, m.b4, m.c4, m.d4);
}

// Catmull-Rom spline through p1->p2 using neighbours p0,p3 as tangents.
// Gives C1-continuous (smooth velocity) interpolation, so a looping pose
// sequence flows continuously instead of stopping at every keyframe.
static glm::vec3 CatmullRom(const glm::vec3& p0, const glm::vec3& p1,
                            const glm::vec3& p2, const glm::vec3& p3, float t)
{
    const float t2 = t * t;
    const float t3 = t2 * t;
    return 0.5f * ((2.0f * p1) +
                   (-p0 + p2) * t +
                   (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
                   (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
}

namespace Core
{
    SkinnedMesh::SkinnedMesh(std::vector<SkinnedVertex> v,
                             std::vector<unsigned int>  i,
                             std::vector<ModelTexture>  t)
        : vertices(std::move(v)), indices(std::move(i)), textures(std::move(t))
    {
        SetupMesh();
    }

    SkinnedMesh::~SkinnedMesh()
    {
        if (vao) { glDeleteVertexArrays(1, &vao); vao = 0; }
        if (vbo) { glDeleteBuffers(1, &vbo);       vbo = 0; }
        if (ebo) { glDeleteBuffers(1, &ebo);       ebo = 0; }
    }

    SkinnedMesh::SkinnedMesh(SkinnedMesh&& o) noexcept
        : vertices(std::move(o.vertices)),
          indices(std::move(o.indices)),
          textures(std::move(o.textures)),
          vao(o.vao), vbo(o.vbo), ebo(o.ebo)
    {
        o.vao = 0; o.vbo = 0; o.ebo = 0;
    }

    SkinnedMesh& SkinnedMesh::operator=(SkinnedMesh&& o) noexcept
    {
        if (this != &o)
        {
            if (vao) glDeleteVertexArrays(1, &vao);
            if (vbo) glDeleteBuffers(1, &vbo);
            if (ebo) glDeleteBuffers(1, &ebo);
            vertices = std::move(o.vertices);
            indices  = std::move(o.indices);
            textures = std::move(o.textures);
            vao = o.vao; vbo = o.vbo; ebo = o.ebo;
            o.vao = 0;   o.vbo = 0;   o.ebo = 0;
        }
        return *this;
    }

    void SkinnedMesh::SetupMesh()
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     vertices.size() * sizeof(SkinnedVertex),
                     vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indices.size() * sizeof(unsigned int),
                     indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                              sizeof(SkinnedVertex),
                              (void*)offsetof(SkinnedVertex, position));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
                              sizeof(SkinnedVertex),
                              (void*)offsetof(SkinnedVertex, normal));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE,
                              sizeof(SkinnedVertex),
                              (void*)offsetof(SkinnedVertex, texcoords));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE,
                              sizeof(SkinnedVertex),
                              (void*)offsetof(SkinnedVertex, tangent));
        glEnableVertexAttribArray(6);
        glVertexAttribIPointer(6, 4, GL_INT,
                               sizeof(SkinnedVertex),
                               (void*)offsetof(SkinnedVertex, boneIDs));
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE,
                              sizeof(SkinnedVertex),
                              (void*)offsetof(SkinnedVertex, boneWeights));

        glBindVertexArray(0);
    }

    void SkinnedMesh::DrawMesh(std::unique_ptr<Shader>& shader)
    {
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);

            std::string uniformName;
            switch (textures[i].type)
            {
            case TextureType::DiffuseMap:           uniformName = "u_BaseColorSampler";         break;
            case TextureType::NormalMap:            uniformName = "u_NormalSampler";            break;
            case TextureType::MetallicRoughnessMap: uniformName = "u_MetallicRoughnessSampler"; break;
            case TextureType::EmissiveMap:          uniformName = "u_EmissiveSampler";          break;
            case TextureType::AmbientMap:           uniformName = "u_AmbientSampler";           break;
            default: break;
            }

            if (!uniformName.empty())
                glUniform1i(glGetUniformLocation(shader->GetShaderProgram(), uniformName.c_str()),
                            static_cast<GLint>(i));

            shader->SetUniform("u_MetallicFactor",    textures[i].pbrMaterial.metallic);
            shader->SetUniform("u_RoughnessFactor",   textures[i].pbrMaterial.roughness);
            shader->SetUniform("u_Exposure",          shader->exposure);
            shader->SetUniform("u_MipCount",          10);
            shader->SetUniform("u_EnvIntensity",      0.5f);
            shader->SetUniform("u_MaterialShininess", 64.0f);
            shader->SetUniform("u_LD",                vmath::vec3(1.0f, 1.0f, 1.0f));
            shader->SetUniform("u_LS",                vmath::vec3(1.0f, 1.0f, 1.0f));
        }

        if (textures.empty())
        {
            shader->SetUniform("u_Exposure", shader->exposure);
            shader->SetUniform("u_MetallicFactor",  0.0f);
            shader->SetUniform("u_RoughnessFactor", 1.0f);
        }

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }

    AnimatedModel::AnimatedModel()
    {
        mBoneMatrices.assign(ANIM_MAX_BONES, glm::mat4(1.0f));
    }

    AnimatedModel::~AnimatedModel() {}

    const aiNodeAnim* AnimatedModel::FindChannel(const aiAnimation* anim, const std::string& name)
    {
        for (unsigned int i = 0; i < anim->mNumChannels; i++)
            if (name == anim->mChannels[i]->mNodeName.C_Str())
                return anim->mChannels[i];
        return nullptr;
    }

    aiVector3D AnimatedModel::InterpolatePos(const aiNodeAnim* ch, double tick)
    {
        if (ch->mNumPositionKeys == 0) return aiVector3D(0, 0, 0);
        if (ch->mNumPositionKeys == 1) return ch->mPositionKeys[0].mValue;
        unsigned int i = ch->mNumPositionKeys - 2;
        for (unsigned int k = 0; k + 1 < ch->mNumPositionKeys; k++)
            if (tick < ch->mPositionKeys[k + 1].mTime) { i = k; break; }
        unsigned int j = (i + 1 < ch->mNumPositionKeys) ? i + 1 : i;
        double dt = ch->mPositionKeys[j].mTime - ch->mPositionKeys[i].mTime;
        float  f  = (dt > 0.0) ? (float)((tick - ch->mPositionKeys[i].mTime) / dt) : 0.0f;
        f = f < 0.0f ? 0.0f : (f > 1.0f ? 1.0f : f);
        return ch->mPositionKeys[i].mValue + (ch->mPositionKeys[j].mValue - ch->mPositionKeys[i].mValue) * f;
    }

    aiQuaternion AnimatedModel::InterpolateRot(const aiNodeAnim* ch, double tick)
    {
        if (ch->mNumRotationKeys == 0) return aiQuaternion(1, 0, 0, 0);
        if (ch->mNumRotationKeys == 1) return ch->mRotationKeys[0].mValue;
        unsigned int i = ch->mNumRotationKeys - 2;
        for (unsigned int k = 0; k + 1 < ch->mNumRotationKeys; k++)
            if (tick < ch->mRotationKeys[k + 1].mTime) { i = k; break; }
        unsigned int j = (i + 1 < ch->mNumRotationKeys) ? i + 1 : i;
        double dt = ch->mRotationKeys[j].mTime - ch->mRotationKeys[i].mTime;
        float  f  = (dt > 0.0) ? (float)((tick - ch->mRotationKeys[i].mTime) / dt) : 0.0f;
        f = f < 0.0f ? 0.0f : (f > 1.0f ? 1.0f : f);
        aiQuaternion out;
        aiQuaternion::Interpolate(out, ch->mRotationKeys[i].mValue, ch->mRotationKeys[j].mValue, f);
        return out.Normalize();
    }

    aiVector3D AnimatedModel::InterpolateScl(const aiNodeAnim* ch, double tick)
    {
        if (ch->mNumScalingKeys == 0) return aiVector3D(1, 1, 1);
        if (ch->mNumScalingKeys == 1) return ch->mScalingKeys[0].mValue;
        unsigned int i = ch->mNumScalingKeys - 2;
        for (unsigned int k = 0; k + 1 < ch->mNumScalingKeys; k++)
            if (tick < ch->mScalingKeys[k + 1].mTime) { i = k; break; }
        unsigned int j = (i + 1 < ch->mNumScalingKeys) ? i + 1 : i;
        double dt = ch->mScalingKeys[j].mTime - ch->mScalingKeys[i].mTime;
        float  f  = (dt > 0.0) ? (float)((tick - ch->mScalingKeys[i].mTime) / dt) : 0.0f;
        f = f < 0.0f ? 0.0f : (f > 1.0f ? 1.0f : f);
        return ch->mScalingKeys[i].mValue + (ch->mScalingKeys[j].mValue - ch->mScalingKeys[i].mValue) * f;
    }

    std::string AnimatedModel::LogicalBoneName(const std::string& nodeName)
    {
        size_t colon = nodeName.find_last_of(':');
        return (colon != std::string::npos) ? nodeName.substr(colon + 1) : nodeName;
    }

    void AnimatedModel::BuildGlobalTransforms(
        const aiNode* node, const aiAnimation* anim, double tick,
        const aiMatrix4x4& parent, std::map<std::string, aiMatrix4x4>& out)
    {
        std::string name(node->mName.C_Str());
        aiMatrix4x4 nodeT = node->mTransformation;

        // ----- Procedural dance: override the bind local rotation with a
        // hand-authored delta (applied in the bone's local space). -----
        if (mUseProceduralPose)
        {
            auto pit = mProceduralPose.find(LogicalBoneName(name));
            if (pit != mProceduralPose.end())
            {
                aiVector3D   bindScl(1, 1, 1);
                aiQuaternion bindRot(1, 0, 0, 0);
                aiVector3D   bindPos(0, 0, 0);
                node->mTransformation.Decompose(bindScl, bindRot, bindPos);

                glm::quat bind(bindRot.w, bindRot.x, bindRot.y, bindRot.z);
                glm::quat fin = glm::normalize(bind * pit->second);

                const float x = fin.x, y = fin.y, z = fin.z, w = fin.w;
                const float xx = x * x, yy = y * y, zz = z * z;
                const float xy = x * y, xz = x * z, yz = y * z;
                const float wx = w * x, wy = w * y, wz = w * z;

                const float R00 = 1.0f - 2.0f * (yy + zz);
                const float R01 = 2.0f * (xy - wz);
                const float R02 = 2.0f * (xz + wy);
                const float R10 = 2.0f * (xy + wz);
                const float R11 = 1.0f - 2.0f * (xx + zz);
                const float R12 = 2.0f * (yz - wx);
                const float R20 = 2.0f * (xz - wy);
                const float R21 = 2.0f * (yz + wx);
                const float R22 = 1.0f - 2.0f * (xx + yy);

                nodeT.a1 = R00 * bindScl.x; nodeT.a2 = R01 * bindScl.y; nodeT.a3 = R02 * bindScl.z; nodeT.a4 = bindPos.x;
                nodeT.b1 = R10 * bindScl.x; nodeT.b2 = R11 * bindScl.y; nodeT.b3 = R12 * bindScl.z; nodeT.b4 = bindPos.y;
                nodeT.c1 = R20 * bindScl.x; nodeT.c2 = R21 * bindScl.y; nodeT.c3 = R22 * bindScl.z; nodeT.c4 = bindPos.z;
                nodeT.d1 = 0.0f;            nodeT.d2 = 0.0f;            nodeT.d3 = 0.0f;            nodeT.d4 = 1.0f;
            }

            aiMatrix4x4 globalT = parent * nodeT;
            out[name] = globalT;
            for (unsigned int i = 0; i < node->mNumChildren; i++)
                BuildGlobalTransforms(node->mChildren[i], anim, tick, globalT, out);
            return;
        }

        const aiNodeAnim* ch = anim ? FindChannel(anim, name) : nullptr;
        if (ch)
        {
            aiVector3D   bindScl(1, 1, 1);
            aiQuaternion bindRot(1, 0, 0, 0);
            aiVector3D   bindPos(0, 0, 0);
            node->mTransformation.Decompose(bindScl, bindRot, bindPos);

            aiVector3D   pos = (ch->mNumPositionKeys > 0) ? InterpolatePos(ch, tick) : bindPos;
            aiQuaternion rot = (ch->mNumRotationKeys > 0) ? InterpolateRot(ch, tick) : bindRot;
            aiVector3D   scl = (ch->mNumScalingKeys  > 0) ? InterpolateScl(ch, tick) : bindScl;

            const float x = rot.x, y = rot.y, z = rot.z, w = rot.w;
            const float xx = x * x, yy = y * y, zz = z * z;
            const float xy = x * y, xz = x * z, yz = y * z;
            const float wx = w * x, wy = w * y, wz = w * z;

            const float R00 = 1.0f - 2.0f * (yy + zz);
            const float R01 = 2.0f * (xy - wz);
            const float R02 = 2.0f * (xz + wy);
            const float R10 = 2.0f * (xy + wz);
            const float R11 = 1.0f - 2.0f * (xx + zz);
            const float R12 = 2.0f * (yz - wx);
            const float R20 = 2.0f * (xz - wy);
            const float R21 = 2.0f * (yz + wx);
            const float R22 = 1.0f - 2.0f * (xx + yy);

            nodeT.a1 = R00 * scl.x; nodeT.a2 = R01 * scl.y; nodeT.a3 = R02 * scl.z; nodeT.a4 = pos.x;
            nodeT.b1 = R10 * scl.x; nodeT.b2 = R11 * scl.y; nodeT.b3 = R12 * scl.z; nodeT.b4 = pos.y;
            nodeT.c1 = R20 * scl.x; nodeT.c2 = R21 * scl.y; nodeT.c3 = R22 * scl.z; nodeT.c4 = pos.z;
            nodeT.d1 = 0.0f;        nodeT.d2 = 0.0f;        nodeT.d3 = 0.0f;        nodeT.d4 = 1.0f;
        }

        aiMatrix4x4 globalT = parent * nodeT;
        out[name] = globalT;

        for (unsigned int i = 0; i < node->mNumChildren; i++)
            BuildGlobalTransforms(node->mChildren[i], anim, tick, globalT, out);
    }

    void AnimatedModel::UpdateBoneMatrices()
    {
        if (!mScene || mNumBones <= 0)
            return;

        // TEST TOGGLE: force bind pose (identity bone matrices) to render the
        // raw mesh with no skinning math. Used to isolate model vs. skinning.
        const bool kForceBindPose = false;
        if (kForceBindPose)
        {
            for (int i = 0; i < ANIM_MAX_BONES; ++i)
                mBoneMatrices[i] = glm::mat4(1.0f);
            return;
        }

        const aiAnimation* anim = mScene->HasAnimations() ? mScene->mAnimations[0] : nullptr;
        std::map<std::string, aiMatrix4x4> globalBones;
        BuildGlobalTransforms(mScene->mRootNode, anim, mAnimTimeTicks, aiMatrix4x4(), globalBones);

        for (auto& kv : mBoneNameToIdx)
        {
            const int idx = kv.second;
            if (idx < 0 || idx >= ANIM_MAX_BONES)
                continue;

            auto git = globalBones.find(kv.first);
            if (git != globalBones.end())
                mBoneMatrices[idx] = AiMat4ToGlm(git->second * mBoneOffsets[idx]);
            else
                mBoneMatrices[idx] = glm::mat4(1.0f);
        }
    }

    bool AnimatedModel::LoadModel(const std::string& path)
    {
        meshes.clear();
        mBoneNameToIdx.clear();
        mBoneOffsets.clear();
        mNumBones = 0;
        mAnimTimeTicks = 0.0;
        mAnimDuration  = 0.0;
        mTicksPerSec   = 25.0;
        mBoneMatrices.assign(ANIM_MAX_BONES, glm::mat4(1.0f));

        // Store the directory so external texture paths can be resolved relative to it.
        {
            size_t lastSlash = path.find_last_of("/\\");
            mModelDirectory = (lastSlash != std::string::npos) ? path.substr(0, lastSlash + 1) : "./";
        }

        mShader = std::make_unique<Core::Shader>();
        mShader->CompileShader(".\\src\\utils\\gltf\\shader\\model\\skinned.vert", Core::Shader::Type::VERTEX);
        mShader->CompileShader(".\\src\\utils\\gltf\\shader\\model\\skinned.frag",   Core::Shader::Type::FRAGMENT);
        mShader->BindAttribLocation(0, "a_position");
        mShader->BindAttribLocation(2, "a_normal");
        mShader->BindAttribLocation(3, "a_texcoord");
        mShader->BindAttribLocation(4, "a_tangent");
        mShader->BindAttribLocation(6, "a_boneIDs");
        mShader->BindAttribLocation(7, "a_boneWeights");
        mShader->Link();

        // Collapse Assimp's synthetic $AssimpFbx$ pivot nodes so each bone is a
        // single node. Without this, Mixamo FBX rigs animate with compounding
        // error toward the extremities (stretched hands/legs).
        mImporter.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

        mScene = mImporter.ReadFile(path,
            aiProcess_Triangulate |
            aiProcess_CalcTangentSpace |
            aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs |
            aiProcess_LimitBoneWeights);

        if (!mScene || (mScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !mScene->mRootNode)
        {
            PrintLogFunction(__FUNCTION__, "AnimatedModel: failed to load '%s': %s",
                path.c_str(), mImporter.GetErrorString());
            return false;
        }

        if (mScene->HasAnimations())
        {
            const aiAnimation* anim = mScene->mAnimations[0];
            mAnimDuration = anim->mDuration;
            mTicksPerSec  = (anim->mTicksPerSecond > 1e-6) ? anim->mTicksPerSecond : 25.0;
            PrintLogFunction(__FUNCTION__, "AnimatedModel '%s': anim='%s' dur=%.2f tps=%.2f",
                path.c_str(), anim->mName.C_Str(), mAnimDuration, mTicksPerSec);
        }

        for (unsigned int mi = 0; mi < mScene->mNumMeshes; mi++)
        {
            const aiMesh* mesh = mScene->mMeshes[mi];
            for (unsigned int bi = 0; bi < mesh->mNumBones; bi++)
            {
                const aiBone* bone = mesh->mBones[bi];
                std::string   name = bone->mName.C_Str();
                if (mBoneNameToIdx.find(name) == mBoneNameToIdx.end())
                {
                    mBoneNameToIdx[name] = mNumBones;
                    mBoneOffsets.push_back(bone->mOffsetMatrix);
                    mNumBones++;
                }
            }
        }

        ProcessNode(mScene->mRootNode);
        UpdateBoneMatrices();

        PrintLogFunction(__FUNCTION__, "AnimatedModel '%s': bones=%d meshes=%zu",
            path.c_str(), mNumBones, meshes.size());

        return true;
    }

    void AnimatedModel::ProcessNode(const aiNode* node)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
            meshes.push_back(ProcessMesh(mScene->mMeshes[node->mMeshes[i]]));

        for (unsigned int i = 0; i < node->mNumChildren; i++)
            ProcessNode(node->mChildren[i]);
    }

    SkinnedMesh AnimatedModel::ProcessMesh(const aiMesh* mesh)
    {
        std::vector<SkinnedVertex> vertices(mesh->mNumVertices);
        std::vector<unsigned int>  indices;
        std::vector<ModelTexture>  textures;

        for (auto& v : vertices)
        {
            for (int k = 0; k < ANIM_MAX_BONE_INFLUENCE; k++)
            {
                v.boneIDs[k]     = 0;
                v.boneWeights[k] = 0.0f;
            }
        }

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            SkinnedVertex& v = vertices[i];
            if (mesh->HasPositions())
                v.position  = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            if (mesh->HasNormals())
                v.normal    = glm::vec3(mesh->mNormals[i].x,  mesh->mNormals[i].y,  mesh->mNormals[i].z);
            if (mesh->mTextureCoords[0])
                v.texcoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            if (mesh->HasTangentsAndBitangents())
                v.tangent   = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
        }

        for (unsigned int bi = 0; bi < mesh->mNumBones; bi++)
        {
            const aiBone* bone = mesh->mBones[bi];
            auto it = mBoneNameToIdx.find(bone->mName.C_Str());
            if (it == mBoneNameToIdx.end()) continue;
            const int boneIdx = it->second;

            for (unsigned int wi = 0; wi < bone->mNumWeights; wi++)
            {
                const unsigned int vid    = bone->mWeights[wi].mVertexId;
                const float        weight = bone->mWeights[wi].mWeight;
                if (weight <= 0.0f) continue;

                for (int k = 0; k < ANIM_MAX_BONE_INFLUENCE; k++)
                {
                    if (vertices[vid].boneWeights[k] == 0.0f)
                    {
                        vertices[vid].boneIDs[k]     = boneIdx;
                        vertices[vid].boneWeights[k] = weight;
                        break;
                    }
                }
            }
        }

        for (auto& v : vertices)
        {
            float sum = 0.0f;
            for (int k = 0; k < ANIM_MAX_BONE_INFLUENCE; k++)
                sum += v.boneWeights[k];
            if (sum > 1e-5f && (sum < 0.999f || sum > 1.001f))
            {
                for (int k = 0; k < ANIM_MAX_BONE_INFLUENCE; k++)
                    v.boneWeights[k] /= sum;
            }
        }

        for (unsigned int fi = 0; fi < mesh->mNumFaces; fi++)
        {
            const aiFace& face = mesh->mFaces[fi];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        if (mesh->mMaterialIndex >= 0)
        {
            const aiMaterial* mat = mScene->mMaterials[mesh->mMaterialIndex];

            // Try PBR base color first; FBX/Mixamo files store diffuse under
            // aiTextureType_DIFFUSE so fall back to that if BASE_COLOR is empty.
            auto d = LoadMaterialTextures(mat, aiTextureType_BASE_COLOR, TextureType::DiffuseMap);
            if (d.empty())
                d = LoadMaterialTextures(mat, aiTextureType_DIFFUSE, TextureType::DiffuseMap);
            textures.insert(textures.end(), d.begin(), d.end());

            auto n  = LoadMaterialTextures(mat, aiTextureType_NORMALS,    TextureType::NormalMap);
            if (n.empty())
                n = LoadMaterialTextures(mat, aiTextureType_HEIGHT,       TextureType::NormalMap);
            textures.insert(textures.end(), n.begin(), n.end());

            auto mr = LoadMaterialTextures(mat, aiTextureType_METALNESS,  TextureType::MetallicRoughnessMap);
            textures.insert(textures.end(), mr.begin(), mr.end());

            auto e  = LoadMaterialTextures(mat, aiTextureType_EMISSIVE,   TextureType::EmissiveMap);
            textures.insert(textures.end(), e.begin(), e.end());
        }

        return SkinnedMesh(std::move(vertices), std::move(indices), std::move(textures));
    }

    void AnimatedModel::Update(float deltaSeconds)
    {
        if (!mScene)
            return;

        if (mUseProceduralPose)
        {
            EvaluateProceduralPose(deltaSeconds);
            UpdateBoneMatrices();
            return;
        }

        const aiAnimation* anim = mScene->HasAnimations() ? mScene->mAnimations[0] : nullptr;
        if (anim)
        {
            mAnimTimeTicks += (double)deltaSeconds * mTicksPerSec;
            if (mAnimDuration > 0.0)
                mAnimTimeTicks = fmod(mAnimTimeTicks, mAnimDuration);
        }

        UpdateBoneMatrices();
    }

    void AnimatedModel::EnableProceduralDance(bool enable)
    {
        mUseProceduralPose = enable;
        mDanceTime = 0.0f;
        if (enable && mDancePoses.empty())
            BuildTandavPoses();
    }

    // Author the looping Tandav dance as a sequence of expressive key poses.
    // Each value is a local-space rotation delta (XYZ Euler degrees) applied on
    // top of the bind (T) pose. Because Mixamo bone local axes are not world
    // aligned, these were tuned visually; tweak the numbers to restyle the dance.
    void AnimatedModel::BuildTandavPoses()
    {
        mDancePoses.clear();

        // Pose 0 - grounded ready stance, both arms raised outward/up.
        {
            DancePose p; p.blendIn = 0.8f; p.holdAfter = 0.3f;
            p.euler["Spine"]        = glm::vec3( 0,  0,   0);
            p.euler["Spine1"]       = glm::vec3( 0,  0,   0);
            p.euler["Neck"]         = glm::vec3( 0,  0,   0);
            p.euler["LeftArm"]      = glm::vec3( 0,  0,  60);
            p.euler["LeftForeArm"]  = glm::vec3( 0,  0,  30);
            p.euler["RightArm"]     = glm::vec3( 0,  0, -60);
            p.euler["RightForeArm"] = glm::vec3( 0,  0, -30);
            p.euler["LeftUpLeg"]    = glm::vec3( 0,  0,  10);
            p.euler["RightUpLeg"]   = glm::vec3( 0,  0, -10);
            mDancePoses.push_back(p);
        }

        // Pose 1 - Nataraja: left knee lifts & crosses, right arm sweeps across.
        {
            DancePose p; p.blendIn = 0.7f; p.holdAfter = 0.4f;
            p.euler["Spine"]        = glm::vec3( 0,  0,  -8);
            p.euler["Spine1"]       = glm::vec3( 0, 15,  -6);
            p.euler["Neck"]         = glm::vec3( 0, 10,   0);
            p.euler["LeftArm"]      = glm::vec3( 0,  0,  95);
            p.euler["LeftForeArm"]  = glm::vec3( 0, 40,  20);
            p.euler["RightArm"]     = glm::vec3( 0,  0, -35);
            p.euler["RightForeArm"] = glm::vec3( 0,-60, -10);
            p.euler["LeftUpLeg"]    = glm::vec3(70,  0,  25);
            p.euler["LeftLeg"]      = glm::vec3(-90, 0,   0);
            p.euler["RightUpLeg"]   = glm::vec3( 0,  0, -8);
            mDancePoses.push_back(p);
        }

        // Pose 2 - open both arms wide, slight back-bend (peak of the beat).
        {
            DancePose p; p.blendIn = 0.6f; p.holdAfter = 0.25f;
            p.euler["Spine"]        = glm::vec3(-10, 0,   0);
            p.euler["Spine1"]       = glm::vec3(-8,  0,   0);
            p.euler["Neck"]         = glm::vec3(-12, 0,   0);
            p.euler["LeftArm"]      = glm::vec3( 0,  0, 100);
            p.euler["LeftForeArm"]  = glm::vec3( 0,  0,  10);
            p.euler["RightArm"]     = glm::vec3( 0,  0,-100);
            p.euler["RightForeArm"] = glm::vec3( 0,  0, -10);
            p.euler["LeftUpLeg"]    = glm::vec3( 0,  0,  12);
            p.euler["RightUpLeg"]   = glm::vec3( 0,  0, -12);
            mDancePoses.push_back(p);
        }

        // Pose 3 - mirror of pose 1: right knee lifts, left arm sweeps across.
        {
            DancePose p; p.blendIn = 0.7f; p.holdAfter = 0.4f;
            p.euler["Spine"]        = glm::vec3( 0,  0,   8);
            p.euler["Spine1"]       = glm::vec3( 0,-15,   6);
            p.euler["Neck"]         = glm::vec3( 0,-10,   0);
            p.euler["RightArm"]     = glm::vec3( 0,  0, -95);
            p.euler["RightForeArm"] = glm::vec3( 0,-40, -20);
            p.euler["LeftArm"]      = glm::vec3( 0,  0,  35);
            p.euler["LeftForeArm"]  = glm::vec3( 0, 60,  10);
            p.euler["RightUpLeg"]   = glm::vec3(70,  0, -25);
            p.euler["RightLeg"]     = glm::vec3(-90, 0,   0);
            p.euler["LeftUpLeg"]    = glm::vec3( 0,  0,  8);
            mDancePoses.push_back(p);
        }
    }

    void AnimatedModel::EvaluateProceduralPose(float dt)
    {
        const int n = (int)mDancePoses.size();
        if (n == 0)
            return;

        mDanceTime += dt * mDanceSpeed;

        // Equal-length, continuously looping segments. No holds -> the body
        // never freezes, which is what kills the robotic stop-go feeling.
        const float segDur = 1.0f;                 // seconds per pose->pose hop
        const float total  = (float)n * segDur;
        float t = fmodf(mDanceTime, total);
        if (t < 0.0f) t += total;

        const int   seg   = ((int)(t / segDur)) % n;
        const float local = (t - seg * segDur) / segDur; // 0..1 within segment

        const int i0 = (seg - 1 + n) % n;
        const int i1 =  seg;
        const int i2 = (seg + 1) % n;
        const int i3 = (seg + 2) % n;

        // Union of every bone used anywhere in the loop, so bones fade in/out
        // smoothly (absent in a pose == zero delta) across the whole spline.
        std::map<std::string, int> bones;
        for (auto& p : mDancePoses)
            for (auto& kv : p.euler) bones[kv.first] = 1;
        bones["Hips"] = 1; // always present for a gentle full-body weight shift

        auto eulerAt = [&](int idx, const std::string& bone) -> glm::vec3
        {
            auto it = mDancePoses[idx].euler.find(bone);
            return (it != mDancePoses[idx].euler.end()) ? it->second : glm::vec3(0.0f);
        };

        // Continuous secondary motion (breathing sway) layered on top so the
        // performance always has organic life, even mid-hold of a limb.
        const float ph = mDanceTime;

        mProceduralPose.clear();
        for (auto& kv : bones)
        {
            const std::string& b = kv.first;
            glm::vec3 e = CatmullRom(eulerAt(i0, b), eulerAt(i1, b),
                                     eulerAt(i2, b), eulerAt(i3, b), local);

            // Per-bone organic sway (degrees). Different freqs/phases avoid a
            // mechanical look; amplitudes kept small so poses stay readable.
            if (b == "Hips")         e += glm::vec3(1.5f * sinf(ph * 0.9f), 4.0f * sinf(ph * 0.7f),        3.0f * sinf(ph * 0.9f));
            else if (b == "Spine")   e += glm::vec3(2.0f * sinf(ph * 1.3f), 5.0f * sinf(ph * 0.9f),        3.0f * sinf(ph * 1.1f));
            else if (b == "Spine1")  e += glm::vec3(2.0f * sinf(ph * 1.1f + 0.6f), 4.0f * sinf(ph * 0.9f + 0.4f), 2.5f * sinf(ph * 1.2f));
            else if (b == "Neck")    e += glm::vec3(3.0f * sinf(ph * 1.6f), 5.0f * sinf(ph * 0.8f),        2.0f * sinf(ph * 1.0f));
            else if (b == "LeftForeArm")  e += glm::vec3(0.0f, 6.0f * sinf(ph * 1.7f),        4.0f * sinf(ph * 1.4f));
            else if (b == "RightForeArm") e += glm::vec3(0.0f, 6.0f * sinf(ph * 1.7f + 3.14f), 4.0f * sinf(ph * 1.4f + 3.14f));
            else if (b == "LeftArm")      e += glm::vec3(0.0f, 0.0f, 3.0f * sinf(ph * 1.0f));
            else if (b == "RightArm")     e += glm::vec3(0.0f, 0.0f, 3.0f * sinf(ph * 1.0f + 3.14f));

            mProceduralPose[b] = glm::quat(glm::radians(e));
        }
    }

    void AnimatedModel::SetBaseColorTexture(const std::string& path)
    {
        mOverrideBaseColor = TextureModel::LoadTextureModel(path);
        PrintLogFunction(__FUNCTION__, "AnimatedModel override base color '%s': id=%u",
            path.c_str(), mOverrideBaseColor);
    }

    void AnimatedModel::Draw(std::unique_ptr<Shader>& shader)
    {
        const GLint loc = glGetUniformLocation(shader->GetShaderProgram(), "u_BoneMatrices");
        if (loc >= 0 && mNumBones > 0)
            glUniformMatrix4fv(loc, ANIM_MAX_BONES, GL_FALSE, &mBoneMatrices[0][0][0]);

        if (mOverrideBaseColor)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mOverrideBaseColor);
            glUniform1i(glGetUniformLocation(shader->GetShaderProgram(), "u_BaseColorSampler"), 0);
        }

        for (auto& mesh : meshes)
            mesh.DrawMesh(shader);

        // Restore clean texture state so the bound skin texture does not leak
        // into later passes (e.g. the water reflection/refraction FBOs).
        for (int unit = 4; unit >= 0; --unit)
        {
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        glActiveTexture(GL_TEXTURE0);
    }

    unsigned int AnimatedModel::LoadEmbeddedTexture(const aiTexture* texture)
    {
        unsigned int texID;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);

        if (texture->mHeight == 0)
        {
            int w, h, ch;
            unsigned char* data = stbi_load_from_memory(
                reinterpret_cast<unsigned char*>(texture->pcData),
                texture->mWidth, &w, &h, &ch, 0);
            if (data)
            {
                GLenum fmt = (ch == 1) ? GL_RED : (ch == 3) ? GL_RGB : GL_RGBA;
                glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
                stbi_image_free(data);
            }
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                         texture->mWidth, texture->mHeight, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, texture->pcData);
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return texID;
    }

    std::vector<ModelTexture> AnimatedModel::LoadMaterialTextures(
        const aiMaterial* mat, aiTextureType type, TextureType texType)
    {
        std::vector<ModelTexture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            const aiTexture* embedded = mScene->GetEmbeddedTexture(str.C_Str());
            ModelTexture tex = {};
            tex.type = texType;
            tex.path = str;

            if (embedded)
            {
                tex.id = LoadEmbeddedTexture(embedded);
            }
            else
            {
                // The path stored in the FBX is often an absolute path from the
                // artist's machine (e.g. "C:\Users\...\texture.png"). Strip it
                // down to just the filename and resolve it next to the model file.
                std::string rawPath = str.C_Str();
                size_t slash = rawPath.find_last_of("/\\");
                std::string filename = (slash != std::string::npos)
                    ? rawPath.substr(slash + 1) : rawPath;

                // 1. Try next to the model file.
                std::string fullPath = mModelDirectory + filename;
                tex.id = TextureModel::LoadTextureModel(fullPath.c_str());

                // 2. Try raw path as-is (in case it's already a valid relative path).
                if (!tex.id)
                    tex.id = TextureModel::LoadTextureModel(rawPath.c_str());

                if (!tex.id)
                    PrintLogFunction(__FUNCTION__,
                        "Could not load texture '%s' (also tried '%s')",
                        fullPath.c_str(), rawPath.c_str());
            }

            if (tex.id)
                textures.push_back(tex);
        }
        return textures;
    }
}
