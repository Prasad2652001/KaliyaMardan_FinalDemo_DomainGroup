#pragma once
#include "Common.h"
#include "Model.h"
#include "Shader.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
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
        // Cached element count so the heavy CPU-side vertices/indices can be
        // released after they are uploaded to the GPU (saves a lot of RAM when
        // many models are loaded, e.g. the 8 Krishna Tandav pose GLBs).
        GLsizei indexCount = 0;

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

        std::string mModelDirectory; // directory of the loaded model file

        void ProcessNode(const aiNode* node);
        SkinnedMesh ProcessMesh(const aiMesh* mesh);
        void UpdateBoneMatrices();

        void BuildGlobalTransforms(
            const aiNode* node, const aiAnimation* anim, double tick,
            const aiMatrix4x4& parent, std::map<std::string, aiMatrix4x4>& out);

        // ----- Procedural keyframe-pose animation (e.g. Tandav dance) -----
        // A single dance pose: per-bone local-space rotation deltas (applied on
        // top of the bind pose). Keyed by "logical" bone name (the part after
        // the last ':' so it works with or without the "mixamorig:" prefix).
        struct DancePose
        {
            std::map<std::string, glm::vec3> euler; // bone -> XYZ degrees
            float holdAfter = 0.0f;                 // seconds to hold this pose
            float blendIn   = 0.6f;                 // seconds to blend into it
        };

        std::vector<DancePose>             mDancePoses;
        bool                               mUseProceduralPose = false;
        float                              mDanceTime         = 0.0f;
        float                              mDanceSpeed        = 2.6f; // playback multiplier
        std::map<std::string, glm::quat>   mProceduralPose;   // logical name -> delta quat

        void BuildTandavPoses();
        void EvaluateProceduralPose(float dt);
        static std::string LogicalBoneName(const std::string& nodeName);

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

        // Replace the embedded clip with a hand-authored procedural dance that
        // drives the rig bones through a looping sequence of Tandav poses.
        void EnableProceduralDance(bool enable);

        // Playback speed multiplier for the procedural dance (1.0 = authored).
        void SetDanceSpeed(float speed) { mDanceSpeed = speed; }

    private:
        unsigned int mOverrideBaseColor = 0;
    };
}
