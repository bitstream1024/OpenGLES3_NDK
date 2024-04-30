//
// Created by bitstream1024_wang on 2020/9/23.
//

#ifndef SENIOROPENGLES_SAMPLEDRAWYUV_H
#define SENIOROPENGLES_SAMPLEDRAWYUV_H


#include <ShaderHelper.h>
#include <KitCommon.h>
#include <KitImage.h>
#include "SampleBase.h"

class SampleDrawYUV : public SampleBase {

public:
    SampleDrawYUV ();
    ~SampleDrawYUV();
    RESULT OnDrawFrame ();
    RESULT InitSample ();
    void UnInitSample ();
    RESULT SetImageYuvResource (KitImage *const pSrcImage);

private:
    RESULT createShader();
    void destroyShader();
    RESULT createGLBuffer ();
    void destroyGLBuffer ();
    void initMVPMatrix ();

    KitImage m_YUVImage;
    GLuint mTextureLumin;
    GLuint mTextureAlpha;
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

    const char *yuv_fragment_shader_nv21 =
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
    yuv.x = texture (y_texture, v_texCoord).r;
    yuv.y = texture (uv_texture, v_texCoord).a - 0.5;
    yuv.z = texture (uv_texture, v_texCoord).r - 0.5;

    vec3 rgb = matYUVToRGB * yuv;
	FragColor = vec4 (rgb, 1.f);
}
)";

    const char *yuv_fragment_shader_nv12 =
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
    yuv.x = texture (y_texture, v_texCoord).r;
    yuv.y = texture (uv_texture, v_texCoord).r - 0.5;
    yuv.z = texture (uv_texture, v_texCoord).a - 0.5;

    vec3 rgb = matYUVToRGB * yuv;
	FragColor = vec4 (rgb, 1.f);
}
)";

};


#endif //SENIOROPENGLES_SAMPLEDRAWYUV_H
