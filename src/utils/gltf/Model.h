#pragma once
#include "Common.h"
#include "../OpenGL/GLLog.h"
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include <map>
#include <string>

namespace Core
{
    enum TextureType
    {
        DiffuseMap = 0,
        NormalMap,
        MetallicRoughnessMap,
        AmbientMap,
        EmissiveMap
    };
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoords;
        glm::vec3 tangent;
        glm::vec3 bitangent;
    };

    struct PBRMaterial
    {
        glm::vec3 albedo;
        glm::float32 metallic;
        glm::float32 roughness;
        glm::float32 emissive;
        glm::float32 alpha;
    };

    struct ModelTexture
    {
        UINT id;
        TextureType type;
        aiString path;
        PBRMaterial pbrMaterial;
    };

    class Mesh
    {
        std::vector<Vertex> vertices;
        std::vector<UINT> indices;
        std::vector<ModelTexture> textures;
        GLuint vao;
        GLuint vbo;
        GLuint ebo;

    public:
        Mesh();
        Mesh(const std::vector<Vertex> &, const std::vector<UINT> &, const std::vector<ModelTexture> &);
        ~Mesh();

        void DrawMesh(std::unique_ptr<Shader> &);
        void SetupMesh();
    };

    class Model
    {
        std::vector<Mesh> meshes;
        std::map<std::string, aiMatrix4x4> mBoneTransforms; // Populated during LoadModel for skinned meshes

        Mesh ProcessMesh(const aiMesh *mesh, const aiScene *scene, const glm::mat4 &transform);
        void ProcessNode(const aiNode *node, const aiScene *scene, const glm::mat4 &parentTransform);
        glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4 &mat);
        unsigned int LoadEmbeddedTexture(const aiTexture *texture);
        std::vector<ModelTexture> loadMaterialTextures(const aiMaterial *mat, aiTextureType type, TextureType textureType, const aiScene *scene);

    public:
        std::unique_ptr<Core::Shader> mTextureShader;
        
        Model();
        ~Model();
        void SetupShader();
        bool LoadModel(const std::string &pFile);
        void Draw(std::unique_ptr<Shader> &shader);
    };
};
