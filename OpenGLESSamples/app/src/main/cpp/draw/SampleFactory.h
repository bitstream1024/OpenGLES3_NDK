//
// Created by wcg3031 on 2020/9/28.
//

#pragma once

#include "SampleBase.h"

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
	eDraw_Texture
}SampleType;

class SampleFactory {
public:
	static SampleBase * CreateSample (SampleType sampleType) {
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
				return new SampleDrawFBO ();
			case eDraw_Render3DMesh:
				return new SampleRender3DMesh ();
			case eDraw_Texture:
				return new SampleTexture();
			default:
				break;
		}
	}
};