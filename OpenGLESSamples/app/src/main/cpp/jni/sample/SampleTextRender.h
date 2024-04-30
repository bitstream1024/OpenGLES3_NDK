//
// Created by bitstream1024 on 2022/7/5.
//

#pragma once

#include "ft2build.h"
#include "freetype/ftglyph.h"
#include "SampleBase.h"

#include <unordered_map>

struct Character {
    GLuint TextureID;   // 字形纹理 ID
    glm::ivec2 Size;    // 字形大小
    glm::ivec2 Bearing; // 字形基于集线和起点的位置
    GLuint Advance;     // 起点到下一个字形起点的距离
};

using ASCIICharMap = std::unordered_map <GLchar, Character>;

class SampleTextRender : public SampleBase
{
public:
    SampleTextRender ();
    ~SampleTextRender();
    int Init();
    int Draw();
    void UnInit();

private:
    // 为 128 个 ASCII 字符创建字形并保存到 map 中
    int createASCIICharacters ();
    int initBuffer();
    int renderText(const std::string& strText, const float &posX, const float &posY, const float &scale,
                   const glm::vec3 &color, const LPKitRect pRenderWindow);

private:
    ASCIICharMap *m_pASCIICharMap;
    ShaderHelper *m_pShaderHelper;

    GLuint       m_TextVBO[2]{};
};



