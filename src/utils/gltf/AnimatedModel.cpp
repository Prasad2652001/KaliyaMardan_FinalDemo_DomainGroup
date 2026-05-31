#include "AnimatedModel.h"
#include "TextureModel.h"
#include <assimp/postprocess.h>
#include <assimp/config.h>
#include "../OpenGL/GLLog.h"
#include "../../includes/stb_image.h"
#include <cmath>
#include <map>

static glm::mat4 AiMat4ToGlm(const aiMatrix4x4& m)
{
    return glm::mat4(m.a1, m.b1, m.c1, m.d1,
                     m.a2, m.b2, m.c2, m.d2,
                     m.a3, m.b3, m.c3, m.d3,
                     m.a4, m.b4, m.c4, m.d4);
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

    void AnimatedModel::BuildGlobalTransforms(
        const aiNode* node, const aiAnimation* anim, double tick,
        const aiMatrix4x4& parent, std::map<std::string, aiMatrix4x4>& out)
    {
        std::string name(node->mName.C_Str());
        aiMatrix4x4 nodeT = node->mTransformation;

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
            auto d  = LoadMaterialTextures(mat, aiTextureType_BASE_COLOR, TextureType::DiffuseMap);
            textures.insert(textures.end(), d.begin(), d.end());
            auto n  = LoadMaterialTextures(mat, aiTextureType_NORMALS,     TextureType::NormalMap);
            textures.insert(textures.end(), n.begin(), n.end());
            auto mr = LoadMaterialTextures(mat, aiTextureType_METALNESS,  TextureType::MetallicRoughnessMap);
            textures.insert(textures.end(), mr.begin(), mr.end());
            auto e  = LoadMaterialTextures(mat, aiTextureType_EMISSIVE,    TextureType::EmissiveMap);
            textures.insert(textures.end(), e.begin(), e.end());
        }

        return SkinnedMesh(std::move(vertices), std::move(indices), std::move(textures));
    }

    void AnimatedModel::Update(float deltaSeconds)
    {
        if (!mScene)
            return;

        const aiAnimation* anim = mScene->HasAnimations() ? mScene->mAnimations[0] : nullptr;
        if (anim)
        {
            mAnimTimeTicks += (double)deltaSeconds * mTicksPerSec;
            if (mAnimDuration > 0.0)
                mAnimTimeTicks = fmod(mAnimTimeTicks, mAnimDuration);
        }

        UpdateBoneMatrices();
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
                tex.id = LoadEmbeddedTexture(embedded);
            else
                tex.id = TextureModel::LoadTextureModel(str.C_Str());

            textures.push_back(tex);
        }
        return textures;
    }
}
