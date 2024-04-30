//
// Created by chauncy on 2020/4/3.
//

#pragma once

#include "png.h"
#include "LogAndroid.h"
#include "MyDefineUtils.h"
#include "MyImageInfo.h"
#include "MyFileHelper.h"

#define CHECK_PNG_RET_BREAK(_pRet_)	if (0 == (_pRet_)) {LOGE ("CHECK_PNG_RET_BREAK error"); (_pRet_) = ERROR_UNKNOWN; break;}

class OpenImageHelper
{

private:
	/**
	 * Calculate image ppBuffer length
	 * @param lpMyImageInfo
	 * @return Size of image ppBuffer
	 */
	static long CalMyImageBufferLength (const LPMyImageInfo lpMyImageInfo)
	{
		LOGD("CalMyImageBufferLength");
		if (NULL == lpMyImageInfo || 0 == lpMyImageInfo->width || 0 == lpMyImageInfo->height)
		{
			LOGE("AllocMyImageInfo m_pImageRGBA wrong");
			return 0;
		}
		int lSize = 0;
		switch (lpMyImageInfo->format)
		{
			case MY_FORMAT_RGB24:
			case MY_FORMAT_RGB32:
				lSize = lpMyImageInfo->wPitch[0] * lpMyImageInfo->height;
				break;
			case MY_FORMAT_NV12:
			case MY_FORMAT_NV21:
				lSize = (long)(lpMyImageInfo->wPitch[0] * lpMyImageInfo->height * 3/2);
			default:
				break;
		}
		return lSize;
	}

public:
	/**
	 * Alloc MyImageInfo, ppBuffer of image should be null
	 * @param lpMyImageInfo
	 * @return
	 */
	static long AllocMyImageInfo (LPMyImageInfo lpMyImageInfo)
	{
		LOGD("AllocMyImageInfo");
		if (NULL == lpMyImageInfo || 0 == lpMyImageInfo->width || 0 == lpMyImageInfo->wPitch[0]
			|| 0 == lpMyImageInfo->height || NULL != lpMyImageInfo->ppBuffer[0])
		{
			LOGE("AllocMyImageInfo m_pImageRGBA wrong");
			return ERROR_INPUT;
		}

		ERROR_CODE ret = ERROR_OK;
		long lSize = 0;
		lSize = CalMyImageBufferLength(lpMyImageInfo);
		LOGD("AllocMyImageInfo CalMyImageBufferLength lSize = %ld", lSize);
		if (0 == lSize)
		{
			LOGE("AllocMyImageInfo lSize = 0");
			return ERROR_INPUT;
		}
		switch (lpMyImageInfo->format)
		{
			case MY_FORMAT_RGB24:
			case MY_FORMAT_RGB32:
				lpMyImageInfo->ppBuffer[0] = (unsigned char *) malloc(lSize);
				CHECK_MALLOC_BREAK(lpMyImageInfo->ppBuffer[0], &ret, "AllocMyImageInfo MY_FORMAT_RGB24 MY_FORMAT_RGB32");
				memset(lpMyImageInfo->ppBuffer[0], 0, lSize);
				break;
			case MY_FORMAT_NV12:
			case MY_FORMAT_NV21:
				lpMyImageInfo->ppBuffer[0] = (unsigned char*) malloc(lSize);
				CHECK_MALLOC_BREAK(lpMyImageInfo->ppBuffer[0], &ret, "AllocMyImageInfo MY_FORMAT_NV12 MY_FORMAT_NV21");
				memset(lpMyImageInfo->ppBuffer[0], 0, lSize);
				lpMyImageInfo->ppBuffer[1] = lpMyImageInfo->ppBuffer[0] + lpMyImageInfo->wPitch[0] * lpMyImageInfo->height;
				break;
			default:
				break;
		}
		return lSize;
	}

	static void FreeMyImageInfo (LPMyImageInfo lpMyImageInfo)
	{
		LOGD("FreeMyImageInfo");
        if (lpMyImageInfo && lpMyImageInfo->ppBuffer[0]) {
          SafeFree(lpMyImageInfo->ppBuffer[0]);
          memset(lpMyImageInfo, 0, sizeof(MyImageInfo));
        }
	}

	/**
	 *
	 * @param sPath
	 * @param lpMyImageInfo
	 * @return
	 */
	static int LoadPngFromFile (const char* sPath, LPMyImageInfo lpMyImageInfo)
	{
		LOGD("LoadPngFromFile %s", sPath);
		CHECK_NULL_INPUT(sPath)
		CHECK_NULL_INPUT(lpMyImageInfo)
		int ret = 0;
		png_image image {0};
		image.version = PNG_IMAGE_VERSION;
		png_bytep buffer = NULL;
		do
		{
			ret = png_image_begin_read_from_file (&image, sPath);
			LOGD("LoadPngFromFile png_image_begin_read_from_file ret = %d", ret);
			CHECK_PNG_RET_BREAK (ret)
			image.format = PNG_FORMAT_RGBA;
			buffer = (png_bytep)malloc(PNG_IMAGE_SIZE(image));
			CHECK_MALLOC_BREAK (buffer, &ret, "LoadPngFromFile ")
			memset(buffer, 0, PNG_IMAGE_SIZE(image));

			ret = png_image_finish_read(&image, NULL, buffer, 0, NULL);
			LOGD("LoadPngFromFile png_image_finish_read ret = %d", ret);
			CHECK_PNG_RET_BREAK (ret)

			/*ret = png_image_write_to_file(&image, "/sdcard/testlibpng.png", 0, ppBuffer, 0, NULL);
			LOGD("LoadPngFromFile png_image_write_to_file ret = %d", ret);
			CHECK_PNG_RET_BREAK (ret)*/

			CHECK_NULL_INPUT_BREAK (lpMyImageInfo, &ret, "LoadPngFromFile lpImageInfo")
			lpMyImageInfo->width = static_cast<int>(image.width);
			lpMyImageInfo->height = static_cast<int>(image.height);
			if (PNG_FORMAT_RGBA == image.format) {
				lpMyImageInfo->format = MY_FORMAT_RGB32;
                lpMyImageInfo->wPitch[0] = lpMyImageInfo->width * 4;
            } else {
                lpMyImageInfo->format = MY_FORMAT_RGB24;
                lpMyImageInfo->wPitch[0] = lpMyImageInfo->width * 3;
            }
			long lSize = 0;
			lSize = AllocMyImageInfo(lpMyImageInfo);
			if (0 == lSize)
			{
				LOGE("LoadPngFromFile AllocMyImageInfo error");
				break;
			}
			memcpy(lpMyImageInfo->ppBuffer[0], buffer, PNG_IMAGE_SIZE(image));

		} while(false);

		png_image_free(&image);
		SafeFree(buffer);

		if (1 == ret)
			ret = ERROR_OK;

		return ret;
	}

