//
// Created by chauncy on 2022/1/25.
//

#pragma once

#include "MyImageInfo.h"
#include "LogAndroid.h"

class NativeImageUtils
{
public:

    static void PrintNativeImageInfo(const MyImageInfo *pSrcImg)
    {
        LOGV("PrintNativeImageInfo pSrcImg: %p", pSrcImg);
        if (nullptr != pSrcImg)
        {
            LOGV("PrintNativeImageInfo pSrcImg: %d, %d, %d, wPitch(%d,%d,%d,%d), hPitch(%d,%d,%d,%d), ppBuffer(%p,%p,%p,%p)",
                 pSrcImg->width, pSrcImg->height, pSrcImg->format,
                 pSrcImg->wPitch[0], pSrcImg->wPitch[1], pSrcImg->wPitch[2], pSrcImg->wPitch[3],
                 pSrcImg->hPitch[0], pSrcImg->hPitch[1], pSrcImg->hPitch[2], pSrcImg->hPitch[3],
                 pSrcImg->ppBuffer[0], pSrcImg->ppBuffer[1], pSrcImg->ppBuffer[2], pSrcImg->ppBuffer[3]);
        }
    }

    static bool IsNativeImageValid(const MyImageInfo *pSrcImg)
    {
        if (nullptr == pSrcImg || nullptr == pSrcImg->ppBuffer[0] || pSrcImg->width <= 0 || pSrcImg->height <= 0)
            return false;

        return true;
    }

    static bool IsNativeImageSameSize(const MyImageInfo *pSrcImg, const MyImageInfo* pDstImg)
    {
        if (!IsNativeImageValid(pSrcImg) || !IsNativeImageValid(pDstImg))
            return false;

        if (pSrcImg->width != pDstImg->width || pSrcImg->height != pDstImg->height || pSrcImg->format != pDstImg->format)
            return false;

        bool bOK = true;

        for (int i = 0; i < 4; ++i)
        {
            if (pSrcImg->wPitch[i] != pDstImg->wPitch[i] || pSrcImg->hPitch[i] != pDstImg->hPitch[i]) {
                bOK = false;
                break;
            }
        }

        return bOK;
    }

    static RESULT AllocNativeImage (MyImageInfo *pSrcImg)
    {
        if (nullptr != pSrcImg->ppBuffer[0]) {
            FreeNativeImage(pSrcImg);
        }

        RESULT nRet = ERROR_OK;
        unsigned int length = 0;
        switch (pSrcImg->format)
        {
            case MY_FORMAT_RGBA:
            case MY_FORMAT_RGB:
                length = pSrcImg->wPitch[0] * pSrcImg->height;
                pSrcImg->ppBuffer[0] = static_cast<unsigned char *>(malloc(length));
                memset(pSrcImg->ppBuffer[0], 0, length);
                break;
            case MY_FORMAT_NV21:
            case MY_FORMAT_NV12:
                length = pSrcImg->wPitch[0] * pSrcImg->height * 3/2;
                pSrcImg->ppBuffer[0] = static_cast<unsigned char *>(malloc(length));
                memset(pSrcImg->ppBuffer[0], 0, length);
                pSrcImg->ppBuffer[1] = pSrcImg->ppBuffer[0] + pSrcImg->wPitch[0] * pSrcImg->height;
                break;
            default:
                LOGE("AllocNativeImage failed, not support format!!!");
                nRet = ERROR_IMAGE;
                break;
        }

        return nRet;
    }

    static void FreeNativeImage (MyImageInfo *pSrcImg)
    {
        if (IsNativeImageValid(pSrcImg))
            return;
        SafeFree(pSrcImg->ppBuffer[0])
    }

    static RESULT CopyNativeImageToDst(const MyImageInfo *pSrcImg, MyImageInfo *pDstImg)
    {
        AUTO_COUNT_TIME_COST("CopyNativeImageToDst");

        if (!IsNativeImageValid(pSrcImg) || !IsNativeImageValid(pDstImg) || !IsNativeImageSameSize(pSrcImg, pDstImg))
        {
            return ERROR_IMAGE;
        }

        int bufferLen = 0;
        switch(pSrcImg->format)
        {
            case MY_FORMAT_RGBA:
            case MY_FORMAT_RGB:
                bufferLen = pSrcImg->wPitch[0] * pSrcImg->height;
                memcpy(pDstImg->ppBuffer[0], pSrcImg->ppBuffer[0], bufferLen);
                break;
            case MY_FORMAT_NV12:
            case MY_FORMAT_NV21:
                bufferLen = pSrcImg->wPitch[0] * pSrcImg->height * 3/2;
                memcpy(pDstImg->ppBuffer[0], pSrcImg->ppBuffer[0], bufferLen);
                break;
            default:
                LOGE("CopyNativeImageToDst failed, not support format!!!");
                break;
        }
        if (bufferLen <= 0)
            return ERROR_IMAGE;

        return ERROR_OK;
    }
};