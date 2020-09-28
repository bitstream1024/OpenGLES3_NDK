//
// Created by chauncy_wang on 2020/9/23.
//

#ifndef SENIOROPENGLES_SAMPLEDRAWYUV_H
#define SENIOROPENGLES_SAMPLEDRAWYUV_H


#include <ShaderHelper.h>
#include <common.h>
#include <MyImageInfo.h>
#include "SampleBase.h"

class SampleDrawYUV : public SampleBase {

public:
    SampleDrawYUV ();
    ~SampleDrawYUV();
    RESULT OnDrawFrame ();
    RESULT SetImageYuvResource (MyImageInfo *const pSrcImage);

private:
    RESULT createShader();
    void destroyShader();
    RESULT createGLBuffer ();
    void destroyGLBuffer ();
    void initMVPMatrix ();

    MyImageInfo m_YUVImage;
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

    const char *yuv_fragment_shader =
            GLES_VERSION_STRING
    GLES_MEDIUM_STRING
    R"(
in vec2 v_texCoord;
out vec4 FragColor;
uniform sampler2D y_texture;
uniform sampler2D uv_texture;

void main()
{
    vec3 yuv;
    yuv.x = texture (y_texture, v_texCoord).r;
    yuv.x = texture (uv_texture, v_texCoord).a - 0.5;
    yuv.x = texture (uv_texture, v_texCoord).a - 0.5;
    mat3 matYuv2Rgb = {
        1.0,    1.0,    1.0,
        0.0, 	-0.344, 1.770,
        1.403,  -0.714, 0.0
    };
	FragColor = matYuv2Rgb * yuv;
}
)";

};


#endif //SENIOROPENGLES_SAMPLEDRAWYUV_H