	static void PrintMyImageInfo (const LPMyImageInfo lpMyImageInfo, const char *logInfo = nullptr)
	{
		if (!lpMyImageInfo)
			return;
		if (logInfo)
			LOGD("%s OpenImageHelper::PrintMyImageInfo width = %d, height = %d, format = %d, wPitch = (%d, %d, %d, %d), ppBuffer[0] = %p",
                 logInfo, lpMyImageInfo->width, lpMyImageInfo->height, lpMyImageInfo->format, lpMyImageInfo->wPitch[0],
                 lpMyImageInfo->wPitch[1], lpMyImageInfo->wPitch[2], lpMyImageInfo->wPitch[3], lpMyImageInfo->ppBuffer[0]);
		else
			LOGD("OpenImageHelper::PrintMyImageInfo width = %d, height = %d, format = %d, wPitch = (%d, %d, %d, %d), ppBuffer[0] = %p",
                 lpMyImageInfo->width, lpMyImageInfo->height, lpMyImageInfo->format, lpMyImageInfo->wPitch[0],
                 lpMyImageInfo->wPitch[1], lpMyImageInfo->wPitch[2], lpMyImageInfo->wPitch[3], lpMyImageInfo->ppBuffer[0]);
	}

	static ERROR_CODE SaveImageToPng (const LPMyImageInfo lpMyImageInfo, const char* sPath)
	{
		AUTO_COUNT_TIME_COST("SaveImageToPng");
		CHECK_NULL_INPUT(lpMyImageInfo)
		CHECK_NULL_INPUT(lpMyImageInfo->ppBuffer[0])
		CHECK_NULL_INPUT(sPath)
		LOGD("SaveImageToPng sPath = %s", sPath);

		ERROR_CODE ret = ERROR_OK;

		png_image image {0};
		image.version = PNG_IMAGE_VERSION;
		image.width = static_cast<png_uint_32 >(lpMyImageInfo->width);
		image.height = static_cast<png_uint_32 >(lpMyImageInfo->height);
		if (MY_FORMAT_RGB32 == lpMyImageInfo->format) {
			image.format = PNG_FORMAT_RGBA;
		} else{
			image.format = PNG_FORMAT_RGB;
		}
		image.colormap_entries = 256;
		png_bytep buffer = lpMyImageInfo->ppBuffer[0];
		int png_ret = png_image_write_to_file(&image, sPath, 0, buffer, 0, NULL);
		LOGD("SaveImageToPng png_image_write_to_file ret = %d", png_ret);
		if (0 == png_ret)
		{
			LOGE("SaveImageToPng png_image_write_to_file warning_or_error = %d message = %s",
					image.warning_or_error, image.message);
			ret = ERROR_IMAGE;
		}
		return ret;
	}

	static void ExchangeImageCoordinateY (LPMyImageInfo lpMyImageInfo)
	{
		AUTO_COUNT_TIME_COST("ExchangeImageCoordinateY");
		if (NULL == lpMyImageInfo || NULL == lpMyImageInfo->ppBuffer[0])
		{
			LOGE("ExchangeImageCoordinateY ERROR_INPUT");
			return;
		}

		int channelNum = 0;
		switch (lpMyImageInfo->format)
		{
			case MY_FORMAT_NV12:
			case MY_FORMAT_NV21:
                channelNum = 2;
                break;
            case MY_FORMAT_RGB32:
				channelNum = 4;
				break;
			case MY_FORMAT_RGB24:
				channelNum = 3;
				break;
			default:
				break;
		}

		int pitch = lpMyImageInfo->wPitch[0];
		int height = lpMyImageInfo->height;
		int lineSize = pitch;
		unsigned char *lineBuffer = NULL;
		lineBuffer = (unsigned char *)malloc (lineSize);
		memset(lineBuffer, 0, lineSize);

		int num = height / 2;
		for (int i = 0; i < num; ++i)
		{
			memcpy(lineBuffer, lpMyImageInfo->ppBuffer[0] + i * lineSize, lineSize);
			memcpy(lpMyImageInfo->ppBuffer[0] + i * lineSize, (lpMyImageInfo->ppBuffer[0] + (height - i - 1) * lineSize), lineSize);
			memcpy((lpMyImageInfo->ppBuffer[0] + (height - i - 1) * lineSize), lineBuffer, lineSize);
			memset(lineBuffer, 0, sizeof(lineSize));
		}

		SafeFree(lineBuffer);
		return;
	}
};

