//
// Created by bitstream1024 on 2022/7/5.
//

#include "SampleTextRender.h"

#include <string>
#include <KitCommonDefine.h>
#include <DrawHelper.h>

static const std::string text_vertex_shader = "#version 300 es\n"
                                              "layout (location = 0) in vec4 vertex;\n"
                                              "out vec2 TexCoord;\n"
                                              "uniform mat4 MVP;\n"
                                              "\n"
                                              "void main()\n"
                                              "{\n"
                                              "    gl_Position = MVP * vec4(vertex.xy, 0.f, 1.f);\n"
                                              "    TexCoord = vertex.zw;\n"
                                              "}";

static const std::string text_fragment_shader = "#version 300 es\n"
                                                "precision mediump float;\n"
                                                "\n"
                                                "out vec4 FragColor;\n"
                                                "in vec2 TexCoord;\n"
                                                "uniform vec3 textColor;\n"
                                                "uniform sampler2D screenTexture;\n"
                                                "\n"
                                                "void main()\n"
                                                "{\n"
                                                "    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(screenTexture, TexCoord).r);\n"
                                                "    FragColor = sampled * vec4 (textColor, 1.0);\n"
                                                "}";

SampleTextRender::SampleTextRender():m_pASCIICharMap(nullptr), m_pShaderHelper(nullptr)
{

}

SampleTextRender::~SampleTextRender()
= default;

int SampleTextRender::Init()
{
    if (nullptr == m_pShaderHelper)
        m_pShaderHelper = new ShaderHelper(text_vertex_shader.c_str(), text_fragment_shader.c_str());
    if (m_pShaderHelper->getShaderHelperState() != NONE_ERROR)
    {
        LOGE("ScreenRect::CreateFullRectBuffer m_pFullRectShader shaderState = %d", m_pShaderHelper->getShaderHelperState());
        return ERROR_GL_STATE;
    }

    createASCIICharacters();
    initBuffer();

    return NONE_ERROR;
}

void SampleTextRender::UnInit()
{
    SafeDeleteGLArrays(1, &m_VAO)
    SafeDeleteGLBuffers(1, &m_VBO)
    SafeDelete(m_pASCIICharMap);
    SafeDelete(m_pShaderHelper);
}

int SampleTextRender::Draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    // Set OpenGL options
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::string strText("Hello, this is bitstream1024, what's your name?");
    KitRect rect = {viewport[0], viewport[1], viewport[2], viewport[3]};
    int ret = renderText(strText, -0.9f, 0.7f, 1.3f, glm::vec3(1.f, 0.f, 1.f), &rect);

    // Set OpenGL options
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    ++m_FrameID;

    return ret;
}

int SampleTextRender::createASCIICharacters()
{
    AUTO_COUNT_TIME_COST("createASCIICharacters");

    DrawHelper::CheckGLError("SampleTextRender::createASCIICharacters begin");

    if (nullptr == m_pASCIICharMap)
        m_pASCIICharMap = new ASCIICharMap();

    int nRet = NONE_ERROR;
    FT_Library pFreeTypeLib = nullptr;
    FT_Face pFreeTypeFace = nullptr;
    do
    {
        nRet = FT_Init_FreeType(&pFreeTypeLib);
        if (NONE_ERROR != nRet)
        {
            LOGE("SampleTextRender::createASCIICharacters FT_Init_FreeType failed, nRet = %d\n", nRet);
            break;
        }

        nRet = FT_New_Face(pFreeTypeLib, "/sdcard/OpenGLESTest/times.ttf", 0, &pFreeTypeFace);
        if (NONE_ERROR != nRet)
        {
            LOGE("SampleTextRender::createASCIICharacters FT_New_Face failed, nRet = %d\n", nRet);
            break;
        }

        FT_Set_Pixel_Sizes(pFreeTypeFace, 0, 48);

        // 禁用 byte-alignment 限制
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (GLubyte c = 0; c < 128; c++)
        {
            nRet = FT_Load_Char(pFreeTypeFace, c, FT_LOAD_RENDER);
            if (nRet)
            {
                LOGE("SampleTextRender::createASCIICharacters FT_Load_Char(%c) failed, nRet = %d\n", c, nRet);
                break;
            }

            // generate character texture
            GLuint texture = GL_NONE;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            DrawHelper::CheckGLError("SampleTextRender::createASCIICharacters glBindTexture");
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, pFreeTypeFace->glyph->bitmap.width, pFreeTypeFace->glyph->bitmap.rows,
                         0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pFreeTypeFace->glyph->bitmap.buffer);
            DrawHelper::CheckGLError("SampleTextRender::createASCIICharacters glTexImage2D");
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, GL_NONE);
            DrawHelper::CheckGLError("SampleTextRender::createASCIICharacters glBindTexture GL_NONE");

            // save character texture to map
            Character character {texture,
                                 glm::ivec2(pFreeTypeFace->glyph->bitmap.width, pFreeTypeFace->glyph->bitmap.rows),
                                 glm::ivec2(pFreeTypeFace->glyph->bitmap_left, pFreeTypeFace->glyph->bitmap_top),
                                 static_cast<GLuint>(pFreeTypeFace->glyph->advance.x)};
            m_pASCIICharMap->insert(std::pair<GLchar, Character>(c, character));
        }
    } while (false);

    FT_Done_Face(pFreeTypeFace);
    FT_Done_FreeType(pFreeTypeLib);

    return NONE_ERROR;
}

