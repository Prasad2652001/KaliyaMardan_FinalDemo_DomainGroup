#pragma once
#include "../../utils/common.h"


//#include "GLHeadersAndMacros.h"
//#include "GLShaders.h"
//#include "common.h"
// #include "vector"

class Terrain
{

private:
    // Terrain Generation Related Variables
    static const int TERRAIN_MAP_X = 256;
    static const int TERRAIN_MAP_Z = 256;
    static const int TERRAIN_MAP_DEPTH = 3;
    float HEIGHT_MAP_SCALE = 5.0f;
    unsigned char *image_data = NULL;
    std::vector<int> heights;

    int SIZE_OF_TERRAIN_MAP = TERRAIN_MAP_X * TERRAIN_MAP_Z * TERRAIN_MAP_DEPTH * 4;

    float terrainPosition[3] = {0.0f, 0.0f, 0.0f};

public:
    float terrainHeightMap[TERRAIN_MAP_X][TERRAIN_MAP_Z][TERRAIN_MAP_DEPTH];
    float terrainHeightMapTexCoord[TERRAIN_MAP_X][TERRAIN_MAP_Z][2];
    float terrainHeightMapNormals[TERRAIN_MAP_X][TERRAIN_MAP_Z][TERRAIN_MAP_DEPTH];

    float terrainVertexData[(TERRAIN_MAP_X * TERRAIN_MAP_Z * TERRAIN_MAP_DEPTH) * 4];
    float terrainNormalData[(TERRAIN_MAP_X * TERRAIN_MAP_Z * TERRAIN_MAP_DEPTH) * 4];
    float terrainTexCoordData[(TERRAIN_MAP_X * TERRAIN_MAP_Z * 2) * 4];

    std::vector<float> vertices;
    std::vector<std::vector<float>> verticesThree;
    std::vector<float> texcoords;
    std::vector<float> normals;
    std::vector<unsigned> indices;

    int numStrips;
    int numTrisPerStrip;

    // Terrain Render Related Variables

    GLuint shaderProgramObject_terrain;

    GLuint vao_terrain;          // Vertex Array Object
    GLuint vbo_Position_terrain; // Vertex Buffer Object - Position
    GLuint vbo_TexCoord_terrain; // Vertex Buffer Object - Color
    GLuint vbo_Normal_terrain;   // Vertex Buffer Object - Color
    GLuint vbo_Indices;

    GLuint modelMatrixUniform_terrain;
    GLuint viewMatrixUniform_terrain;
    GLuint projectionMatrixUniform_terrain;

    // float objX, objY, objZ;

    GLuint texture_grass;
    GLuint texture_r;
    GLuint texture_g;
    GLuint texture_b;
    GLuint texture_blendMap;

    GLuint rTextureSamplerUniform_terrain;
    GLuint gTextureSamplerUniform_terrain;
    GLuint bTextureSamplerUniform_terrain;
    GLuint blendMapTextureSamplerUniform_terrain;
    GLuint textureSamplerUniform_terrain;
    GLuint terrainAlphaUniform;
    GLuint terrainClipPlaneUniform;
    GLuint resolutionUniform;
    GLuint timeUniform;
    GLuint enableCausticUniform;

    GLfloat timeFactor = 0.0f;
    GLuint lightPositionUniform_terrain;
    GLuint lightDiffuseUniform_terrain;
    GLuint lightShinninessUniform_terrain;
    GLuint lightReflectivityUniform_terrain;

    GLfloat lightPosition[3] = {20000.0f, 20000.0f, 2000.0f};
    GLfloat lightDiffuse[3] = {1.0f, 1.0f, 1.0f};
    GLfloat lightShinniness = 1.0f;
    GLfloat lightReflectivity = 0.1f;
    float alpha = 1.0;

    

