//
// Created by chauncy on 2022/1/25.
//

#pragma once

#include "MyImageInfo.h"
#include "LogAndroid.h"

class NativeImageUtils
{
private:
    static void GetImageFileExtByFormat(const int &imgFormat, char *szExt)
    {
        CHECK_NULL_INPUT_VOID(szExt);

        switch(imgFormat)
        {
            case MY_FORMAT_NV12:
                sprintf(szExt, "%s", ".NV12");
                break;
            case MY_FORMAT_NV21:
                sprintf(szExt, "%s", ".NV21");
                break;
            case MY_FORMAT_RGB24:
                sprintf(szExt, "%s", ".RGB24");
                break;
            case MY_FORMAT_RGB32:
                sprintf(szExt, "%s", ".RGB32");
                break;
            default:
                sprintf(szExt, "%s", ".undefined");
                LOGE("GetImageFileExtByFormat image format is not supported!!!");
                break;
        }
    }

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

    static void ZeroNativeImage (LPMyImageInfo lpMyImageInfo) {
        if (nullptr == lpMyImageInfo) {
            return;
        }
        memset(lpMyImageInfo, 0, sizeof(MyImageInfo));
    }

    static unsigned int AllocNativeImage (MyImageInfo *pSrcImg)
    {
        AUTO_COUNT_TIME_COST("AllocNativeImage");

        if (nullptr == pSrcImg)
            return 0;

        if (nullptr != pSrcImg->ppBuffer[0]) {
            FreeNativeImage(pSrcImg);
        }

        RESULT nRet = ERROR_OK;
        unsigned int length = 0;
        switch (pSrcImg->format)
        {
            case MY_FORMAT_RGB32:
            case MY_FORMAT_RGB24:
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

        LOGE("AllocNativeImage length: %d, nRet = %d", length, nRet);
        return length;
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
            case MY_FORMAT_RGB32:
            case MY_FORMAT_RGB24:
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

    static void AssignNativeImagePitchByWidth (LPMyImageInfo lpMyImageInfo)
    {
        CHECK_NULL_INPUT_VOID(lpMyImageInfo);

        switch (lpMyImageInfo->format)
        {
            case MY_FORMAT_NV12:
            case MY_FORMAT_NV21:
                lpMyImageInfo->wPitch[0] = lpMyImageInfo->width;
                lpMyImageInfo->wPitch[1] = lpMyImageInfo->width;
                break;
            case MY_FORMAT_RGB24:
                lpMyImageInfo->wPitch[0] = lpMyImageInfo->width * 3;
                break;
            case MY_FORMAT_RGB32:
                lpMyImageInfo->wPitch[0] = lpMyImageInfo->width * 4;
                break;
            default:
                LOGE("AssignNativeImagePitchByWidth format nor supported %d", lpMyImageInfo->format);
                break;
        }
    }

    static int LoadYuvImageFromFile (const char* sPath, LPMyImageInfo lpMyImageInfo)
    {
        LOGD("LoadYuvImageFromFile sPath = %s", sPath);
        CHECK_NULL_INPUT(sPath)
        CHECK_NULL_INPUT(lpMyImageInfo)
        int ret = ERROR_OK;
        do
        {
            std::string filePath = sPath;
            std::vector<std::string> strTemp0 = MyFileHelper::StringSplit (filePath, '.', '.');
            for (auto val:strTemp0)
                LOGD("LoadYuvImageFromFile %s", val.c_str());
            if (strTemp0.size() != 2)
            {
                LOGE("LoadYuvImageFromFile sPath is not supported strTemp0");
                ret = ERROR_INPUT;
                break;
            }
            memset(lpMyImageInfo, 0, sizeof(MyImageInfo));
            GetImageFormatByExt(strTemp0[1], lpMyImageInfo->format);
            LOGD("LoadYuvImageFromFile GetImageFormatByExt nFormat = %d", lpMyImageInfo->format);

            std::vector<std::string> strTemp1 = MyFileHelper::StringSplit(strTemp0[0], 'x', 'X');
            if (strTemp1.size() != 2)
            {
                LOGE("LoadYuvImageFromFile sPath is not supported   strTemp1");
                ret = ERROR_INPUT;
                break;
            }
            lpMyImageInfo->height = atoi(strTemp1[1].c_str());
            LOGD("LoadYuvImageFromFile height = %d", lpMyImageInfo->height);

            std::vector<std::string> strTemp2 = MyFileHelper::StringSplit(strTemp1[0], '_', '_');
            if (0 == strTemp2.size())
            {
                LOGE("LoadYuvImageFromFile sPath is not supported   strTemp2");
                ret = ERROR_INPUT;
                break;
            }
            lpMyImageInfo->width = atoi(strTemp2[strTemp2.size()-1].c_str());
            AssignNativeImagePitchByWidth(lpMyImageInfo);
            long lSize = AllocNativeImage(lpMyImageInfo);
            if (0 == lSize)
            {
                ret = ERROR_MEMORY;
                break;
            }

            FILE *fp = nullptr;
            fp = fopen(sPath, "rb");
            if (fp)
            {
                fread(lpMyImageInfo->ppBuffer[0], 1, lSize, fp);
                fclose(fp);
            }
            else
            {
                ret = ERROR_FILE_NOT_EXIT;
                break;
            }

        } while (false);

        if (ERROR_OK != ret)
            FreeNativeImage(lpMyImageInfo);

        return ret;
    }

    static void GetImageSaveNameWithFolder(const char *pFolder, const LPMyImageInfo lpMyImage, char* szSaveName)
    {
        CHECK_NULL_INPUT_VOID(pFolder);
        CHECK_NULL_INPUT_VOID(lpMyImage);
        CHECK_NULL_INPUT_VOID(szSaveName);

        char szExt[PATH_LEN_MAX] {0};
        GetImageFileExtByFormat(lpMyImage->format, szExt);

        long long llTimeStamp = MyTimeUtils::GetCurrentTime();
        sprintf(szSaveName, "%s/Image_0_%dx%d_%lld%s", pFolder, lpMyImage->wPitch[0], lpMyImage->height, llTimeStamp, szExt);
    }

    static RESULT SaveYuvImageToFile (const LPMyImageInfo lpMyImageInfo, const char* sPath)
    {
        AUTO_COUNT_TIME_COST("SaveYuvImageToFile")

        LOGD("SaveYuvImageToFile sPath = %s", sPath);
        CHECK_NULL_INPUT(lpMyImageInfo)
        CHECK_NULL_INPUT(sPath)

        if (!IsNativeImageValid(lpMyImageInfo))
        {
            return ERROR_INPUT;
        }

        int ret = ERROR_OK;
        FILE *fp = fopen(sPath, "wb");
        do
        {
            if (!fp)
            {
                ret = ERROR_FILE_COMMON;
                LOGE("SaveYuvImageToFile, open file failed, sPath = %s", sPath);
                break;
            }

            unsigned int uLength = 0;
            switch (lpMyImageInfo->format)
            {
                case MY_FORMAT_NV21:
                case MY_FORMAT_NV12:
                    uLength = lpMyImageInfo->wPitch[0] * lpMyImageInfo->height * 3/2;
                    break;
                case MY_FORMAT_RGB24:
                case MY_FORMAT_RGB32:
                    uLength = lpMyImageInfo->wPitch[0] * lpMyImageInfo->height;
                    break;
                default:
                    LOGE("SaveYuvImageToFile, image format is not supported!!!");
                    break;
            }

            if (uLength > 0)
                fwrite(lpMyImageInfo->ppBuffer[0], 1, uLength, fp);

        } while(false);

        if (fp)
        {
            fclose(fp);
        }
        return ret;
    }

    /*static void GetImageFormatByFileName(const std::string &strFileName, int &dstFormat)
    {

    }*/

    static void GetImageFormatByExt (const std::string sExt, int &nFormat)
    {
        LOGD ("GetImageFormatByExt sExt = %s", sExt.c_str());
        if ("NV12" == sExt || "nv12" == sExt)
            nFormat = MY_FORMAT_NV12;
        else if ("NV21" == sExt || "nv21" == sExt)
            nFormat = MY_FORMAT_NV21;
        else if ("RGB24" == sExt || "rgb24" == sExt)
            nFormat = MY_FORMAT_RGB24;
        else if ("RGB32" == sExt || "rgb32" == sExt)
            nFormat = MY_FORMAT_RGB32;
        else
            LOGE("GetImageFormatByExt unsupported format");
    }
};