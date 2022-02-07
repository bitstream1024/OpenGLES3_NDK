//
// Created by wcg3031 on 2020/9/28.
//

#pragma once

#include <common.h>
#include <ShaderHelper.h>
#include <MyImageInfo.h>
#include "SampleBase.h"
#include "ScreenRect.h"

#include <thread>
#include <mutex>
#include <condition_variable>

class SampleTexture: public SampleBase
{
public:
	SampleTexture();
	~SampleTexture();
	RESULT SetImageInfo (MyImageInfo *const pSrc);
	RESULT Create ();
    RESULT Destroy ();
    RESULT UpdateTexture (MyImageInfo *const pSrcImg);
    RESULT DrawFrame ();
    RESULT DrawFrameAsync ();

    MyImageInfo* GetMyImageInfo();
    GLuint GetTextureID();

public: //test
    bool m_bThreadExist;
    bool m_bTextureReady;
    std::mutex m_TextureMutex;
    std::condition_variable m_TextureReadyConditionVariable;
    /*std::condition_variable_any m_TextureReadyCondition;
    std::condition_variable_any m_TextureUsedCondition;*/

    bool m_bExitThread;
    int GetFrameID();
    void UpdateFrameID(int frameID = -1);

private:
	GLuint m_VAO;
	GLuint m_TextureID;
	ShaderHelper *m_TextureShaderHelper;
	MyImageInfo m_TextureImage;
	ScreenRect* m_pScreenRect;
};



