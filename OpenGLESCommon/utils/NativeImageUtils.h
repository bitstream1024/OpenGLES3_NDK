//
// Created by chauncy on 2022/1/25.
//

#pragma once

#include "MyImageInfo.h"
#include "LogAndroid.h"

class NativeImageUtils {
public:
    class ImageConverter {
    public:
        static void ConvertImage(const MyImageInfo *pSrcImg, MyImageInfo *pDstImg) {

        }

        static void ConvertRGB32ToNV12(const MyImageInfo *pSrcImg, MyImageInfo *pDstImg) {

        }

        static void ConvertRGB32ToNV21(const MyImageInfo *pSrcImg, MyImageInfo *pDstImg) {

        }

        static void ConvertGray10ToGray(const MyImageInfo *pSrcImg, MyImageInfo *pDstImg) {
            if (nullptr == pSrcImg || nullptr == pDstImg || MY_FORMAT_GRAY10LE != pSrcImg->format
                || MY_FORMAT_GRAY != pDstImg->format) {
                return;
            }

        }
    };


public:

    static void PrintNativeImageInfo(const MyImageInfo *pSrcImg) {
        LOGV("PrintNativeImageInfo pSrcImg: %p", pSrcImg);
        if (nullptr != pSrcImg) {
            LOGV("PrintNativeImageInfo pSrcImg: %d, %d, %d, wPitch(%d,%d,%d,%d), hPitch(%d,%d,%d,%d), ppBuffer(%p,%p,%p,%p)",
                 pSrcImg->width, pSrcImg->height, pSrcImg->format,
                 pSrcImg->wPitch[0], pSrcImg->wPitch[1], pSrcImg->wPitch[2], pSrcImg->wPitch[3],
                 pSrcImg->hPitch[0], pSrcImg->hPitch[1], pSrcImg->hPitch[2], pSrcImg->hPitch[3],
                 pSrcImg->ppBuffer[0], pSrcImg->ppBuffer[1], pSrcImg->ppBuffer[2],
                 pSrcImg->ppBuffer[3]);
        }
    }

    static bool IsNativeImageValid(const MyImageInfo *pSrcImg) {
        if (nullptr == pSrcImg || nullptr == pSrcImg->ppBuffer[0] || pSrcImg->width <= 0 ||
            pSrcImg->height <= 0)
            return false;

        return true;
    }

    static bool IsNativeImageSameSize(const MyImageInfo *pSrcImg, const MyImageInfo *pDstImg) {
        if (!IsNativeImageValid(pSrcImg) || !IsNativeImageValid(pDstImg))
            return false;

        if (pSrcImg->width != pDstImg->width || pSrcImg->height != pDstImg->height ||
            pSrcImg->format != pDstImg->format)
            return false;

        bool bOK = true;

        for (int i = 0; i < 4; ++i) {
            if (pSrcImg->wPitch[i] != pDstImg->wPitch[i] ||
                pSrcImg->hPitch[i] != pDstImg->hPitch[i]) {
                bOK = false;
                break;
            }
        }
        return bOK;
    }

    static void ZeroNativeImage(LPMyImageInfo lpMyImageInfo) {
        if (nullptr == lpMyImageInfo) {
            return;
        }
        memset(lpMyImageInfo, 0, sizeof(MyImageInfo));
    }

