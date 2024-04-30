//
// Created by bitstream1024 on 2020/9/28.
//

#pragma once

#include "SampleBase.h"
#include "SampleTextRender.h"
#include "SampleRender16Bit.h"

typedef enum
{
	eDraw_Triangle = 0,
	eDraw_SimpleTexture,
	eDraw_TextureFBO,
	eDraw_HardwareBuffer,
	eDraw_Transform,
	eDraw_Render3D,
	eDraw_TriangleFBO,
	eDraw_Render3DMesh,
	eDraw_Texture,
	eDraw_YUV,
	eDraw_YUV16Bit,
    eDraw_Text,
    lDraw_TypeNum
}SampleType;

class SampleFactory {
public:
	static SampleBase * CreateSample (SampleType sampleType) {
        SampleBase *pBase(nullptr);
		switch (sampleType) {
			case eDraw_Triangle:
				break;
			case eDraw_SimpleTexture:
				break;
			case eDraw_TextureFBO:
				break;
			case eDraw_HardwareBuffer:
				break;
			case eDraw_Transform:
				break;
			case eDraw_Render3D:
				break;
			case eDraw_TriangleFBO:
                pBase = new SampleDrawFBO ();
                break;
			case eDraw_Render3DMesh:
                pBase = new SampleRender3DMesh ();
                break;
			case eDraw_Texture:
                pBase = new SampleTexture();
                break;
			case eDraw_YUV:
                pBase = new SampleDrawYUV();
                break;
            case eDraw_YUV16Bit:
                pBase = new SampleRender16Bit();
                break;
            case eDraw_Text:
                pBase = new SampleTextRender();
                break;
			default:
				break;
		}
        return pBase;
	}
};