//
// Created by bitstream1024 on 2022/1/25.
//

#pragma once

#include "KitImage.h"
#include "KitLogUtils.h"

class KitImageUtils {
 public:
  class ImageConverter {
   public:
    static void ConvertImage(const KitImage *pSrcImg, KitImage *pDstImg) {

    }

    static void ConvertRGB32ToNV12(const KitImage *pSrcImg, KitImage *pDstImg) {

    }

    static void ConvertRGB32ToNV21(const KitImage *pSrcImg, KitImage *pDstImg) {

    }

    static void ConvertGray10ToGray(const KitImage *pSrcImg, KitImage *pDstImg) {
      if (nullptr == pSrcImg || nullptr == pDstImg || KIT_FMT_GRAY10LE != pSrcImg->format
          || KIT_FMT_GRAY != pDstImg->format) {
        return;
      }

    }
  };

 public:

  static void PrintImageInfo(const KitImage *pSrcImg) {
    LOGV("PrintImageInfo pSrcImg: %p", pSrcImg);
    if (nullptr != pSrcImg) {
      LOGV(
          "PrintImageInfo pSrcImg: %d, %d, %d, wStride(%d,%d,%d,%d), hStride(%d,%d,%d,%d), data(%p,%p,%p,%p)",
          pSrcImg->width,
          pSrcImg->height,
          pSrcImg->format,
          pSrcImg->wStride[0],
          pSrcImg->wStride[1],
          pSrcImg->wStride[2],
          pSrcImg->wStride[3],
          pSrcImg->hStride[0],
          pSrcImg->hStride[1],
          pSrcImg->hStride[2],
          pSrcImg->hStride[3],
          pSrcImg->data[0],
          pSrcImg->data[1],
          pSrcImg->data[2],
          pSrcImg->data[3]);
    }
  }

  static bool IsImageValid(const KitImage *pSrcImg) {
    if (nullptr == pSrcImg || nullptr == pSrcImg->data[0] || pSrcImg->width <= 0 ||
        pSrcImg->height <= 0)
      return false;

    return true;
  }

  static bool IsImageSameSize(const KitImage *pSrcImg, const KitImage *pDstImg) {
    if (!IsImageValid(pSrcImg) || !IsImageValid(pDstImg))
      return false;

    if (pSrcImg->width != pDstImg->width || pSrcImg->height != pDstImg->height ||
        pSrcImg->format != pDstImg->format)
      return false;

    bool bOK = true;

    for (int i = 0; i < 4; ++i) {
      if (pSrcImg->wStride[i] != pDstImg->wStride[i] ||
          pSrcImg->hStride[i] != pDstImg->hStride[i]) {
        bOK = false;
        break;
      }
    }
    return bOK;
  }

  static void ZeroImage(LPKitImage lpMyImageInfo) {
    if (nullptr == lpMyImageInfo) {
      return;
    }
    memset(lpMyImageInfo, 0, sizeof(KitImage));
  }

  static unsigned int AllocImage(KitImage *pSrcImg) {
    AUTO_COUNT_TIME_COST("AllocImage");

    if (nullptr == pSrcImg)
      return 0;

    if (nullptr != pSrcImg->data[0]) {
      FreeImage(pSrcImg);
    }

    RESULT nRet = NONE_ERROR;
    unsigned int bufferSize[4] = {0};
    unsigned int totalSize = 0;
    switch (pSrcImg->format) {
      case KIT_FMT_NV21:
      case KIT_FMT_NV12:
      case KIT_FMT_P010_LSB:bufferSize[0] = pSrcImg->wStride[0] * pSrcImg->height;
        bufferSize[1] = pSrcImg->wStride[1] * pSrcImg->height;
        totalSize = bufferSize[0] + bufferSize[1];
        pSrcImg->data[0] = static_cast<unsigned char *>(malloc(totalSize));
        memset(pSrcImg->data[0], 0, totalSize);
        pSrcImg->data[1] = pSrcImg->data[0] + bufferSize[0];
        break;
      case KIT_FMT_I420: {
        int yLen = pSrcImg->wStride[0] * pSrcImg->height;
        totalSize = yLen + (yLen << 1);
        pSrcImg->data[0] = static_cast<unsigned char *>(malloc(totalSize));
        memset(pSrcImg->data[0], 0, totalSize);
        pSrcImg->data[1] = pSrcImg->data[0] + (yLen >> 2);
        pSrcImg->data[2] = pSrcImg->data[1] + (yLen >> 2);
      }
        break;
      case KIT_FMT_GRAY:
      case KIT_FMT_GRAY10LE:totalSize = pSrcImg->wStride[0] * pSrcImg->height;
        pSrcImg->data[0] = static_cast<unsigned char *>(malloc(totalSize));
        memset(pSrcImg->data[0], 0, totalSize);
        break;
      case KIT_FMT_RGB32:
      case KIT_FMT_RGB24:totalSize = pSrcImg->wStride[0] * pSrcImg->height;
        pSrcImg->data[0] = static_cast<unsigned char *>(malloc(totalSize));
        memset(pSrcImg->data[0], 0, totalSize);
        break;
      default:LOGE("AllocImage failed, not support format!!!");
        nRet = ERROR_IMAGE;
        break;
    }

    LOGE("AllocImage totalSize: %d, nRet = %d", totalSize, nRet);
    return totalSize;
  }