    int initializeTerrain(void)
    {
        PrintLog(" ### Inside initializeTerrain() . ###\n");

        // vertex Shader
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/terrain/Terrain.vs", VERTEX);

        // fragment Shader
        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/terrain/Terrain.fs", FRAGMENT);

        // Shader Program Object
        
        shaderProgramObject_terrain = glCreateProgram();
        glAttachShader(shaderProgramObject_terrain, vertexShaderObject);
        glAttachShader(shaderProgramObject_terrain, fragmentShaderObject);

        // // prelinked binding
        // // Binding Position Array
        glBindAttribLocation(shaderProgramObject_terrain, VERTEX_ATTRIBUTE_POSITION, "a_position");
        // // Binding Color Array
        glBindAttribLocation(shaderProgramObject_terrain, VERTEX_ATTRIBUTE_TEXTURE0, "a_texcoord");
        //glBindAttribLocation(shaderProgramObject_terrain, VERTEX_ATTRIBUTE_NORMAL, "a_normal");

        // // // link
        BOOL bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObject_terrain);

        if (bShaderLinkStatus == FALSE)
        {
            PrintLog(" # Shader Linking ERR FOR TERRAIN. \n ");
            return -1;
        }else{
            PrintLog(" # Shader Linking SUCCESS FOR TERRAIN. \n ");
        }
        // // post link - getting
        modelMatrixUniform_terrain = glGetUniformLocation(shaderProgramObject_terrain, "u_modelMatrix");
        viewMatrixUniform_terrain = glGetUniformLocation(shaderProgramObject_terrain, "u_viewMatrix");
        projectionMatrixUniform_terrain = glGetUniformLocation(shaderProgramObject_terrain, "u_projectionMatrix");

        textureSamplerUniform_terrain = glGetUniformLocation(shaderProgramObject_terrain, "texture_background");
        blendMapTextureSamplerUniform_terrain = glGetUniformLocation(shaderProgramObject_terrain, "texture_blendMap");
        rTextureSamplerUniform_terrain = glGetUniformLocation(shaderProgramObject_terrain, "texture_r");
        gTextureSamplerUniform_terrain = glGetUniformLocation(shaderProgramObject_terrain, "texture_g");
        bTextureSamplerUniform_terrain = glGetUniformLocation(shaderProgramObject_terrain, "texture_b");

        lightPositionUniform_terrain = glGetUniformLocation(shaderProgramObject_terrain, "lightPosition");
        lightDiffuseUniform_terrain = glGetUniformLocation(shaderProgramObject_terrain, "lightColor");
        lightShinninessUniform_terrain = glGetUniformLocation(shaderProgramObject_terrain, "shineDamper");
        lightReflectivityUniform_terrain = glGetUniformLocation(shaderProgramObject_terrain, "reflectivity");

        terrainAlphaUniform = glGetUniformLocation(shaderProgramObject_terrain, "u_alpha");
        terrainClipPlaneUniform = glGetUniformLocation(shaderProgramObject_terrain, "u_plane");
        resolutionUniform = glGetUniformLocation(shaderProgramObject_terrain, "iResolution");
        timeUniform = glGetUniformLocation(shaderProgramObject_terrain, "iTime");
        enableCausticUniform = glGetUniformLocation(shaderProgramObject_terrain, "enableCaustic");

        // // // //if (loadHeightMapData("./src/04_Assets/01_Textures/Terrain/HeightMap/heightmap.bmp") == FALSE)
        // // // //     if (loadHeightMapData("Height-Map.bmp") == FALSE)
        // // // //    return -7;

            // if (loadHeightMapData("Height-Map.bmp") == FALSE)
        if (loadHeightMapDataOptimized("./assets/textures/Terrain/HeightMap/heightmap.bmp") == FALSE)
        {
            return -8;
        }

        // // vao and vbo related code
        glGenVertexArrays(1, &vao_terrain);
        glBindVertexArray(vao_terrain);

