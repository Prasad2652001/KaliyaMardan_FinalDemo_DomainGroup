#pragma once
#include "../../utils/common.h"

struct Character
{
    GLuint textureID;
    vmath::vec2 size;
    vmath::vec2 bearing;
    GLuint advance;
} characters[128];

// render line of text
// -------------------
GLuint VAO, VBO;
GLuint gShaderProgramObjectFontRender, perspectiveProjectionMatrixUniform;

GLuint fontSize = 16;

int FontRendering::initFontRendering(void)
{
    char vshaderBuffer[128];
    char fshaderBuffer[128];
    char pathBuffer[128];
    DWORD nBufferLength = 128;
    DWORD dwRet;

    BOOL bResult;

    dwRet = GetCurrentDirectoryA(nBufferLength, pathBuffer);
    if (dwRet == 0)
    {
        PrintLog("GetCurrentDirectory failed (%d)\n", GetLastError());
        return 0;
    }
    if (dwRet > 512)
    {
        PrintLog("Buffer too small; need %d characters\n", dwRet);
        return 0;
    }

    strcpy(vshaderBuffer, pathBuffer);
    strcpy(fshaderBuffer, pathBuffer);
    strcat(vshaderBuffer, "\\Source\\Shaders\\fontRender\\vertex_shader.vert");
    strcat(fshaderBuffer, "\\Source\\Shaders\\fontRender\\fragment_shader.frag");

    // C2 : Creating shader object
    vertexShaderObject = CreateAndCompileShaderObjects(vshaderBuffer, VERTEX);
    if (vertexShaderObject == 0)
        return SHADER_OBJECT_ERROR;

    // Fragment shader source
    fragmentShaderObject = CreateAndCompileShaderObjects(fshaderBuffer, FRAGMENT);
    if (fragmentShaderObject == 0)
        return SHADER_OBJECT_ERROR;

    // SHADER PROGRAM OBJECT
    // D1 : Create shader program object
    gShaderProgramObjectFontRender = glCreateProgram();

    // D2 : Attach desired shaders to this shader prog object
    glAttachShader(
        gShaderProgramObjectFontRender, // Kunala chiktvaycha
        vertexShaderObject              // Kon chiktvaychay
    );

    glAttachShader(
        gShaderProgramObjectFontRender,
        fragmentShaderObject);

    // pre-link binding of shader program object with vertex shader position

    // link shader
    BOOL linkStatus = LinkShaderProgramObject(gShaderProgramObjectFontRender);
    if (linkStatus == FALSE)
        return SHADER_PROGRAM_OBJECT_ERROR;
    // get mvp uniform location

    perspectiveProjectionMatrixUniform = glGetUniformLocation(gShaderProgramObjectFontRender, "u_mvpMatrix");
    colorUniform = glGetUniformLocation(gShaderProgramObjectFontRender, "textColor");

    // Initialize the freetype lib
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        fprintf(gpFile, "ERROR:FREETYPE: coubld not init freetype lib \n");
        return (-1);
    }
    FT_Face face;
    char str[255];
    getCurrentDirPath("\\Resources\\Fonts\\arial.ttf", str);
    PrintLog("san: %s \n", str);
    fflush(gpFile);
    if (FT_New_Face(ft, str, 0, &face))
    {
        fprintf(gpFile, "ERROR:FREETYPE: coubld not load font\n");
        return (-1);
    }

    // Each of the freetype function returns non-zero number if there is error
    // Once we have loaded the face , we shuld define the pixel font size we had
    // like to extract from this face
    // 0 -> set the width as per given font height
    // 48 -> set the font height to 48
    FT_Set_Pixel_Sizes(face, 0, fontSize);

    // Now we have to select the glyph/character by calling FT_Load_Char
    //  face -> contain 128 charecter glyph for specified fond
    //  X -> take char 'X' fron the given font face
    //  FT_LOAD_RENDER ->one of the loading flags, we telss FreeType to create an 8 bit
    //                   grayscale bitmap for us that we can access via face->glyph->bitmap
    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
    {
        fprintf(gpFile, "ERROR:FREETYPE: failed to laod character \n");
        return (-1);
    }
    // In FreeType each glyph is of different dimention, eg, '.' is smalller in dimention.
    //'X' its dimention is bigger then the '.' glyph
    // For that FreeType loads severval marics that specify how large each character should be
    // and how to properly position them.
    //  properties of glyphs
    // width, height, xMin, xMax, yMin, yMax, bearingX, bearingY, advance, origin
    // ho to acces them, eg, face->glyph->bitmap.width

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // dissable byte-alingment restriction

    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            fprintf(gpFile, "ERROR:FREETYPE: failed to laod character %c\n", c);
            continue;
        }

        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                     face->glyph->bitmap.width,
                     face->glyph->bitmap.rows,
                     0,
                     GL_RED,
                     GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);

        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // now store character for feature use
        characters[c].textureID = texture;
        characters[c].size[0] = face->glyph->bitmap.width;
        characters[c].size[1] = face->glyph->bitmap.rows;
        characters[c].bearing[0] = face->glyph->bitmap_left;
        characters[c].bearing[1] = face->glyph->bitmap_top;
        characters[c].advance = face->glyph->advance.x;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return (0);
}

void RenderText(std::string text, float x, float y, float scale, vmath::vec3 color, mat4 projectionMatrix)
{
    // activate corresponding render state
    glUseProgram(gShaderProgramObjectFontRender);
    glUniformMatrix4fv(perspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);
    glUniform3f(glGetUniformLocation(gShaderProgramObjectFontRender, "textColor"), color[0], color[1], color[2]);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.bearing[0] * scale;
        float ypos = y - (ch.size[1] - ch.bearing[1]) * scale;

        float w = ch.size[0] * scale;
        float h = ch.size[1] * scale;
        // update VBO for each character
        float vertices[6][4] = {
            {xpos, ypos + h, 0.0f, 0.0f},
            {xpos, ypos, 0.0f, 1.0f},
            {xpos + w, ypos, 1.0f, 1.0f},

            {xpos, ypos + h, 0.0f, 0.0f},
            {xpos + w, ypos, 1.0f, 1.0f},
            {xpos + w, ypos + h, 1.0f, 0.0f}};
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void FontRendering::displayHelp(mat4 modelMatrix, mat4 viewMatrix, mat4 projectionMatrix)
{
    RenderText("This is sample text", 25.0f, 25.0f, 1.0f, vmath::vec3(0.5, 0.8f, 0.2f), projectionMatrix * viewMatrix * modelMatrix);
    RenderText("(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, vmath::vec3(0.3, 0.7f, 0.9f), projectionMatrix * viewMatrix * modelMatrix);
}