  static void FreeImage(KitImage *pSrcImg) {
    if (IsImageValid(pSrcImg))
      return;
    SafeFree(pSrcImg->data[0])
  }

  static RESULT CopyImageToDst(const KitImage *pSrcImg, KitImage *pDstImg) {
    AUTO_COUNT_TIME_COST("CopyImageToDst");

    if (!IsImageValid(pSrcImg) || !IsImageValid(pDstImg) ||
        !IsImageSameSize(pSrcImg, pDstImg)) {
      return ERROR_IMAGE;
    }

    unsigned int bufSizeList[4]{0};
    CalImagePlaneSize(pSrcImg, bufSizeList);
    switch (pSrcImg->format) {
      case KIT_FMT_NV12:
      case KIT_FMT_NV21:
      case KIT_FMT_P010_LSB:
      case KIT_FMT_P010_MSB:memcpy(pDstImg->data[0], pSrcImg->data[0], bufSizeList[0]);
        memcpy(pDstImg->data[1], pSrcImg->data[1], bufSizeList[1]);
        break;
      case KIT_FMT_I420:memcpy(pDstImg->data[0], pSrcImg->data[0], bufSizeList[0]);
        memcpy(pDstImg->data[1], pSrcImg->data[1], bufSizeList[1]);
        memcpy(pDstImg->data[2], pSrcImg->data[2], bufSizeList[2]);
        break;
      case KIT_FMT_RGB32:
      case KIT_FMT_RGB24:
      case KIT_FMT_GRAY:
      case KIT_FMT_GRAY10LE:memcpy(pDstImg->data[0], pSrcImg->data[0], bufSizeList[0]);
        break;
      default:LOGE("CopyImageToDst failed, not support format!!!");
        break;
    }

    return NONE_ERROR;
  }

  static void AssignImageStrideByWidth(LPKitImage lpMyImageInfo) {
    CHECK_NULL_INPUT_VOID(lpMyImageInfo);

    unsigned int bufSizeList[4]{0};
    CalImagePlaneSize(lpMyImageInfo, bufSizeList);
    switch (lpMyImageInfo->format) {
      case KIT_FMT_NV12:
      case KIT_FMT_NV21:lpMyImageInfo->wStride[0] = lpMyImageInfo->width;
        lpMyImageInfo->wStride[1] = lpMyImageInfo->width;
        break;
      case KIT_FMT_P010_LSB:
      case KIT_FMT_P010_MSB:lpMyImageInfo->wStride[0] = lpMyImageInfo->width << 1;
        lpMyImageInfo->wStride[1] = lpMyImageInfo->width << 1;
        break;
      case KIT_FMT_I420:lpMyImageInfo->wStride[0] = lpMyImageInfo->width;
        lpMyImageInfo->wStride[1] = lpMyImageInfo->width >> 1;
        lpMyImageInfo->wStride[2] = lpMyImageInfo->width >> 1;
        break;
      case KIT_FMT_GRAY:lpMyImageInfo->wStride[0] = lpMyImageInfo->width;
        break;
      case KIT_FMT_GRAY10LE:lpMyImageInfo->wStride[0] = lpMyImageInfo->width << 1;
        break;
      case KIT_FMT_RGB24:lpMyImageInfo->wStride[0] = lpMyImageInfo->width * 3;
        break;
      case KIT_FMT_RGB32:lpMyImageInfo->wStride[0] = lpMyImageInfo->width * 4;
        break;
      default:LOGE("AssignImageStrideByWidth format not supported %d!!!",
                   lpMyImageInfo->format);
        break;
    }
  }

