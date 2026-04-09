#include "TextureModel.h"

Core::TextureModel::TextureModel()
{
}

Core::TextureModel::~TextureModel() = default;

GLuint Core::TextureModel::LoadTextureModel(const std::string &filename, bool flip)
{
    int width = 0;
    int height = 0;
    int channels = 0;
    GLuint texture = 0;

    // stbi_set_flip_vertically_on_load(flip);
    unsigned char *imageData = stbi_load(filename.c_str(), &width, &height, &channels, 0);

    if (imageData == nullptr)
    {
        PrintLogFunction(__FUNCTION__, "Failed to load Texture: %s\n", filename.c_str());
        return -1;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLenum format = 0;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(imageData);

    return texture;
}

GLuint Core::TextureModel::LoadCubeMapModel(const std::vector<std::string> &faces)
{
    int width = 0;
    int height = 0;
    int channels = 0;
    GLuint texture = 0;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *imageData = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
        if (imageData == nullptr)
        {
            PrintLogFunction(__FUNCTION__, "Failed to load Cubemap Texture: %s\n", faces[i].c_str());
            glDeleteTextures(1, &texture);
            return -1;
        }

        GLenum format = GL_RGBA;
        if (channels == 1)
            format = GL_RED;
        else if (channels == 3)
            format = GL_RGB;
        else if (channels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
        stbi_image_free(imageData);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return texture;
}
