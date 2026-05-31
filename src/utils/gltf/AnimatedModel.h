#pragma once
#include "Common.h"
#include "Model.h"
#include "Shader.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <vector>

#define ANIM_MAX_BONES          100
#define ANIM_MAX_BONE_INFLUENCE 4

namespace Core
{
    struct SkinnedVertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoords;
        glm::vec3 tangent;
        int       boneIDs[ANIM_MAX_BONE_INFLUENCE];
        float     boneWeights[ANIM_MAX_BONE_INFLUENCE];
    };

    class SkinnedMesh
    {
        std::vector<SkinnedVertex> vertices;
        std::vector<unsigned int>  indices;
        std::vector<ModelTexture>  textures;
        GLuint vao = 0, vbo = 0, ebo = 0;

    public:
        SkinnedMesh() = default;
        SkinnedMesh(std::vector<SkinnedVertex> v,
                    std::vector<unsigned int>  i,
                    std::vector<ModelTexture>  t);
        ~SkinnedMesh();

        SkinnedMesh(const SkinnedMesh&) = delete;
        SkinnedMesh& operator=(const SkinnedMesh&) = delete;
        SkinnedMesh(SkinnedMesh&& o) noexcept;
        SkinnedMesh& operator=(SkinnedMesh&& o) noexcept;

        void SetupMesh();
        void DrawMesh(std::unique_ptr<Shader>& shader);
    };

    class AnimatedModel
    {
        Assimp::Importer           mImporter;
        const aiScene*             mScene = nullptr;

        std::vector<SkinnedMesh>   meshes;

        std::map<std::string, int> mBoneNameToIdx;
        std::vector<aiMatrix4x4>   mBoneOffsets;
        std::vector<glm::mat4>     mBoneMatrices;
        int                        mNumBones = 0;

        double  mAnimTimeTicks = 0.0;
        double  mAnimDuration  = 0.0;
        double  mTicksPerSec   = 25.0;

        void ProcessNode(const aiNode* node);
        SkinnedMesh ProcessMesh(const aiMesh* mesh);
        void UpdateBoneMatrices();

        static void BuildGlobalTransforms(
            const aiNode* node, const aiAnimation* anim, double tick,
            const aiMatrix4x4& parent, std::map<std::string, aiMatrix4x4>& out);

        std::vector<ModelTexture> LoadMaterialTextures(
            const aiMaterial* mat, aiTextureType type, TextureType texType);
        unsigned int LoadEmbeddedTexture(const aiTexture* texture);

        static const aiNodeAnim* FindChannel(const aiAnimation* anim, const std::string& name);
        static aiVector3D   InterpolatePos(const aiNodeAnim* ch, double tick);
        static aiQuaternion InterpolateRot(const aiNodeAnim* ch, double tick);
        static aiVector3D   InterpolateScl(const aiNodeAnim* ch, double tick);

    public:
        std::unique_ptr<Core::Shader> mShader;

        AnimatedModel();
        ~AnimatedModel();

        bool LoadModel(const std::string& path);
        void Update(float deltaSeconds);
        void Draw(std::unique_ptr<Shader>& shader);

        // Force a base-color (diffuse) texture for models whose FBX carries no
        // embedded skin (e.g. Mixamo exports). Bound as u_BaseColorSampler.
        void SetBaseColorTexture(const std::string& path);

    private:
        unsigned int mOverrideBaseColor = 0;
    };
}