  static int LoadYuvImageFromFile(const char *sPath, LPKitImage lpMyImageInfo) {
    LOGD("LoadYuvImageFromFile sPath = %s", sPath);
    CHECK_NULL_INPUT(sPath)
    CHECK_NULL_INPUT(lpMyImageInfo)
    int ret = NONE_ERROR;
    do {
      std::string filePath = sPath;
      std::vector<std::string> strTemp0 = KitFileUtils::StringSplit(filePath, '.', '.');
      for (auto val : strTemp0)
        LOGD("LoadYuvImageFromFile %s", val.c_str());
      if (strTemp0.size() != 2) {
        LOGE("LoadYuvImageFromFile sPath is not supported strTemp0");
        ret = ERROR_INPUT;
        break;
      }
      memset(lpMyImageInfo, 0, sizeof(KitImage));
      GetImageFormatByExt(strTemp0[1], lpMyImageInfo->format);
      LOGD("LoadYuvImageFromFile GetImageFormatByExt nFormat = %d", lpMyImageInfo->format);

      std::vector<std::string> strTemp1 = KitFileUtils::StringSplit(strTemp0[0], 'x', 'X');
      if (strTemp1.size() != 2) {
        LOGE("LoadYuvImageFromFile sPath is not supported   strTemp1");
        ret = ERROR_INPUT;
        break;
      }
      lpMyImageInfo->height = atoi(strTemp1[1].c_str());
      LOGD("LoadYuvImageFromFile height = %d", lpMyImageInfo->height);

      std::vector<std::string> strTemp2 = KitFileUtils::StringSplit(strTemp1[0], '_', '_');
      if (0 == strTemp2.size()) {
        LOGE("LoadYuvImageFromFile sPath is not supported   strTemp2");
        ret = ERROR_INPUT;
        break;
      }
      lpMyImageInfo->width = atoi(strTemp2[strTemp2.size() - 1].c_str());
      AssignImageStrideByWidth(lpMyImageInfo);
      long lSize = AllocImage(lpMyImageInfo);
      if (0 == lSize) {
        ret = ERROR_MEMORY;
        break;
      }

      FILE *fp = nullptr;
      fp = fopen(sPath, "rb");
      if (fp) {
        fread(lpMyImageInfo->data[0], 1, lSize, fp);
        fclose(fp);
      } else {
        ret = ERROR_FILE_NOT_EXIT;
        break;
      }

    } while (false);

    if (NONE_ERROR != ret)
      FreeImage(lpMyImageInfo);

    return ret;
  }

  static void GetImageSaveName(const char *pFolder, const LPKitImage lpMyImage, char *szSaveName) {
    CHECK_NULL_INPUT_VOID(pFolder);
    CHECK_NULL_INPUT_VOID(lpMyImage);
    CHECK_NULL_INPUT_VOID(szSaveName);

    char szExt[PATH_LEN_MAX]{0};
    GetImageFileExtByFormat(lpMyImage->format, szExt);

    long long llTimeStamp = MyTimeUtils::GetCurrentTime();
    sprintf(szSaveName, "%s/Image_0_%dx%d_%lld%s", pFolder, lpMyImage->wStride[0],
            lpMyImage->height, llTimeStamp, szExt);
  }

