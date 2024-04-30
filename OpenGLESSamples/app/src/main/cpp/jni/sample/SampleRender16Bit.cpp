//
// Created by bitstream1024_wang on 2020/9/23.
//

#include <KitCommonDefine.h>
#include <vector>
#include "SampleRender16Bit.h"
#include "DrawHelper.h"

SampleRender16Bit::SampleRender16Bit() : m_TextureY(GL_NONE), m_TextureUV(GL_NONE) {
  m_pShaderHelper = nullptr;
  KitImageUtils::ZeroImage(&m_YUVImage);
  initMVPMatrix();
}

SampleRender16Bit::~SampleRender16Bit() {
  SafeDelete(m_pShaderHelper)
  OpenImageHelper::FreeMyImageInfo(&m_YUVImage);
}

RESULT SampleRender16Bit::OnDrawFrame() {
  //("SampleRender16Bit::OnDrawFrame begin");
  GL_CHECK_ERROR("SampleRender16Bit::OnDrawFrame begin");

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.f, 1.f, 0.f, 1.f);

  if (!m_bFitInOutDone) {
    int viewport[4]{0};
    glGetIntegerv(GL_VIEWPORT, viewport);
    int viewportOut[4]{0};
    DrawHelper::ViewportFitInOut(viewport, m_YUVImage.width, m_YUVImage.height, viewportOut);
    glViewport(viewportOut[0], viewportOut[1], viewportOut[2], viewportOut[3]);
    m_bFitInOutDone = true;
  }

  m_pShaderHelper->use();
  GL_CHECK_ERROR("SampleRender16Bit::OnDrawFrame use");

  glm::mat4 mvp = m_Projection * m_View * m_Model;
  m_pShaderHelper->setMat4("mvp", mvp);
  DrawHelper::CheckGLError("SampleRender16Bit::OnDrawFrame setMat4");

  if (m_YUVImage.format == KIT_FMT_GRAY10LE) {
    activeTextureForGray16();
  } else {
    activeTextureForP010();
  }

  glBindVertexArray(m_VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *) 0);
  DrawHelper::CheckGLError("SampleRender16Bit::OnDrawFrame glDrawElements");
  glBindVertexArray(GL_NONE);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, GL_NONE);

  GL_CHECK_ERROR("SampleRender16Bit::OnDrawFrame end");
  return 0;
}

RESULT SampleRender16Bit::InitSample() {
  LOGV("SampleRender16Bit::InitSample begin");
  createShader();
  createGLBuffer();
  return NONE_ERROR;
}

void SampleRender16Bit::UnInitSample() {
  destroyGLBuffer();
  destroyShader();
}

RESULT SampleRender16Bit::SetImage(KitImage *const pSrcImage) {
  LOGD("SampleRender16Bit::SetImage");
  if (nullptr == pSrcImage || nullptr == pSrcImage->data[0]) {
    LOGE("SampleRender16Bit::SetImage pSrcImage error");
    return ERROR_IMAGE;
  }
  m_YUVImage.width = pSrcImage->width;
  m_YUVImage.height = pSrcImage->height;
  m_YUVImage.format = pSrcImage->format;
  memcpy(m_YUVImage.wStride, pSrcImage->wStride, 4 * sizeof(int));
  KitImageUtils::AllocImage(&m_YUVImage);
  KitImageUtils::CopyImageToDst(pSrcImage, &m_YUVImage);
  return 0;
}

RESULT SampleRender16Bit::createShader() {
  LOGD("SampleRender16Bit::createShader begin");
  GL_CHECK_ERROR("SampleRender16Bit::createShader begin");

  RESULT retCode = NONE_ERROR;
  do {
    const char *pFragShader = yuv_fragment_shader_p010_lsb;
    if (KIT_FMT_GRAY10LE == m_YUVImage.format) {
      pFragShader = yuv_fragment_shader_gray16;
    }
    m_pShaderHelper = new ShaderHelper(yuv_vertex_shader, pFragShader);
    if (NONE_ERROR != m_pShaderHelper->getShaderHelperState()) {
      LOGE("SampleRender16Bit::createShader error");
      retCode = m_pShaderHelper->getShaderHelperState();
      break;
    }
  } while (false);

  GL_CHECK_ERROR("SampleRender16Bit::createShader end");
  return retCode;
}

void SampleRender16Bit::destroyShader() {
  LOGD("SampleRender16Bit::destroyShader");
  SafeDelete(m_pShaderHelper)
}