    static unsigned int AllocNativeImage(MyImageInfo *pSrcImg) {
        AUTO_COUNT_TIME_COST("AllocNativeImage");

        if (nullptr == pSrcImg)
            return 0;

        if (nullptr != pSrcImg->ppBuffer[0]) {
            FreeNativeImage(pSrcImg);
        }

        RESULT nRet = ERROR_OK;
      unsigned int bufferSize[4] = {0};
      unsigned int totalSize = 0;
        switch (pSrcImg->format) {
            case MY_FORMAT_NV21:
            case MY_FORMAT_NV12:
            case MY_FORMAT_P010_LSB:
              bufferSize[0] = pSrcImg->wPitch[0] * pSrcImg->height;
              bufferSize[1] = pSrcImg->wPitch[1] * pSrcImg->height;
              totalSize = bufferSize[0] + bufferSize[1];
                pSrcImg->ppBuffer[0] = static_cast<unsigned char *>(malloc(totalSize));
                memset(pSrcImg->ppBuffer[0], 0, totalSize);
                pSrcImg->ppBuffer[1] = pSrcImg->ppBuffer[0] + bufferSize[0];
                break;
            case MY_FORMAT_I420: {
                int yLen = pSrcImg->wPitch[0] * pSrcImg->height;
              totalSize = yLen + (yLen << 1);
                pSrcImg->ppBuffer[0] = static_cast<unsigned char *>(malloc(totalSize));
                memset(pSrcImg->ppBuffer[0], 0, totalSize);
                pSrcImg->ppBuffer[1] = pSrcImg->ppBuffer[0] + (yLen >> 2);
                pSrcImg->ppBuffer[2] = pSrcImg->ppBuffer[1] + (yLen >> 2);
            }
                break;
            case MY_FORMAT_GRAY:
            case MY_FORMAT_GRAY10LE:
              totalSize = pSrcImg->wPitch[0] * pSrcImg->height;
                pSrcImg->ppBuffer[0] = static_cast<unsigned char *>(malloc(totalSize));
                memset(pSrcImg->ppBuffer[0], 0, totalSize);
                break;
            case MY_FORMAT_RGB32:
            case MY_FORMAT_RGB24:
              totalSize = pSrcImg->wPitch[0] * pSrcImg->height;
                pSrcImg->ppBuffer[0] = static_cast<unsigned char *>(malloc(totalSize));
                memset(pSrcImg->ppBuffer[0], 0, totalSize);
                break;
            default:
                LOGE("AllocNativeImage failed, not support format!!!");
                nRet = ERROR_IMAGE;
                break;
        }

        LOGE("AllocNativeImage totalSize: %d, nRet = %d", totalSize, nRet);
        return totalSize;
    }

    static void FreeNativeImage(MyImageInfo *pSrcImg) {
        if (IsNativeImageValid(pSrcImg))
            return;
        SafeFree(pSrcImg->ppBuffer[0])
    }

    static RESULT CopyNativeImageToDst(const MyImageInfo *pSrcImg, MyImageInfo *pDstImg) {
        AUTO_COUNT_TIME_COST("CopyNativeImageToDst");

        if (!IsNativeImageValid(pSrcImg) || !IsNativeImageValid(pDstImg) ||
            !IsNativeImageSameSize(pSrcImg, pDstImg)) {
            return ERROR_IMAGE;
        }

        unsigned int bufSizeList[4] {0};
      CalImagePlaneSize(pSrcImg, bufSizeList);
        switch (pSrcImg->format) {
            case MY_FORMAT_NV12:
            case MY_FORMAT_NV21:
            case MY_FORMAT_P010_LSB:
            case MY_FORMAT_P010_MSB:
                memcpy(pDstImg->ppBuffer[0], pSrcImg->ppBuffer[0], bufSizeList[0]);
                memcpy(pDstImg->ppBuffer[1], pSrcImg->ppBuffer[1], bufSizeList[1]);
                break;
            case MY_FORMAT_I420:
                memcpy(pDstImg->ppBuffer[0], pSrcImg->ppBuffer[0], bufSizeList[0]);
                memcpy(pDstImg->ppBuffer[1], pSrcImg->ppBuffer[1], bufSizeList[1]);
                memcpy(pDstImg->ppBuffer[2], pSrcImg->ppBuffer[2], bufSizeList[2]);
                break;
          case MY_FORMAT_RGB32:
          case MY_FORMAT_RGB24:
          case MY_FORMAT_GRAY:
          case MY_FORMAT_GRAY10LE:
            memcpy(pDstImg->ppBuffer[0], pSrcImg->ppBuffer[0], bufSizeList[0]);
            break;
            default:
                LOGE("CopyNativeImageToDst failed, not support format!!!");
                break;
        }

        return ERROR_OK;
    }

    static void AssignNativeImagePitchByWidth(LPMyImageInfo lpMyImageInfo) {
        CHECK_NULL_INPUT_VOID(lpMyImageInfo);

        unsigned int bufSizeList[4] {0};
      CalImagePlaneSize(lpMyImageInfo, bufSizeList);
        switch (lpMyImageInfo->format) {
            case MY_FORMAT_NV12:
          case MY_FORMAT_NV21:
            lpMyImageInfo->wPitch[0] = lpMyImageInfo->width;
            lpMyImageInfo->wPitch[1] = lpMyImageInfo->width;
            break;
          case MY_FORMAT_P010_LSB:
          case MY_FORMAT_P010_MSB:
            lpMyImageInfo->wPitch[0] = lpMyImageInfo->width << 1;
            lpMyImageInfo->wPitch[1] = lpMyImageInfo->width << 1;
            break;
            case MY_FORMAT_I420:
                lpMyImageInfo->wPitch[0] = lpMyImageInfo->width;
                lpMyImageInfo->wPitch[1] = lpMyImageInfo->width >> 1;
                lpMyImageInfo->wPitch[2] = lpMyImageInfo->width >> 1;
                break;
            case MY_FORMAT_GRAY:
                lpMyImageInfo->wPitch[0] = lpMyImageInfo->width;
                break;
            case MY_FORMAT_GRAY10LE:
                lpMyImageInfo->wPitch[0] = lpMyImageInfo->width << 1;
                break;
            case MY_FORMAT_RGB24:
                lpMyImageInfo->wPitch[0] = lpMyImageInfo->width * 3;
                break;
            case MY_FORMAT_RGB32:
                lpMyImageInfo->wPitch[0] = lpMyImageInfo->width * 4;
                break;
            default:
                LOGE("AssignNativeImagePitchByWidth format not supported %d!!!", lpMyImageInfo->format);
                break;
        }
    }