        // vbo for position
        glGenBuffers(1, &vbo_Position_terrain);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_Position_terrain);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(terrainVertexData), terrainVertexData, GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.at(0), GL_STATIC_DRAW);
        glVertexAttribPointer(VERTEX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(VERTEX_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // // vbo for color
        glGenBuffers(1, &vbo_TexCoord_terrain);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_TexCoord_terrain);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(terrainTexCoordData), terrainTexCoordData, GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * texcoords.size(), &texcoords[0], GL_STATIC_DRAW);
        glVertexAttribPointer(VERTEX_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(VERTEX_ATTRIBUTE_TEXTURE0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // vbo for Normal
        //glGenBuffers(1, &vbo_Normal_terrain);
        //glBindBuffer(GL_ARRAY_BUFFER, vbo_Normal_terrain);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(terrainNormalData), terrainNormalData, GL_STATIC_DRAW);
        //glVertexAttribPointer(VERTEX_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        //glEnableVertexAttribArray(VERTEX_ATTRIBUTE_NORMAL);
        //glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &vbo_Indices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * indices.size(), &indices.at(0), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        if (LoadPNGImage(&texture_grass, "./assets/textures/Terrain/grass.bmp") == FALSE)
        {
            PrintLog("LoadGLTexture Failed for TEXTURE_GRASS..\n");
            return -2;
        }

        if (LoadPNGImage(&texture_r, "./assets/textures/Terrain/path5.bmp") == FALSE)
        {
            PrintLog("LoadGLTexture Failed for TEXTURE_R..\n");
            return -3;
        }

        if (LoadPNGImage(&texture_g, "./assets/textures/Terrain/grassFlowers.bmp") == FALSE)
        {
            PrintLog("LoadGLTexture Failed for TEXTURE_G..\n");
            return -4;
        }

        // // // // if (LoadPNGImage(&texture_b, "./src/04_Assets/01_Textures/Terrain/path4.bmp") == FALSE)
        // // // // {
        // // // //     PrintLog("LoadGLTexture Failed for TEXTURE_B..\n");
        // // // //     return -5;
        // // // // }

        if (LoadPNGImage(&texture_b, "./assets/textures/Terrain/path7.bmp") == FALSE)
        {
            PrintLog("LoadGLTexture Failed for TEXTURE_B..\n");
            return -5;
        }

        if (LoadPNGImage(&texture_blendMap, "./assets/textures/Terrain/blendMap.bmp") == FALSE)
        {
            PrintLog("LoadGLTexture Failed for TEXTURE_BLENDMAP..\n");
            return -6;
        }

        glEnable(GL_CLIP_DISTANCE0);

        return 0;
    }

    void renderTerrain()
    {

        GLfloat eqn[] = {0.0f, 1.0f, 0.0f, -8.5f};

        // use shader program obejct
        glUseProgram(shaderProgramObject_terrain);

        // Tranformations
        // translationMatrix = vmath::translate(0.0f, 0.0f, -5.0f); // glTranslatef() is replaced by this line

        glUniformMatrix4fv(modelMatrixUniform_terrain, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(viewMatrixUniform_terrain, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionMatrixUniform_terrain, 1, GL_FALSE, perspectiveProjectionMatrix);

         lightPosition[0] = objX;
         lightPosition[1] = objY;
         lightPosition[2] = objZ;

        glUniform3fv(lightPositionUniform_terrain, 1, lightPosition);
        glUniform3fv(lightDiffuseUniform_terrain, 1, lightDiffuse);
        glUniform1f(lightShinninessUniform_terrain, lightShinniness);
        glUniform1f(lightReflectivityUniform_terrain, lightReflectivity);

        glUniform1f(terrainAlphaUniform, alpha);

        glUniform4fv(terrainClipPlaneUniform, 1, eqn);
        // glEnable(GL_CLIP_PLANE0);

        // grass

        // glClipPlane(GL_CLIP_PLANE0, eqn);

        glActiveTexture(GL_TEXTURE0); //
        glBindTexture(GL_TEXTURE_2D, texture_grass);
        glUniform1i(textureSamplerUniform_terrain, 0); //

        //
        glActiveTexture(GL_TEXTURE1); //
        glBindTexture(GL_TEXTURE_2D, texture_r);
        glUniform1i(rTextureSamplerUniform_terrain, 1); //

        glActiveTexture(GL_TEXTURE2); //
        glBindTexture(GL_TEXTURE_2D, texture_g);
        glUniform1i(gTextureSamplerUniform_terrain, 2); //

        glActiveTexture(GL_TEXTURE3); //
        glBindTexture(GL_TEXTURE_2D, texture_b);
        glUniform1i(bTextureSamplerUniform_terrain, 3); //

        glActiveTexture(GL_TEXTURE4); //
        glBindTexture(GL_TEXTURE_2D, texture_blendMap);
        glUniform1i(blendMapTextureSamplerUniform_terrain, 4); //

        // glBindVertexArray(vao_terrain);
        // // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size());
        // glBindVertexArray(0);

        // glBindVertexArray(vao_terrain);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Indices);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        // glBindVertexArray(0);

        glBindVertexArray(vao_terrain);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        for (unsigned strip = 0; strip < numStrips; strip++)
        {
            glDrawElements(GL_TRIANGLE_STRIP,                                           // primitive type
                           numTrisPerStrip + 2,                                         // number of indices to render
                           GL_UNSIGNED_INT,                                             // index data type
                           (void *)(sizeof(unsigned) * (numTrisPerStrip + 2) * strip)); // offset to starting index
        }
        glBindVertexArray(0);

        // unuse the shader program object
        glUseProgram(0);
    }

    void renderTerrain(BOOL Up)
    {
        GLfloat eqn[] = {0.0f, -1.0f, 0.0f, 8.5f};
        //if (Up == TRUE)
        {
            eqn[1] = 1.0f;
            eqn[3] = 0.0f;
        }
        // else
        // {
        //     eqn[1] = -1.0f;
        //     eqn[3] = 0.0f;
        // }

        // use shader program obejct
        glUseProgram(shaderProgramObject_terrain);

        // Tranformations
        // translationMatrix = vmath::translate(0.0f, 0.0f, -5.0f); // glTranslatef() is replaced by this line

        glUniformMatrix4fv(modelMatrixUniform_terrain, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(viewMatrixUniform_terrain, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionMatrixUniform_terrain, 1, GL_FALSE, perspectiveProjectionMatrix);

        // lightPosition[0] = objX;
        // lightPosition[1] = objY;
        // lightPosition[2] = objZ;

        glUniform3fv(lightPositionUniform_terrain, 1, lightPosition);
        glUniform3fv(lightDiffuseUniform_terrain, 1, lightDiffuse);
        glUniform1f(lightShinninessUniform_terrain, lightShinniness);
        glUniform1f(lightReflectivityUniform_terrain, lightReflectivity);

        glUniform1f(terrainAlphaUniform, alpha);

        glUniform4fv(lightPositionUniform_terrain, 1, lightPosition);
        glUniform2f(resolutionUniform, (GLfloat)1920, (GLfloat)1080);
        glUniform1f(timeUniform, timeFactor);
        //glUniform4fv(terrainClipPlaneUniform, 1, eqn);


        glUniform1i(enableCausticUniform, 0);
        

        // glEnable(GL_CLIP_PLANE0);

        // grass

        glActiveTexture(GL_TEXTURE0); //
        glBindTexture(GL_TEXTURE_2D, texture_grass);
        glUniform1i(textureSamplerUniform_terrain, 0); //

        //
        glActiveTexture(GL_TEXTURE1); //
        glBindTexture(GL_TEXTURE_2D, texture_r);
        glUniform1i(rTextureSamplerUniform_terrain, 1); //

        glActiveTexture(GL_TEXTURE2); //
        glBindTexture(GL_TEXTURE_2D, texture_g);
        glUniform1i(gTextureSamplerUniform_terrain, 2); //

        glActiveTexture(GL_TEXTURE3); //
        glBindTexture(GL_TEXTURE_2D, texture_b);
        glUniform1i(bTextureSamplerUniform_terrain, 3); //

        glActiveTexture(GL_TEXTURE4); //
        glBindTexture(GL_TEXTURE_2D, texture_blendMap);
        glUniform1i(blendMapTextureSamplerUniform_terrain, 4); //

        // glBindVertexArray(vao_terrain);
        // // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size());
        // glBindVertexArray(0);

        // glBindVertexArray(vao_terrain);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Indices);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        // glBindVertexArray(0);

        glUniform1f(glGetUniformLocation(shaderProgramObject_terrain, "temp"), objY); //

        glBindVertexArray(vao_terrain);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        for (unsigned strip = 0; strip < numStrips; strip++)
        {
            glDrawElements(GL_TRIANGLE_STRIP,                                           // primitive type
                           numTrisPerStrip + 2,                                         // number of indices to render
                           GL_UNSIGNED_INT,                                             // index data type
                           (void *)(sizeof(unsigned) * (numTrisPerStrip + 2) * strip)); // offset to starting index
        }
        glBindVertexArray(0);

        // unuse the shader program object
        glUseProgram(0);
        timeFactor += 0.05f;
    }

    void uninitializeTerrain(void)
    {
        // code:

        PrintLog(" ### Inside uninitializeTerrain() . ###\n");

        if (texture_grass)
        {
            glDeleteTextures(1, &texture_grass);
            texture_grass = 0;
        }

        if (texture_r)
        {
            glDeleteTextures(1, &texture_r);
            texture_r = 0;
        }

        if (texture_g)
        {
            glDeleteTextures(1, &texture_g);
            texture_g = 0;
        }

        if (texture_b)
        {
            glDeleteTextures(1, &texture_b);
            texture_b = 0;
        }

        if (texture_blendMap)
        {
            glDeleteTextures(1, &texture_blendMap);
            texture_blendMap = 0;
        }

        // deletion of vbo_TexCoord
        if (vbo_TexCoord_terrain)
        {
            glDeleteBuffers(1, &vbo_TexCoord_terrain);
            vbo_TexCoord_terrain = 0;
        }

        // deletion of vbo_Position
        if (vbo_Position_terrain)
        {
            glDeleteBuffers(1, &vbo_Position_terrain);
            vbo_Position_terrain = 0;
        }

        // deletion of vao
        if (vao_terrain)
        {
            glDeleteVertexArrays(1, &vao_terrain);
            vao_terrain = 0;
        }

        UninitializeShaders(shaderProgramObject_terrain);
    }

    int getVertexCount()
    {
        return (SIZE_OF_TERRAIN_MAP);
    }

    float *getVertexData()
    {
        return (terrainVertexData);
    }

    float barryCentric(float *p1, float *p2, float *p3, float *pos)
    {
        float det = (p2[2] - p3[2]) * (p1[0] - p3[0]) + (p3[0] - p2[0]) * (p1[2] - p3[2]);
        float l1 = ((p2[2] - p3[2]) * (pos[0] - p3[0]) + (p3[0] - p2[0]) * (pos[1] - p3[2])) / det;
        float l2 = ((p3[2] - p1[2]) * (pos[0] - p3[0]) + (p1[0] - p3[0]) * (pos[1] - p3[2])) / det;
        float l3 = 1.0f - l1 - l2;
        return l1 * p1[1] + l2 * p2[1] + l3 * p3[1];
    }

    float getHeightOfTerrain(float worldX, float worldY, float worldZ)
    {
        // float terrainX = worldX - this->terrainPosition[0];
        float terrainX = worldX;
        float terrainZ = worldZ;
        // float terrainZ = worldZ - this->terrainPosition[2];
        // float gridSquareSize = (HEIGHT_MAP_SCALE + TERRAIN_MAP_X) / TERRAIN_MAP_X - 1;
        float gridSquareSize = HEIGHT_MAP_SCALE * 1.9f;
        // float gridSquareSize = TERRAIN_MAP_X / (float)((/*Number Of Heights*/ TERRAIN_MAP_X + TERRAIN_MAP_X) - 1.0f);
        // PrintLog(" terrainX :%f\n terrainZ:%f\n gridSquareSize:%f\n", terrainX, terrainZ, gridSquareSize);

        int gridX = floor(terrainX / gridSquareSize);
        int gridZ = floor(terrainZ / gridSquareSize);

        // if (gridX >= TERRAIN_MAP_X - 1 || gridZ <= -TERRAIN_MAP_Z - 1 || gridX < 0 || gridZ > 0)
        // {
        //     PrintLog("Position is Not On Terrain\n");
        //     return 0;
        // }

        float xCoord = ((int)terrainX % (int)gridSquareSize) / gridSquareSize;
        float zCoord = ((int)terrainZ % (int)gridSquareSize) / gridSquareSize;
        if (gridZ < 0)
        {
            gridZ = -gridZ;
        }
        float temp[2] = {xCoord, zCoord};

        // float ret = terrainHeightMap[gridX][gridZ][1];
        float ret = terrainHeightMap[gridZ][gridX][1];
        if (xCoord <= (1 - zCoord))
        {
            // ret = barryCentric(terrainHeightMap[gridX ][gridZ], terrainHeightMap[gridX + 1][gridZ], terrainHeightMap[gridX][gridZ + 1], temp);
        }
        else
        {
            // ret = barryCentric(terrainHeightMap[gridX + 1][gridZ], terrainHeightMap[gridX + 1][gridZ + 1], terrainHeightMap[gridX][gridZ + 1], temp);
        }
        return (ret - 128.0f);
    }

    BOOLEAN loadHeightMapData(char *filename)
    {
        // VARIABLE DECLARTIONS
        int width, height, channels_in_file;
        int heightMultiplier = 1.0, terrain_counter = 0, terrain_texcoord_counter = 0;

        // CODE
        image_data = stbi_load(filename, &width, &height, &channels_in_file, 0);

        if (image_data == NULL)
        {
            PrintLog("File %s loading Failed...\n", filename);
            return FALSE;
        }

        // FETCHING DATA FROM HEIGHT MAP
        for (int z = 0; z < TERRAIN_MAP_Z; z++)
        {
            for (int x = 0; x < TERRAIN_MAP_X; x++)
            {
                terrainHeightMap[x][z][0] = float(x) * HEIGHT_MAP_SCALE;
                terrainHeightMap[x][z][1] = (image_data[(x + z * TERRAIN_MAP_Z) * 3] * heightMultiplier);
                // terrainHeightMap[x][z][1] = (image_data[(x + z * TERRAIN_MAP_Z) * 3] * heightMultiplier) - 128.0f;
                terrainHeightMap[x][z][2] = -float(z) * HEIGHT_MAP_SCALE;
                // PrintLog("ret :%f\n", terrainHeightMap[x][z][1]);

                terrainHeightMapTexCoord[x][z][0] = (float)x / (float)(TERRAIN_MAP_X - 1);
                terrainHeightMapTexCoord[x][z][1] = (float)z / ((float)TERRAIN_MAP_Z - 1);
            }
        }

        // Calculate Normal
        for (int z = 0; z < TERRAIN_MAP_Z; z++)
        {
            for (int x = 0; x < TERRAIN_MAP_X; x++)
            {
                float heightL = terrainHeightMap[x - 1][z][1];
                float heightR = terrainHeightMap[x + 1][z][1];
                float heightD = terrainHeightMap[x][z - 1][1];
                float heightU = terrainHeightMap[x][z + 1][1];
                terrainHeightMapNormals[x][z][0] = heightL - heightR;
                terrainHeightMapNormals[x][z][1] = 2.0f;
                terrainHeightMapNormals[x][z][2] = heightD - heightU;
            }
        }

        // FREE IMAGE DATA
        stbi_image_free(image_data);

        // GANERATE MESH
        for (int z = 0; z < (TERRAIN_MAP_Z - 1); z++)
        {
            for (int x = 0; x < (TERRAIN_MAP_X - 1); x++)
            {
                if (terrain_counter < SIZE_OF_TERRAIN_MAP)
                {
                    // TRIANGLE 1
                    terrainTexCoordData[terrain_texcoord_counter++] = terrainHeightMapTexCoord[x][z][0];
                    terrainNormalData[terrain_counter] = terrainHeightMapNormals[x][z][0];
                    terrainVertexData[terrain_counter++] = terrainHeightMap[x][z][0];

                    terrainTexCoordData[terrain_texcoord_counter++] = terrainHeightMapTexCoord[x][z][1];
                    terrainNormalData[terrain_counter] = terrainHeightMapNormals[x][z][1];
                    terrainVertexData[terrain_counter++] = terrainHeightMap[x][z][1];

                    terrainNormalData[terrain_counter] = terrainHeightMapNormals[x][z][2];
                    terrainVertexData[terrain_counter++] = terrainHeightMap[x][z][2];

                    // TRIANGLE 2
                    terrainTexCoordData[terrain_texcoord_counter++] = terrainHeightMapTexCoord[x + 1][z][0];
                    terrainNormalData[terrain_counter] = terrainHeightMapNormals[x + 1][z][0];
                    terrainVertexData[terrain_counter++] = terrainHeightMap[x + 1][z][0];

                    terrainTexCoordData[terrain_texcoord_counter++] = terrainHeightMapTexCoord[x + 1][z][1];
                    terrainNormalData[terrain_counter] = terrainHeightMapNormals[x + 1][z][0];
                    terrainVertexData[terrain_counter++] = terrainHeightMap[x + 1][z][1];

                    terrainNormalData[terrain_counter] = terrainHeightMapNormals[x + 1][z][0];
                    terrainVertexData[terrain_counter++] = terrainHeightMap[x + 1][z][2];

                    // TRIANGLE 3
                    terrainTexCoordData[terrain_texcoord_counter++] = terrainHeightMapTexCoord[x][z + 1][0];
                    terrainNormalData[terrain_counter] = terrainHeightMapNormals[x][z + 1][0];
                    terrainVertexData[terrain_counter++] = terrainHeightMap[x][z + 1][0];

                    terrainTexCoordData[terrain_texcoord_counter++] = terrainHeightMapTexCoord[x][z + 1][1];
                    terrainNormalData[terrain_counter] = terrainHeightMapNormals[x][z + 1][0];
                    terrainVertexData[terrain_counter++] = terrainHeightMap[x][z + 1][1];

                    terrainNormalData[terrain_counter] = terrainHeightMapNormals[x][z + 1][0];
                    terrainVertexData[terrain_counter++] = terrainHeightMap[x][z + 1][2];

                    // TRIANGLE 4
                    terrainTexCoordData[terrain_texcoord_counter++] = terrainHeightMapTexCoord[x + 1][z + 1][0];
                    terrainNormalData[terrain_counter] = terrainHeightMapNormals[x + 1][z + 1][0];
                    terrainVertexData[terrain_counter++] = terrainHeightMap[x + 1][z + 1][0];

                    terrainTexCoordData[terrain_texcoord_counter++] = terrainHeightMapTexCoord[x + 1][z + 1][1];
                    terrainNormalData[terrain_counter] = terrainHeightMapNormals[x + 1][z + 1][0];
                    terrainVertexData[terrain_counter++] = terrainHeightMap[x + 1][z + 1][1];

                    terrainNormalData[terrain_counter] = terrainHeightMapNormals[x + 1][z + 1][0];
                    terrainVertexData[terrain_counter++] = terrainHeightMap[x + 1][z + 1][2];
                }
            }
        }

        return TRUE;
    }

    int width, height, channels_in_file;
    BOOLEAN loadHeightMapDataOptimized(char *filename)
    {
        // VARIABLE DECLARTIONS
        int heightMultiplier = 1.0, terrain_counter = 0, terrain_texcoord_counter = 0;

        // CODE
        // stbi_set_flip_vertically_on_load(true);

        image_data = stbi_load(filename, &width, &height, &channels_in_file, 0);
        if (image_data)
        {
            PrintLog("File %s loading Succesded...\n", filename);
        }
        else
        {
            PrintLog("File %s loading Failed...\n", filename);
            return FALSE;
        }

        float yScale = 70.0f / 256.0f, yShift = 16.0f;
        unsigned bytePerPixel = channels_in_file;
        int rez = 1;

        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                unsigned char *pixelOffset = image_data + (j + width * i) * bytePerPixel;
                unsigned char y = pixelOffset[0];

                // vertex
                vertices.push_back(-height / 2.0f + height * i / (float)height); // vx
                vertices.push_back((int)y * yScale - yShift);                    // vy
                vertices.push_back(-width / 2.0f + width * j / (float)width);    // vz

                std::vector<float> vec;
                vec.push_back(-height / 2.0f + height * i / (float)height);
                vec.push_back((int)y * yScale - yShift);
                vec.push_back(-width / 2.0f + width * j / (float)width);

                verticesThree.push_back(vec);

                texcoords.push_back((float)j / (float)(width - 1));
                texcoords.push_back((float)i / (float)(height - 1));

                // terrainHeightMapTexCoord[x][z][0] = (float)x / (float)(TERRAIN_MAP_X - 1);
                // terrainHeightMapTexCoord[x][z][1] = (float)z / ((float)TERRAIN_MAP_Z - 1);
            }
        }
        // Create Mesh
        for (unsigned i = 0; i < height - 1; i += rez)
        {
            for (unsigned j = 0; j < width; j += rez)
            {
                for (unsigned k = 0; k < 2; k++)
                {
                    indices.push_back(j + width * (i + k * rez));
                }
            }
        }

        numStrips = (height - 1) / rez;
        numTrisPerStrip = (width / rez) * 2 - 2;

        return TRUE;
    }
};