int SampleTextRender::initBuffer()
{
    DrawHelper::CheckGLError("SampleTextRender::initBuffer begin");
    glGenVertexArrays(1, &m_VAO);
    DrawHelper::CheckGLError("SampleTextRender::initBuffer glGenVertexArrays");
    glGenBuffers(1, &m_VBO);
    DrawHelper::CheckGLError("SampleTextRender::initBuffer glGenBuffers");
    if (GL_NONE != m_VAO && GL_NONE != m_VBO)
    {
        glBindVertexArray(m_VAO);
        DrawHelper::CheckGLError("SampleTextRender::initBuffer glBindVertexArray");
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        // glBufferData 使用 nullptr 表示只分配内存暂时不往里面塞数据，GL_DYNAMIC_DRAW 表示数据会被频繁修改多次使用，GL_STATIC_DRAW 表示一次修改多次使用
        DrawHelper::CheckGLError("SampleTextRender::initBuffer glBindBuffer");
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
        DrawHelper::CheckGLError("SampleTextRender::initBuffer glBufferData");

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
        DrawHelper::CheckGLError("SampleTextRender::initBuffer glVertexAttribPointer 0");

        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
        glBindVertexArray(GL_NONE);
    }

    return NONE_ERROR;
}

int SampleTextRender::renderText(const std::string& strText, const float &posX, const float &posY,
                                 const float &scale, const glm::vec3 &color, const LPKitRect pRenderWindow)
{
    DrawHelper::CheckGLError("SampleTextRender::renderText begin");

    if (nullptr == m_pShaderHelper || NONE_ERROR != m_pShaderHelper->getShaderHelperState())
        return ERROR_GL_STATE;

    m_pShaderHelper->use();

    glm::mat4 mvp = glm::mat4(1.f);// * glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    m_pShaderHelper->setMat4("MVP", mvp);
    //LOGD("SampleTextRender::renderText color: %f, %f, %f", color.x, color.y, color.z);
    m_pShaderHelper->setVec3f("textColor", color.x, color.y, color.z);

    // set texture sampler
    glActiveTexture(GL_TEXTURE0);
    m_pShaderHelper->setInt("screenTexture", 0);
    glBindVertexArray(m_VAO);

    float xCoord = posX;
    int winWidth = pRenderWindow->right - pRenderWindow->left;
    int winHeight = pRenderWindow->bottom - pRenderWindow->top;
    for (auto c : strText)
    {
        Character ch = m_pASCIICharMap->find(c)->second;

        // 归一化坐标和宽高
        GLfloat xpos = xCoord + ch.Bearing.x * scale / winWidth;
        GLfloat ypos = posY - (ch.Size.y - ch.Bearing.y) * scale / winHeight;
        GLfloat w = ch.Size.x * scale / winWidth;
        GLfloat h = ch.Size.y * scale / winHeight;

        LOGD("SampleTextRender::renderText (c,textid,xpos,ypos,w,h)=(%c, %d, %f, %f, %f, %f)",
             c, ch.TextureID, xpos, ypos, w, h);
        // 当前字符顶点
        GLfloat vertices[6][4] = {
                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos,     ypos,       0.0, 1.0 },
                { xpos + w, ypos,       1.0, 1.0 },

                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos + w, ypos,       1.0, 1.0 },
                { xpos + w, ypos + h,   1.0, 0.0 }
        };

        // 绑定当前字符纹理
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // 绑定当前字符的 VBO
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0][0]);
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

        // 绘制字形方块
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // update x coordinate, unit is 1/64 pixel
        xCoord = xCoord + ((ch.Advance >> 6) * scale)/winWidth;
    }
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindVertexArray(GL_NONE);

#if 0
    KitRect rect = {viewport[0], viewport[1], viewport[2], viewport[3]};
    char szPath[MAX_PATH] {0};
    snprintf(szPath, MAX_PATH - 1, "/sdcard/OpenGLESTest/dump/test_%04d_%dx%d.png", m_FrameID, viewport[2], viewport[3]);
    std::string strPath(szPath);
    DrawHelper::SaveRenderImage(rect, GL_RGBA, strPath);
#endif // test

    DrawHelper::CheckGLError("SampleTextRender::renderText end");

    return NONE_ERROR;
}