    static int LoadYuvImageFromFile(const char *sPath, LPMyImageInfo lpMyImageInfo) {
        LOGD("LoadYuvImageFromFile sPath = %s", sPath);
        CHECK_NULL_INPUT(sPath)
        CHECK_NULL_INPUT(lpMyImageInfo)
        int ret = ERROR_OK;
        do {
            std::string filePath = sPath;
            std::vector<std::string> strTemp0 = MyFileHelper::StringSplit(filePath, '.', '.');
            for (auto val: strTemp0)
                LOGD("LoadYuvImageFromFile %s", val.c_str());
            if (strTemp0.size() != 2) {
                LOGE("LoadYuvImageFromFile sPath is not supported strTemp0");
                ret = ERROR_INPUT;
                break;
            }
            memset(lpMyImageInfo, 0, sizeof(MyImageInfo));
            GetImageFormatByExt(strTemp0[1], lpMyImageInfo->format);
            LOGD("LoadYuvImageFromFile GetImageFormatByExt nFormat = %d", lpMyImageInfo->format);

            std::vector<std::string> strTemp1 = MyFileHelper::StringSplit(strTemp0[0], 'x', 'X');
            if (strTemp1.size() != 2) {
                LOGE("LoadYuvImageFromFile sPath is not supported   strTemp1");
                ret = ERROR_INPUT;
                break;
            }
            lpMyImageInfo->height = atoi(strTemp1[1].c_str());
            LOGD("LoadYuvImageFromFile height = %d", lpMyImageInfo->height);

            std::vector<std::string> strTemp2 = MyFileHelper::StringSplit(strTemp1[0], '_', '_');
            if (0 == strTemp2.size()) {
                LOGE("LoadYuvImageFromFile sPath is not supported   strTemp2");
                ret = ERROR_INPUT;
                break;
            }
            lpMyImageInfo->width = atoi(strTemp2[strTemp2.size() - 1].c_str());
            AssignNativeImagePitchByWidth(lpMyImageInfo);
            long lSize = AllocNativeImage(lpMyImageInfo);
            if (0 == lSize) {
                ret = ERROR_MEMORY;
                break;
            }

            FILE *fp = nullptr;
            fp = fopen(sPath, "rb");
            if (fp) {
                fread(lpMyImageInfo->ppBuffer[0], 1, lSize, fp);
                fclose(fp);
            } else {
                ret = ERROR_FILE_NOT_EXIT;
                break;
            }

        } while (false);

        if (ERROR_OK != ret)
            FreeNativeImage(lpMyImageInfo);

        return ret;
    }

    static void GetImageSaveNameWithFolder(const char *pFolder, const LPMyImageInfo lpMyImage,
                                           char *szSaveName) {
        CHECK_NULL_INPUT_VOID(pFolder);
        CHECK_NULL_INPUT_VOID(lpMyImage);
        CHECK_NULL_INPUT_VOID(szSaveName);

        char szExt[PATH_LEN_MAX]{0};
        GetImageFileExtByFormat(lpMyImage->format, szExt);

        long long llTimeStamp = MyTimeUtils::GetCurrentTime();
        sprintf(szSaveName, "%s/Image_0_%dx%d_%lld%s", pFolder, lpMyImage->wPitch[0],
                lpMyImage->height, llTimeStamp, szExt);
    }