  static RESULT SaveYuvImageToFile(const KitImage *pMyImageInfo, const char *sPath) {
    AUTO_COUNT_TIME_COST("SaveYuvImageToFile")

    LOGD("SaveYuvImageToFile sPath = %s", sPath);
    CHECK_NULL_INPUT(pMyImageInfo)
    CHECK_NULL_INPUT(sPath)

    if (!IsImageValid(pMyImageInfo)) {
      return ERROR_INPUT;
    }

    int ret = NONE_ERROR;
    FILE *fp = fopen(sPath, "wb");
    do {
      if (!fp) {
        ret = ERROR_FILE_COMMON;
        LOGE("SaveYuvImageToFile, open file failed, sPath = %s", sPath);
        break;
      }

      unsigned int uLength = 0;
      switch (pMyImageInfo->format) {
        case KIT_FMT_NV21:
        case KIT_FMT_NV12:uLength = pMyImageInfo->wStride[0] * pMyImageInfo->height * 3 / 2;
          break;
        case KIT_FMT_RGB24:
        case KIT_FMT_RGB32:uLength = pMyImageInfo->wStride[0] * pMyImageInfo->height;
          break;
        case KIT_FMT_GRAY:
        case KIT_FMT_GRAY10LE:uLength = pMyImageInfo->wStride[0] * pMyImageInfo->height;
          break;
        default:LOGE("SaveYuvImageToFile, image format is not supported!!!");
          break;
      }

      if (uLength > 0)
        fwrite(pMyImageInfo->data[0], 1, uLength, fp);

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
      nFormat = KIT_FMT_NV12;
    else if ("NV21" == sExt || "nv21" == sExt)
      nFormat = KIT_FMT_NV21;
    else if ("P010_LSB" == sExt || "p010_lsb" == sExt)
      nFormat = KIT_FMT_P010_LSB;
    else if ("P010_MSB" == sExt || "p010_msb" == sExt)
      nFormat = KIT_FMT_P010_MSB;
    else if ("I420" == sExt || "i420" == sExt)
      nFormat = KIT_FMT_NV21;
    else if ("GRAY" == sExt || "gray" == sExt)
      nFormat = KIT_FMT_GRAY;
    else if ("GRAY10LE" == sExt || "gray10le" == sExt)
      nFormat = KIT_FMT_GRAY10LE;
    else if ("RGB24" == sExt || "rgb24" == sExt)
      nFormat = KIT_FMT_RGB24;
    else if ("RGB32" == sExt || "rgb32" == sExt)
      nFormat = KIT_FMT_RGB32;
    else
      LOGE("GetImageFormatByExt unsupported format");
    LOGD ("GetImageFormatByExt sExt = %s", sExt.c_str());
  }

 private:
  static void CalImagePlaneSize(const KitImage *pImage, unsigned int bufSizeList[4]) {
    if (nullptr == pImage)
      return;
    switch (pImage->format) {
      case KIT_FMT_NV12:
      case KIT_FMT_NV21:
      case KIT_FMT_P010_LSB:
      case KIT_FMT_P010_MSB:bufSizeList[0] = pImage->wStride[0] * pImage->height;
        bufSizeList[1] = pImage->wStride[1] * pImage->height;
        break;
      case KIT_FMT_I420:bufSizeList[0] = pImage->wStride[0] * pImage->height;
        bufSizeList[1] = pImage->wStride[1] * pImage->height;
        bufSizeList[2] = pImage->wStride[2] * pImage->height;
      case KIT_FMT_GRAY:
      case KIT_FMT_GRAY10LE:
      case KIT_FMT_RGB24:
      case KIT_FMT_RGB32:bufSizeList[0] = pImage->wStride[0] * pImage->height;
        break;
      default:LOGE("CalImagePlaneSize format is not support!!!");
        break;
    }
  }
  static void GetImageFileExtByFormat(const int &imgFormat, char *szExt) {
    CHECK_NULL_INPUT_VOID(szExt);

    switch (imgFormat) {
      case KIT_FMT_NV12:sprintf(szExt, "%s", ".NV12");
        break;
      case KIT_FMT_NV21:sprintf(szExt, "%s", ".NV21");
        break;
      case KIT_FMT_I420:sprintf(szExt, "%s", ".I420");
        break;
      case KIT_FMT_GRAY:sprintf(szExt, "%s", ".GRAY");
        break;
      case KIT_FMT_GRAY10LE:sprintf(szExt, "%s", ".GRAY10LE");
        break;
      case KIT_FMT_RGB24:sprintf(szExt, "%s", ".RGB24");
        break;
      case KIT_FMT_RGB32:sprintf(szExt, "%s", ".RGB32");
        break;
      default:sprintf(szExt, "%s", ".undefined");
        LOGE("GetImageFileExtByFormat image format is not supported!!!");
        break;
    }
  }
};