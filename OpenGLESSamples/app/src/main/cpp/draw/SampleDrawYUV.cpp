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
    initMVPMatrix ();
}

SampleDrawYUV::~SampleDrawYUV() {
    SafeDelete(m_pShaderHelper)
    OpenImageHelper::FreeMyImageInfo(&m_YUVImage);
}

RESULT SampleDrawYUV::OnDrawFrame() {
    LOGD("SampleDrawYUV::OnDrawFrame begin");
    GL_CHECK_ERROR("SampleDrawYUV::OnDrawFrame begin");

    m_pShaderHelper->use();
    GL_CHECK_ERROR("SampleDrawYUV::OnDrawFrame use");

    glActiveTexture(GL_TEXTURE0);
    DrawHelper::CheckGLError("SampleDrawYUV::OnDrawFrame glActiveTexture");
    glBindTexture(GL_TEXTURE_2D, mTextureLumin);
    DrawHelper::CheckGLError("SampleDrawYUV::OnDrawFrame glBindTexture");
    m_pShaderHelper->setInt("y_texture", 0);
    DrawHelper::CheckGLError("SampleDrawYUV::OnDrawFrame setInt y_texture");

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mTextureAlpha);
    m_pShaderHelper->setInt("uv_texture", 1);
    DrawHelper::CheckGLError("SampleDrawYUV::OnDrawFrame setInt uv_texture");

    glm::mat4 mvp = m_Projection * m_View * m_Model;
    m_pShaderHelper->setMat4("mvp", mvp);
    DrawHelper::CheckGLError("SampleDrawYUV::OnDrawFrame setMat4");

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
    DrawHelper::CheckGLError("SampleDrawYUV::OnDrawFrame glDrawElements");
    glBindVertexArray(GL_NONE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    GL_CHECK_ERROR("SampleDrawYUV::OnDrawFrame end");
    return 0;
}

RESULT SampleDrawYUV::InitSample() {
    LOGV("SampleDrawYUV::InitSample begin");
    createShader();
    createGLBuffer();
    return ERROR_OK;
}

void SampleDrawYUV::UnInitSample () {
    destroyGLBuffer();
    destroyShader();
}

RESULT SampleDrawYUV::SetImageYuvResource(MyImageInfo *const pSrcImage) {
    LOGD("SampleDrawYUV::SetImageYuvResource");
    if (nullptr == pSrcImage || nullptr == pSrcImage->ppBuffer[0]) {
        LOGE("SampleDrawYUV::SetImageYuvResource pSrcImage error");
        return ERROR_IMAGE;
    }
    m_YUVImage.width = pSrcImage->width;
    m_YUVImage.height = pSrcImage->height;
    m_YUVImage.format = pSrcImage->format;
    memcpy(m_YUVImage.wPitch, pSrcImage->wPitch, 4 * sizeof(int));
    OpenImageHelper::AllocMyImageInfo(&m_YUVImage);
    OpenImageHelper::CopyMyImageInfo(&m_YUVImage, pSrcImage);
    return 0;
}

RESULT SampleDrawYUV::createShader() {
    LOGD("SampleDrawYUV::createShader begin");
    GL_CHECK_ERROR("SampleDrawYUV::createShader begin");

    RESULT retCode = ERROR_OK;
    do {
        m_pShaderHelper = new ShaderHelper (yuv_vertex_shader, yuv_fragment_shader);
        //m_pShaderHelper = new ShaderHelper (vShaderStr, fShaderStr);
        if (nullptr == m_pShaderHelper || ERROR_OK != m_pShaderHelper->getShaderHelperState()) {
            LOGE("SampleDrawYUV::createShader error");
            retCode = m_pShaderHelper->getShaderHelperState();
            break;
        }
    } while(false);

    GL_CHECK_ERROR("SampleDrawYUV::createShader end");
    return retCode;
}

void SampleDrawYUV::destroyShader() {
    LOGD("SampleDrawYUV::destroyShader");
    SafeDelete(m_pShaderHelper)
}

RESULT SampleDrawYUV::createGLBuffer() {
    LOGD("SampleDrawYUV::createGLBuffer begin");
    GL_CHECK_ERROR("SampleDrawYUV::createGLBuffer begin");

    unsigned int srcWidth = m_YUVImage.width;
    unsigned int srcHeight = m_YUVImage.height;

    // create Texture Buffer Object
    const GLenum TEXTURE_TARGET = GL_TEXTURE_2D;
    DrawHelper::GetOneTexture(TEXTURE_TARGET, &mTextureLumin);
    glBindTexture(TEXTURE_TARGET, mTextureLumin);
    glTexImage2D(TEXTURE_TARGET, 0, GL_LUMINANCE, srcWidth, srcHeight, 0, GL_LUMINANCE,
            GL_UNSIGNED_BYTE, m_YUVImage.ppBuffer[0]);
    DrawHelper::CheckGLError("SampleDrawYUV::createGLBuffer glTexImage2D mTextureLumin");
    glBindTexture(TEXTURE_TARGET, GL_NONE);

    DrawHelper::GetOneTexture(TEXTURE_TARGET, &mTextureAlpha);
    glBindTexture(TEXTURE_TARGET, mTextureAlpha);
    glTexImage2D(TEXTURE_TARGET, 0, GL_LUMINANCE_ALPHA, srcWidth>>1, srcHeight>>1,
            0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, m_YUVImage.ppBuffer[1]);
    DrawHelper::CheckGLError("SampleDrawYUV::createGLBuffer glTexImage2D mTextureAlpha");
    glBindTexture(TEXTURE_TARGET, GL_NONE);


    const std::vector<GLfloat> vertex {
        -1.f, -1.f, 0.f,
         1.f, -1.f, 0.f,
         1.f,  1.f, 0.f,
        -1.f,  1.f, 0.f
    };
    const std::vector <GLfloat> texCoords {
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * texCoords.size(), &texCoords[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    DrawHelper::CheckGLError("SampleDrawYUV::createGLBuffer vbo[1]");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);
    DrawHelper::CheckGLError("SampleDrawYUV::createGLBuffer vbo[2]");

    int pos = 0;
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer (pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    DrawHelper::CheckGLError("SampleDrawYUV::createGLBuffer glVertexAttribPointer 0");

    pos = 1;
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void*)0);
    DrawHelper::CheckGLError("SampleDrawYUV::createGLBuffer glVertexAttribPointer 1");
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
    DrawHelper::CheckGLError("SampleDrawYUV::createGLBuffer glBindBuffer GL_NONE");

    int size = sizeof(vbo);
    int sizeu = sizeof(GLuint);
    SafeDeleteGLBuffers(sizeof(vbo)/ sizeof(GLuint), vbo)
    GL_CHECK_ERROR("SampleDrawYUV::createGLBuffer end");
    return ERROR_OK;
}

void SampleDrawYUV::destroyGLBuffer() {
    LOGD("SampleDrawYUV::destroyGLBuffer begin");
    SafeDeleteGLArrays(1, &m_VAO)
}

void SampleDrawYUV::initMVPMatrix() {

    LOGD("SampleDrawYUV::initMVPMatrix");
    m_Model = glm::mat4 (1.f);
    m_View = glm::mat4 (1.f);
    m_Projection = glm::mat4 (1.f);
}
