//
// Created by bitstream1024_wang on 2020/9/23.
//

#ifndef SENIOROPENGLES_SAMPLEDRAWYUV16Bit_H
#define SENIOROPENGLES_SAMPLEDRAWYUV16Bit_H

#include <ShaderHelper.h>
#include <KitCommon.h>
#include <KitImage.h>
#include "SampleBase.h"

class SampleRender16Bit : public SampleBase {

 public:
  SampleRender16Bit();
  ~SampleRender16Bit();
  RESULT OnDrawFrame();
  RESULT InitSample();
  void UnInitSample();
  RESULT SetImage(KitImage *const pSrcImage);

 private:
  RESULT createShader();
  void destroyShader();
  RESULT createGLBuffer();
  void destroyGLBuffer();
  void initMVPMatrix();
  void activeTextureForGray16();
  void activeTextureForP010();

 private:
  KitImage m_YUVImage;
  GLuint m_TextureY;
  GLuint m_TextureUV;
  ShaderHelper *m_pShaderHelper;

  const char *yuv_vertex_shader =
      GLES_VERSION_STRING
      R"(
layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aTexCoord;
out vec2 v_texCoord;
uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(aPos, 1.0);
	v_texCoord = aTexCoord;
}
)";

  const char *yuv_fragment_shader_gray16 =
      GLES_VERSION_STRING
      GLES_MEDIUM_STRING
      R"(
in vec2 v_texCoord;
out vec4 FragColor;
uniform sampler2D la_texture;

void main()
{
    vec4 color = texture(la_texture, v_texCoord);
    float lum = color.r * 255.f + color.a * 255.f * pow(2.f, 8.f);
	FragColor = vec4(vec3(lum/(pow(2.f, 10.f) - 1.f)), 1.f);
}
)";

  const char *yuv_fragment_shader_p010_lsb =
      GLES_VERSION_STRING
      GLES_MEDIUM_STRING
      R"(
in vec2 v_texCoord;
out vec4 FragColor;
uniform sampler2D y_texture;
uniform sampler2D uv_texture;

mat3 matYUVToRGB = mat3(
        1.f,     1.f,     1.f,
        0.f, 	-0.344f,  1.770f,
        1.403f, -0.714f,  0.f
    );

void main()
{
    vec3 yuv;
    float den = pow(2.f, 10.f) - 1.f;
    vec4 yColor = texture (y_texture, v_texCoord);
    yuv.x = (yColor.r * 255.f + yColor.a * 255.f * pow(2.f, 8.f))/den;
    vec4 uvColor = texture (uv_texture, v_texCoord);
    yuv.y = (uvColor.r * 255.f + uvColor.g * 255.f * pow(2.f, 8.f))/den- 0.5f;
    yuv.z = (uvColor.b * 255.f + uvColor.a * 255.f * pow(2.f, 8.f))/den- 0.5f;

    vec3 rgb = matYUVToRGB * yuv;
	FragColor = vec4 (rgb, 1.f);
}
)";

};

#endif //SENIOROPENGLES_SAMPLEDRAWYUV16Bit_H