RESULT SampleRender16Bit::createGLBuffer() {
  LOGD("SampleRender16Bit::createGLBuffer begin");
  GL_CHECK_ERROR("SampleRender16Bit::createGLBuffer begin");

  unsigned int srcWidth = m_YUVImage.width;
  unsigned int srcHeight = m_YUVImage.height;

  // create Texture Buffer Object
  const GLenum TEXTURE_TARGET = GL_TEXTURE_2D;
  DrawHelper::GetOneTexture(TEXTURE_TARGET, &m_TextureY);
  glBindTexture(TEXTURE_TARGET, m_TextureY);
  glTexImage2D(TEXTURE_TARGET, 0, GL_LUMINANCE_ALPHA, srcWidth, srcHeight, 0, GL_LUMINANCE_ALPHA,
               GL_UNSIGNED_BYTE, m_YUVImage.data[0]);
  DrawHelper::CheckGLError("SampleRender16Bit::createGLBuffer glTexImage2D m_TextureY");
  glBindTexture(TEXTURE_TARGET, GL_NONE);
  if (m_YUVImage.format == KIT_FMT_P010_LSB || m_YUVImage.format == KIT_FMT_P010_MSB) {
    DrawHelper::GetOneTexture(TEXTURE_TARGET, &m_TextureUV);
    glBindTexture(TEXTURE_TARGET, m_TextureUV);
    glTexImage2D(TEXTURE_TARGET, 0, GL_RGBA, srcWidth >> 1, srcHeight >> 1, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, m_YUVImage.data[1]);
    DrawHelper::CheckGLError("SampleRender16Bit::createGLBuffer glTexImage2D m_TextureUV");
    glBindTexture(TEXTURE_TARGET, GL_NONE);
  }

  // coordinates left-bottom is origin point, left-bottom, right-bottom, right-top, left-top
  const std::vector<GLfloat> vertex{
      -1.f, -1.f, 0.f,
      1.f, -1.f, 0.f,
      1.f, 1.f, 0.f,
      -1.f, 1.f, 0.f
  };
  // normal texture left-top is origin point, so the order is left-top, right-top, right-bottom, left-bottom
  const std::vector<GLfloat> texCoords{
      0.f, 1.f,
      1.f, 1.f,
      1.f, 0.f,
      0.f, 0.f
  };
  const std::vector<GLuint> indices{0, 1, 2, 0, 2, 3};

  glGenVertexArrays(1, &m_VAO);
  glBindVertexArray(m_VAO);
  GLuint vbo[3] = {GL_NONE};
  glGenBuffers(3, vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex.size(), &vertex[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
  DrawHelper::CheckGLError("SampleRender16Bit::createGLBuffer vbo[0]");

  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * texCoords.size(), &texCoords[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
  DrawHelper::CheckGLError("SampleRender16Bit::createGLBuffer vbo[1]");

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(GLuint) * indices.size(),
               &indices[0],
               GL_STATIC_DRAW);
  DrawHelper::CheckGLError("SampleRender16Bit::createGLBuffer vbo[2]");

  int pos = 0;
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glEnableVertexAttribArray(pos);
  glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void *) 0);
  DrawHelper::CheckGLError("SampleRender16Bit::createGLBuffer glVertexAttribPointer 0");

  pos = 1;
  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glEnableVertexAttribArray(pos);
  glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void *) 0);
  DrawHelper::CheckGLError("SampleRender16Bit::createGLBuffer glVertexAttribPointer 1");
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
  glBindVertexArray(GL_NONE);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
  DrawHelper::CheckGLError("SampleRender16Bit::createGLBuffer glBindBuffer GL_NONE");

  SafeDeleteGLBuffers(sizeof(vbo) / sizeof(GLuint), vbo)
  GL_CHECK_ERROR("SampleRender16Bit::createGLBuffer end");
  return NONE_ERROR;
}

void SampleRender16Bit::destroyGLBuffer() {
  LOGD("SampleRender16Bit::destroyGLBuffer begin");
  SafeDeleteTexture(&m_TextureY);
  SafeDeleteTexture(&m_TextureUV);
}

void SampleRender16Bit::initMVPMatrix() {

  LOGD("SampleRender16Bit::initMVPMatrix");
  m_Model = glm::mat4(1.f);
  m_View = glm::mat4(1.f);
  m_Projection = glm::mat4(1.f);
}

void SampleRender16Bit::activeTextureForGray16() {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_TextureY);
  m_pShaderHelper->setInt("la_texture", 0);
}

void SampleRender16Bit::activeTextureForP010() {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_TextureY);
  m_pShaderHelper->setInt("y_texture", 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, m_TextureUV);
  m_pShaderHelper->setInt("uv_texture", 1);
}
