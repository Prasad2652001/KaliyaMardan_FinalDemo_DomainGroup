#include "Model.h"
#include "TextureModel.h"
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include "Shader.h"

namespace Core
{
    Model::Model()
        : meshes{}
    {
    }

    Model::~Model()
    {
    }

    Mesh Model::ProcessMesh(const aiMesh *mesh, const aiScene *scene, const glm::mat4 &transform)
    {
        std::vector<Vertex> vertices;
        std::vector<UINT> indices;
        std::vector<ModelTexture> textures;

        // Process vertices
        for (UINT i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex = {};

            // position
            if (mesh->HasPositions())
            {
                glm::vec4 transformedPosition = transform * glm::vec4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f);
                vertex.position = glm::vec3(transformedPosition);
            }
            // normals
            if (mesh->HasNormals())
            {
                vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            }
            else
            {
                vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
            }
            // texture coordinates
            if (mesh->mTextureCoords[0])
            {
                vertex.texcoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            }
            else
            {
                vertex.texcoords = glm::vec2(0.0f, 0.0f);
            }
            if (mesh->HasTangentsAndBitangents())
            {
                // tangent
                vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);

                // bitangent
                vertex.bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
            }
            else
            {
                vertex.tangent = glm::vec3(0.0f, 0.0f, 0.0f);
                vertex.bitangent = glm::vec3(0.0f, 0.0f, 0.0f);
            }
            vertices.push_back(vertex);
        }

        // Process indices
        for (UINT i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (UINT j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Process material and textures
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial const *material = scene->mMaterials[mesh->mMaterialIndex];

            // Load diffuse maps
            std::vector<ModelTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_BASE_COLOR, TextureType::DiffuseMap, scene);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            // Load normal maps
            std::vector<ModelTexture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, TextureType::NormalMap, scene);
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

            // Load metallic roughness maps
            std::vector<ModelTexture> metallicRoughnessMaps = loadMaterialTextures(material, aiTextureType_METALNESS, TextureType::MetallicRoughnessMap, scene);
            textures.insert(textures.end(), metallicRoughnessMaps.begin(), metallicRoughnessMaps.end());

            // Load emissive maps
            std::vector<ModelTexture> emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, TextureType::EmissiveMap, scene);
            textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());

            // Load ambient maps
            std::vector<ModelTexture> ambientMaps = loadMaterialTextures(material, aiTextureType_LIGHTMAP, TextureType::AmbientMap, scene);
            textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());
        }
        return Mesh(vertices, indices, textures);
    }

    void Model::ProcessNode(const aiNode *node, const aiScene *scene, const glm::mat4 &parentTransform)
    {
        // Get the current node's transformation
        glm::mat4 nodeTransform = ConvertMatrixToGLMFormat(node->mTransformation);

        // Combine with parent's transform to get the world transform
        glm::mat4 worldTransform = parentTransform * nodeTransform;

        for (UINT i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh const *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(ProcessMesh(mesh, scene, worldTransform));
        }

        for (UINT i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene, worldTransform);
        }
    }

    glm::mat4 Model::ConvertMatrixToGLMFormat(const aiMatrix4x4 &from)
    {
        glm::mat4 to;
        to[0][0] = from.a1;
        to[0][1] = from.b1;
        to[0][2] = from.c1;
        to[0][3] = from.d1;
        to[1][0] = from.a2;
        to[1][1] = from.b2;
        to[1][2] = from.c2;
        to[1][3] = from.d2;
        to[2][0] = from.a3;
        to[2][1] = from.b3;
        to[2][2] = from.c3;
        to[2][3] = from.d3;
        to[3][0] = from.a4;
        to[3][1] = from.b4;
        to[3][2] = from.c4;
        to[3][3] = from.d4;
        return to;
    }
    void Model::SetupShader()
    {
        // create shader class instance
        mTextureShader = std::make_unique<Core::Shader>();

        mTextureShader->CompileShader(".\\src\\utils\\gltf\\shader\\model\\model.vert", Core::Shader::Type::VERTEX);
        mTextureShader->CompileShader(".\\src\\utils\\gltf\\shader\\model\\model.frag", Core::Shader::Type::FRAGMENT);

        mTextureShader->BindAttribLocation(Core::ATTRIBUTE::POSITION, "a_position");
        mTextureShader->BindAttribLocation(Core::ATTRIBUTE::TEXCOORD, "a_texcoord");
        mTextureShader->BindAttribLocation(Core::ATTRIBUTE::NORMAL, "a_normal");

        mTextureShader->Link();
    }

    bool Model::LoadModel(const std::string &pFile)
    {
        // Setting up shader for Model
        this->SetupShader();

        // Importer class
        Assimp::Importer importer;

        const aiScene *scene = importer.ReadFile(pFile,
                                                 aiProcess_Triangulate |
                                                     aiProcess_CalcTangentSpace |
                                                     aiProcess_GenSmoothNormals |
                                                     aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            PrintLogFunction(__FUNCTION__, "Failed to parse model file: %s", importer.GetErrorString());
            return false;
        }

        ProcessNode(scene->mRootNode, scene, glm::mat4(1.0f));

        return true;
    }

    void Model::Draw(std::unique_ptr<Shader> &shader)
    {
        for (auto &mesh : meshes)
        {
            mesh.DrawMesh(shader);
        }
    }

    unsigned int Model::LoadEmbeddedTexture(const aiTexture *texture)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Check if the texture is compressed (JPEG, PNG)
        if (texture->mHeight == 0)
        {
            // Texture is compressed (e.g., JPEG/PNG)
            int width, height, nrComponents;
            unsigned char *data = stbi_load_from_memory(reinterpret_cast<unsigned char *>(texture->pcData),
                                                        texture->mWidth, &width, &height, &nrComponents, 0);

            if (data)
            {
                GLenum format{};
                if (nrComponents == 1)
                    format = GL_RED;
                else if (nrComponents == 3)
                    format = GL_RGB;
                else if (nrComponents == 4)
                    format = GL_RGBA;

                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                stbi_image_free(data);
            }
            else
            {
                // @TODO: Add texture path for debugging
                PrintLogFunction(__FUNCTION__, "Failed to load embedded textures.\n");
                stbi_image_free(data);
            }
        }
        else
        {
            // Texture is uncompressed (RAW format)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->mWidth, texture->mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->pcData);
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return textureID;
    }

    std::vector<ModelTexture> Model::loadMaterialTextures(const aiMaterial *mat, aiTextureType type, TextureType textureType, const aiScene *scene)
    {
        std::vector<ModelTexture> textures;
        std::unordered_map<std::string, ModelTexture> textures_loaded;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {

            aiString str;
            mat->GetTexture(type, i, &str);
            std::string texturePath = str.C_Str();

            // check if the texture is already loaded in the map
            if (textures_loaded.find(texturePath) != textures_loaded.end())
            {
                textures.push_back(textures_loaded[texturePath]);
                continue;
            }

            const aiTexture *texture = scene->GetEmbeddedTexture(str.C_Str());

            ModelTexture tex = {};

            if (texture)
            {
                // Load the embedded texture.
                tex.id = LoadEmbeddedTexture(texture);
            }
            else
            {
                // Load external texture from file.
                tex.id = TextureModel::LoadTextureModel(str.C_Str());
            }

            tex.type = textureType;
            tex.path = str;

            // PBR MR workflow
            FLOAT metallic;
            mat->Get(AI_MATKEY_METALLIC_FACTOR, metallic);
            tex.pbrMaterial.metallic = metallic;

            FLOAT roughness;
            mat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);
            tex.pbrMaterial.roughness = roughness;

            aiColor3D albedo;
            mat->Get(AI_MATKEY_COLOR_DIFFUSE, albedo);
            tex.pbrMaterial.albedo = glm::vec3(albedo.r, albedo.g, albedo.b);

            FLOAT emissive;
            mat->Get(AI_MATKEY_EMISSIVE_INTENSITY, emissive);
            tex.pbrMaterial.emissive = emissive;

            FLOAT alpha;
            mat->Get(AI_MATKEY_OPACITY, alpha);
            tex.pbrMaterial.alpha = alpha;

            textures.push_back(tex);
            textures_loaded[texturePath] = tex;
        }
        return textures;
    }

    Mesh::Mesh() = default;

    Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<UINT> &indices, const std::vector<ModelTexture> &textures)
        : vertices{vertices}, indices{indices}, textures{textures},
          vao{0}, vbo{0}, ebo{0}
    {
        SetupMesh();
    }

    Mesh::~Mesh() = default;

    void Mesh::DrawMesh(std::unique_ptr<Shader> &shader)
    {

        for (UINT i = 0; i < textures.size(); i++)
        {
            // Activate the correct texture unit
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);

            std::string uniformName;
            TextureType type = textures[i].type;

            switch (type)
            {
            case TextureType::DiffuseMap:
                uniformName = "u_BaseColorSampler";
                break;
            case TextureType::NormalMap:
                uniformName = "u_NormalSampler";
                break;
            case TextureType::MetallicRoughnessMap:
                uniformName = "u_MetallicRoughnessSampler";
                break;
            case TextureType::EmissiveMap:
                uniformName = "u_EmissiveSampler";
                break;
            case TextureType::AmbientMap:
                uniformName = "u_AmbientSampler";
                break;
            default:
                break;
            }

            glUniform1i(glGetUniformLocation(shader->GetShaderProgram(), uniformName.c_str()), static_cast<GLint>(i));
            shader->SetUniform("albedo", vmath::vec3(textures[i].pbrMaterial.albedo.r, textures[i].pbrMaterial.albedo.g, textures[i].pbrMaterial.albedo.b));
            shader->SetUniform("u_MetallicFactor", textures[i].pbrMaterial.metallic);
            shader->SetUniform("u_RoughnessFactor", textures[i].pbrMaterial.roughness);
            shader->SetUniform("u_Exposure", shader->exposure);
            shader->SetUniform("u_MipCount", 10);
            shader->SetUniform("u_EnvIntensity", 0.5f);
            shader->SetUniform("u_MaterialShininess", 64.0f);          // 128.0f
            shader->SetUniform("u_LD", vmath::vec3(1.0f, 1.0f, 1.0f)); //  TODO : remove hard coding
            shader->SetUniform("u_LS", vmath::vec3(1.0f, 1.0f, 1.0f));
        }

        // Draw
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        //// unbind everything
        for (UINT i = 0; i < textures.size(); i++)
        {
            // Activate the correct texture unit
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void Mesh::SetupMesh()
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // positions
        glVertexAttribPointer(ATTRIBUTE::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
        glEnableVertexAttribArray(ATTRIBUTE::POSITION);

        // normals
        glVertexAttribPointer(ATTRIBUTE::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(ATTRIBUTE::NORMAL);

        // texcoords
        glVertexAttribPointer(ATTRIBUTE::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(ATTRIBUTE::TEXCOORD);

        // tangents
        glVertexAttribPointer(ATTRIBUTE::TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(8 * sizeof(GLfloat)));
        glEnableVertexAttribArray(ATTRIBUTE::TANGENT);

        // bitangents
        glVertexAttribPointer(ATTRIBUTE::BITANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(11 * sizeof(GLfloat)));
        glEnableVertexAttribArray(ATTRIBUTE::BITANGENT);

        glBindVertexArray(0);
    }
};
