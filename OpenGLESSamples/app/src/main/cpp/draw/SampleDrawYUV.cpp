//
// Created by chauncy_wang on 2020/9/23.
//

#include <MyDefineUtils.h>
#include <vector>
#include "SampleDrawYUV.h"
#include "DrawHelper.h"

SampleDrawYUV::SampleDrawYUV() {
    mTextureLumin = GL_NONE;
    mTextureAlpha = GL_NONE;
    m_pShaderHelper = nullptr;
    OpenImageHelper::ZeroMyImageInfo(&m_YUVImage);
}

SampleDrawYUV::~SampleDrawYUV() {
    SafeDelete(m_pShaderHelper)
    OpenImageHelper::FreeMyImageInfo(&m_YUVImage);
}

RESULT SampleDrawYUV::OnDrawFrame() {
    return 0;
}

RESULT SampleDrawYUV::SetImageYuvResource(MyImageInfo *const pSrcImage) {
    LOGD("SampleDrawYUV::SetImageYuvResource");
    if (nullptr == pSrcImage || nullptr == pSrcImage->buffer[0]) {
        LOGE("SampleDrawYUV::SetImageYuvResource pSrcImage error");
        return ERROR_IMAGE;
    }
    m_YUVImage.width = pSrcImage->width;
    m_YUVImage.height = pSrcImage->height;
    m_YUVImage.format = pSrcImage->format;
    memcpy(m_YUVImage.channel, pSrcImage->channel, 4 * sizeof(int));
    OpenImageHelper::CopyMyImageInfo(&m_YUVImage, pSrcImage);
    return 0;
}




RESULT SampleDrawYUV::createShader() {
    LOGD("SampleDrawYUV::createShader");
    RESULT retCode = ERROR_OK;
    do {
        if (!m_pShaderHelper) {
            m_pShaderHelper = new ShaderHelper (yuv_vertex_shader, yuv_fragment_shader);
        }
        if (!m_pShaderHelper || ERROR_OK != m_pShaderHelper->getShaderHelperState()) {
            LOGE("SampleDrawYUV::createShader error");
            retCode = m_pShaderHelper->getShaderHelperState();
            break;
        }
    } while(false);

    return retCode;
}

void SampleDrawYUV::destroyShader() {
    LOGD("SampleDrawYUV::destroyShader");
    SafeDelete(m_pShaderHelper)
}

RESULT SampleDrawYUV::createGLBuffer() {
    LOGD("SampleDrawYUV::createGLBuffer");

    unsigned int srcWidth = m_YUVImage.width;
    unsigned int srcHeight = m_YUVImage.height;

    // create Texture Buffer Object
    const GLenum TEXTURE_TARGET = GL_TEXTURE_2D;
    DrawHelper::GetOneTexture(TEXTURE_TARGET, &mTextureLumin);
    glBindTexture(TEXTURE_TARGET, mTextureLumin);
    glTexImage2D(TEXTURE_TARGET, 0, GL_LUMINANCE, srcWidth, srcHeight, 0, GL_LUMINANCE,
            GL_UNSIGNED_BYTE, m_YUVImage.buffer[0]);
    DrawHelper::CheckGLError("SampleDrawYUV::createGLBuffer glTexImage2D mTextureLumin");
    glBindTexture(TEXTURE_TARGET, GL_NONE);

    DrawHelper::GetOneTexture(TEXTURE_TARGET, &mTextureAlpha);
    glBindTexture(TEXTURE_TARGET, mTextureAlpha);
    glTexImage2D(TEXTURE_TARGET, 0, GL_LUMINANCE_ALPHA, srcWidth>>1, srcHeight>>1,
            0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, m_YUVImage.buffer[1]);
    DrawHelper::CheckGLError("SampleDrawYUV::createGLBuffer glTexImage2D mTextureAlpha");
    glBindTexture(TEXTURE_TARGET, GL_NONE);


    const std::vector<GLfloat> vertex {
        -1.f, -1.f, 0.f,
         1.f, -1.f, 0.f,
         1.f,  1.f, 0.f,
        -1.f,  1.f, 0.f
    };
    const std::vector <GLfloat> texcoords {
        0.f, 0.f,
        1.f, 0.f,
        1.f, 1.f,
        0.f, 1.f
    };
    const std::vector<GLuint> indices {0, 1, 2, 0, 2, 3};

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    GLuint vbo[3] = {GL_NONE};
    glGenBuffers(3, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex.size(), &vertex[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    DrawHelper::CheckGLError("SampleDrawYUV::createGLBuffer vbo[0]");

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * texcoords.size(), &texcoords[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    DrawHelper::CheckGLError("SampleDrawYUV::createGLBuffer vbo[1]");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);
    DrawHelper::CheckGLError("SampleDrawYUV::createGLBuffer vbo[2]");

    glBindVertexArray(GL_NONE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
    DrawHelper::CheckGLError("SampleDrawYUV::createGLBuffer GL_NONE");

    SafeDeleteGLBuffers(sizeof(vbo)/ sizeof(GLuint), vbo)

    return ERROR_OK;
}

void SampleDrawYUV::destroyGLBuffer() {
    LOGD("SampleDrawYUV::destroyGLBuffer");
    SafeDeleteGLArrays(1, &m_VAO)
}

void SampleDrawYUV::initMVPMatrix() {

    LOGD("SampleDrawYUV::initMVPMatrix");
    m_Model = glm::mat4 (1.f);
    m_View = glm::mat4 (1.f);
    m_Projection = glm::mat4 (1.f);
}