    static RESULT SaveYuvImageToFile(const MyImageInfo *pMyImageInfo, const char *sPath) {
        AUTO_COUNT_TIME_COST("SaveYuvImageToFile")

        LOGD("SaveYuvImageToFile sPath = %s", sPath);
        CHECK_NULL_INPUT(pMyImageInfo)
        CHECK_NULL_INPUT(sPath)

        if (!IsNativeImageValid(pMyImageInfo)) {
            return ERROR_INPUT;
        }

        int ret = ERROR_OK;
        FILE *fp = fopen(sPath, "wb");
        do {
            if (!fp) {
                ret = ERROR_FILE_COMMON;
                LOGE("SaveYuvImageToFile, open file failed, sPath = %s", sPath);
                break;
            }

            unsigned int uLength = 0;
            switch (pMyImageInfo->format) {
                case MY_FORMAT_NV21:
                case MY_FORMAT_NV12:
                    uLength = pMyImageInfo->wPitch[0] * pMyImageInfo->height * 3 / 2;
                    break;
                case MY_FORMAT_RGB24:
                case MY_FORMAT_RGB32:
                    uLength = pMyImageInfo->wPitch[0] * pMyImageInfo->height;
                    break;
                case MY_FORMAT_GRAY:
                case MY_FORMAT_GRAY10LE:
                    uLength = pMyImageInfo->wPitch[0] * pMyImageInfo->height;
                    break;
                default:
                    LOGE("SaveYuvImageToFile, image format is not supported!!!");
                    break;
            }

            if (uLength > 0)
                fwrite(pMyImageInfo->ppBuffer[0], 1, uLength, fp);

        } while (false);

        if (fp) {
            fclose(fp);
        }
        return ret;
    }

    /*static void GetImageFormatByFileName(const std::string &strFileName, int &dstFormat)
    {

    }*/

    static void GetImageFormatByExt(const std::string sExt, int &nFormat) {
        if ("NV12" == sExt || "nv12" == sExt)
            nFormat = MY_FORMAT_NV12;
        else if ("NV21" == sExt || "nv21" == sExt)
            nFormat = MY_FORMAT_NV21;
        else if ("P010_LSB" == sExt || "p010_lsb" == sExt)
            nFormat = MY_FORMAT_P010_LSB;
        else if ("P010_MSB" == sExt || "p010_msb" == sExt)
            nFormat = MY_FORMAT_P010_MSB;
        else if ("I420" == sExt || "i420" == sExt)
            nFormat = MY_FORMAT_NV21;
        else if ("GRAY" == sExt || "gray" == sExt)
            nFormat = MY_FORMAT_GRAY;
        else if ("GRAY10LE" == sExt || "gray10le" == sExt)
            nFormat = MY_FORMAT_GRAY10LE;
        else if ("RGB24" == sExt || "rgb24" == sExt)
            nFormat = MY_FORMAT_RGB24;
        else if ("RGB32" == sExt || "rgb32" == sExt)
            nFormat = MY_FORMAT_RGB32;
        else
            LOGE("GetImageFormatByExt unsupported format");
        LOGD ("GetImageFormatByExt sExt = %s", sExt.c_str());
    }

private:
  static void CalImagePlaneSize(const MyImageInfo *pImage, unsigned int bufSizeList[4]) {
    if (nullptr == pImage)
      return;
    switch (pImage->format) {
      case MY_FORMAT_NV12:
      case MY_FORMAT_NV21:
      case MY_FORMAT_P010_LSB:
      case MY_FORMAT_P010_MSB:
        bufSizeList[0] = pImage->wPitch[0] * pImage->height;
        bufSizeList[1] = pImage->wPitch[1] * pImage->height;
        break;
      case MY_FORMAT_I420:
        bufSizeList[0] = pImage->wPitch[0] * pImage->height;
        bufSizeList[1] = pImage->wPitch[1] * pImage->height;
        bufSizeList[2] = pImage->wPitch[2] * pImage->height;
      case MY_FORMAT_GRAY:
      case MY_FORMAT_GRAY10LE:
      case MY_FORMAT_RGB24:
      case MY_FORMAT_RGB32:
        bufSizeList[0] = pImage->wPitch[0] * pImage->height;
        break;
      default:
        LOGE("CalImagePlaneSize format is not support!!!");
        break;
    }
  }
    static void GetImageFileExtByFormat(const int &imgFormat, char *szExt) {
        CHECK_NULL_INPUT_VOID(szExt);

        switch (imgFormat) {
            case MY_FORMAT_NV12:
                sprintf(szExt, "%s", ".NV12");
                break;
            case MY_FORMAT_NV21:
                sprintf(szExt, "%s", ".NV21");
                break;
            case MY_FORMAT_I420:
                sprintf(szExt, "%s", ".I420");
                break;
            case MY_FORMAT_GRAY:
                sprintf(szExt, "%s", ".GRAY");
                break;
            case MY_FORMAT_GRAY10LE:
                sprintf(szExt, "%s", ".GRAY10LE");
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
};