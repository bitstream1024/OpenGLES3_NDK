//
// Created by bitstream1024 on 2020/4/2.
//

#pragma once

#include "ShaderHelper.h"
#include "processor_inner.h"

int CreateShaderHelper (LPShaderSet pShaderSet, const std::string vShader, const std::string fShader);

int drawTriangle (ShaderHelper *pShaderHelper);

int drawTexture (ShaderHelper *pShaderHelper, const LPKitImage lpMyImageInfo);

int drawFBO (ShaderHelper *pShaderHelperFBO, ShaderHelper *pShaderHelperNormal, const LPKitImage lpMyImageInfo);

int drawByHardwareBuffer (const AHardwareBufferHelper *pHardwareBufferHelper, const LPKitImage lpMyImageInfo);

int drawByHardwareBuffer2 (const LPKitImage lpMyImageInfo);

