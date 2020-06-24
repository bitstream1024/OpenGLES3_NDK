//
// Created by wcg3031 on 2020/4/3.
//

#pragma once

#include "png.h"
#include "LogAndroid.h"
#include "MyDefineUtils.h"
#include "MyImageInfo.h"
#include "MyFileHelper.h"

#define CHECK_PNG_RET_BREAK(_pRet_)	if (0 == *(_pRet_)) {LOGE ("CHECK_PNG_RET_BREAK error"); *(_pRet_) = ERROR_UNKNOWN; break;}

class OpenImageHelper
{

private:
	/**
	 * Calculate image buffer length
	 * @param lpMyImageInfo
	 * @return Size of image buffer
	 */
	static long CalMyImageBufferLength (const LPMyImageInfo lpMyImageInfo)
	{
		LOGD("CalMyImageBufferLength");
		if (NULL == lpMyImageInfo || 0 == lpMyImageInfo->width || 0 == lpMyImageInfo->height)
		{
			LOGE("AllocMyImageInfo lpMyImageInfo wrong");
			return 0;
		}
		int lSize = 0;
		switch (lpMyImageInfo->format)
		{
			case MY_FORMAT_RGB:
				lSize = 3 * lpMyImageInfo->channel[0] * lpMyImageInfo->height;
				break;
			case MY_FORMAT_RGBA:
				lSize = 4 * lpMyImageInfo->channel[0] * lpMyImageInfo->height;
				break;
			case MY_FORMAT_NV12:
			case MY_FORMAT_NV21:
				lSize = (long)(1.5 * lpMyImageInfo->channel[0] * lpMyImageInfo->height);
			default:
				break;
		}
		return lSize;
	}

public:
	/**
	 * Alloc MyImageInfo, buffer of image should be null
	 * @param lpMyImageInfo
	 * @return
	 */
	static long AllocMyImageInfo (LPMyImageInfo lpMyImageInfo)
	{
		LOGD("AllocMyImageInfo");
		if (NULL == lpMyImageInfo || 0 == lpMyImageInfo->width || 0 == lpMyImageInfo->channel[0]
			|| 0 == lpMyImageInfo->height || NULL != lpMyImageInfo->buffer[0])
		{
			LOGE("AllocMyImageInfo lpMyImageInfo wrong");
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
			case MY_FORMAT_RGB:
			case MY_FORMAT_RGBA:
				lpMyImageInfo->buffer[0] = (unsigned char *) malloc(lSize);
				CHECK_MALLOC_BREAK(lpMyImageInfo->buffer[0], &ret, "AllocMyImageInfo MY_FORMAT_RGB MY_FORMAT_RGBA");
				memset(lpMyImageInfo->buffer[0], 0, lSize);
				break;
			case MY_FORMAT_NV12:
			case MY_FORMAT_NV21:
				lpMyImageInfo->buffer[0] = (unsigned char*) malloc(lSize);
				CHECK_MALLOC_BREAK(lpMyImageInfo->buffer[0], &ret, "AllocMyImageInfo MY_FORMAT_NV12 MY_FORMAT_NV21");
				memset(lpMyImageInfo->buffer[0], 0, lSize);
				lpMyImageInfo->buffer[1] = lpMyImageInfo->buffer[0] + lpMyImageInfo->channel[0] * lpMyImageInfo->height;
				break;
			default:
				break;
		}
		return lSize;
	}

	static ERROR_CODE FreeMyImageInfo (LPMyImageInfo lpMyImageInfo)
	{
		LOGD("FreeMyImageInfo");
		CHECK_NULL_INPUT(lpMyImageInfo)
		SafeFree(lpMyImageInfo->buffer[0]);
		memset(lpMyImageInfo, 0, sizeof(MyImageInfo));
		return ERROR_OK;
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
			CHECK_PNG_RET_BREAK (&ret)
			image.format = PNG_FORMAT_RGBA;
			buffer = (png_bytep)malloc(PNG_IMAGE_SIZE(image));
			CHECK_MALLOC_BREAK (buffer, &ret, "LoadPngFromFile ")
			memset(buffer, 0, PNG_IMAGE_SIZE(image));

			ret = png_image_finish_read(&image, NULL, buffer, 0, NULL);
			LOGD("LoadPngFromFile png_image_finish_read ret = %d", ret);
			CHECK_PNG_RET_BREAK (&ret)

			/*ret = png_image_write_to_file(&image, "/sdcard/testlibpng.png", 0, buffer, 0, NULL);
			LOGD("LoadPngFromFile png_image_write_to_file ret = %d", ret);
			CHECK_PNG_RET_BREAK (ret)*/

			CHECK_NULL_INPUT_BREAK (lpMyImageInfo, &ret, "LoadPngFromFile lpImageInfo")
			lpMyImageInfo->width = static_cast<int>(image.width);
			lpMyImageInfo->height = static_cast<int>(image.height);
			if (PNG_FORMAT_RGBA == image.format) {
				lpMyImageInfo->format = MY_FORMAT_RGBA;
			} else{
				lpMyImageInfo->format = MY_FORMAT_RGB;
			}
			lpMyImageInfo->channel[0] = lpMyImageInfo->width;
			long lSize = 0;
			lSize = AllocMyImageInfo(lpMyImageInfo);
			if (0 == lSize)
			{
				LOGE("LoadPngFromFile AllocMyImageInfo error");
				break;
			}
			memcpy(lpMyImageInfo->buffer[0], buffer, PNG_IMAGE_SIZE(image));

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
			LOGD("%s OpenImageHelper::PrintMyImageInfo width = %d, height = %d, format = %d, channel = (%d, %d, %d, %d), buffer[0] = %p",
					logInfo, lpMyImageInfo->width, lpMyImageInfo->height, lpMyImageInfo->format, lpMyImageInfo->channel[0],
					lpMyImageInfo->channel[1], lpMyImageInfo->channel[2], lpMyImageInfo->channel[3], lpMyImageInfo->buffer[0]);
		else
			LOGD("OpenImageHelper::PrintMyImageInfo width = %d, height = %d, format = %d, channel = (%d, %d, %d, %d), buffer[0] = %p",
				 lpMyImageInfo->width, lpMyImageInfo->height, lpMyImageInfo->format, lpMyImageInfo->channel[0],
				 lpMyImageInfo->channel[1], lpMyImageInfo->channel[2], lpMyImageInfo->channel[3], lpMyImageInfo->buffer[0]);
	}

	static int SaveImageToYuv (const LPMyImageInfo lpMyImageInfo, const char* sPath)
	{
		CAL_TIME_COST("SaveImage")
		CHECK_NULL_INPUT(lpMyImageInfo)
		CHECK_NULL_INPUT(lpMyImageInfo->buffer[0])
		CHECK_NULL_INPUT(sPath)
		LOGD("SaveImageToYuv sPath = %s", sPath);

		int ret = ERROR_OK;
		long lSize = CalMyImageBufferLength(lpMyImageInfo);
		FILE *fp = nullptr;
		fp = fopen(sPath, "wb");
		if (fp)
		{
			fwrite(lpMyImageInfo->buffer[0], 1, lSize, fp);
			fclose(fp);
		}
		else
		{
			ret = ERROR_FILE_COMMON;
		}
		return ret;
	}

	static ERROR_CODE SaveImageToPng (const LPMyImageInfo lpMyImageInfo, const char* sPath)
	{
		CAL_TIME_COST("SaveImageToPng");
		CHECK_NULL_INPUT(lpMyImageInfo)
		CHECK_NULL_INPUT(lpMyImageInfo->buffer[0])
		CHECK_NULL_INPUT(sPath)
		LOGD("SaveImageToPng sPath = %s", sPath);

		ERROR_CODE ret = ERROR_OK;

		png_image image {0};
		image.version = PNG_IMAGE_VERSION;
		image.width = static_cast<png_uint_32 >(lpMyImageInfo->width);
		image.height = static_cast<png_uint_32 >(lpMyImageInfo->height);
		if (MY_FORMAT_RGBA == lpMyImageInfo->format) {
			image.format = PNG_FORMAT_RGBA;
		} else{
			image.format = PNG_FORMAT_RGB;
		}
		image.colormap_entries = 256;
		png_bytep buffer = lpMyImageInfo->buffer[0];
		int png_ret = png_image_write_to_file(&image, sPath, 0, buffer, 0, NULL);
		LOGD("SaveImageToPng png_image_write_to_file ret = %d", ret);
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
		CAL_TIME_COST("ExchangeImageCoordinateY");
		if (NULL == lpMyImageInfo || NULL == lpMyImageInfo->buffer[0])
		{
			LOGE("ExchangeImageCoordinateY ERROR_INPUT");
			return;
		}

		int channelNum = 0;
		switch (lpMyImageInfo->format)
		{
			case MY_FORMAT_RGBA:
				channelNum = 4;
				break;
			case MY_FORMAT_RGB:
				channelNum = 3;
				break;
			default:
				break;
		}

		int pitch = lpMyImageInfo->channel[0];
		int height = lpMyImageInfo->height;
		int lineSize = channelNum * pitch;
		unsigned char *lineBuffer = NULL;
		lineBuffer = (unsigned char *)malloc (lineSize);
		memset(lineBuffer, 0, lineSize);

		int num = height / 2;
		for (int i = 0; i < num; ++i)
		{
			memcpy(lineBuffer, lpMyImageInfo->buffer[0] + i * lineSize, lineSize);
			memcpy(lpMyImageInfo->buffer[0] + i * lineSize, (lpMyImageInfo->buffer[0] + (height - i - 1) * lineSize), lineSize);
			memcpy((lpMyImageInfo->buffer[0] + (height - i - 1) * lineSize), lineBuffer, lineSize);
			memset(lineBuffer, 0, sizeof(lineSize));
		}

		SafeFree(lineBuffer);
		return;
	}

	static int LoadYuvFromFile (const char* sPath, LPMyImageInfo lpMyImageInfo)
	{
		LOGD("LoadYuvFromFile sPath = %s", sPath);
		CHECK_NULL_INPUT(sPath)
		CHECK_NULL_INPUT(lpMyImageInfo)
		int ret = ERROR_OK;
		do
		{
			std::string filePath = sPath;
			std::vector<std::string> strTemp0 = MyFileHelper::StringSplit (filePath, '.', '.');
			for (auto val:strTemp0)
				LOGD("LoadYuvFromFile %s", val.c_str());
			if (strTemp0.size() != 2)
			{
				LOGE("LoadYuvFromFile sPath is not supported   strTemp0");
				ret = ERROR_INPUT;
				break;
			}
			memset(lpMyImageInfo, 0, sizeof(MyImageInfo));
			GetImageFormatByExt(strTemp0[1], lpMyImageInfo->format);
			LOGD("LoadYuvFromFile GetImageFormatByExt nFormat = %d", lpMyImageInfo->format);

			std::vector<std::string> strTemp1 = MyFileHelper::StringSplit(strTemp0[0], 'x', 'X');
			if (strTemp1.size() != 2)
			{
				LOGE("LoadYuvFromFile sPath is not supported   strTemp1");
				ret = ERROR_INPUT;
				break;
			}
			lpMyImageInfo->height = atoi(strTemp1[1].c_str());
			LOGD("LoadYuvFromFile height = %d", lpMyImageInfo->height);

			std::vector<std::string> strTemp2 = MyFileHelper::StringSplit(strTemp1[0], '_', '_');
			if (0 == strTemp2.size())
			{
				LOGE("LoadYuvFromFile sPath is not supported   strTemp2");
				ret = ERROR_INPUT;
				break;
			}
			lpMyImageInfo->width = atoi(strTemp2[strTemp2.size()-1].c_str());
			lpMyImageInfo->channel[0] = lpMyImageInfo->width;
			LOGD("LoadYuvFromFile width = %d", lpMyImageInfo->width);
			if (MY_FORMAT_RGBA == lpMyImageInfo->format || MY_FORMAT_RGB == lpMyImageInfo->format)
			{
				lpMyImageInfo->channel[1] = lpMyImageInfo->width;
				lpMyImageInfo->channel[2] = lpMyImageInfo->width;
			}


			long lSize = AllocMyImageInfo(lpMyImageInfo);
			if (0 == lSize)
			{
				ret = ERROR_MEMORY;
				break;
			}

			FILE *fp = nullptr;
			fp = fopen(sPath, "rb");
			if (fp)
			{
				fread(lpMyImageInfo->buffer[0], 1, lSize, fp);
				fclose(fp);
			}
			else
			{
				ret = ERROR_FILE_NOT_EXIT;
				break;
			}

		} while (false);

		if (ERROR_OK != ret)
			FreeMyImageInfo(lpMyImageInfo);

		return ret;
	}

	static void GetImageFormatByExt (const std::string sExt, int &nFormat)
	{
		LOGD ("GetImageFormatByExt sExt = %s", sExt.c_str());
		if ("NV12" == sExt || "nv12" == sExt)
			nFormat = MY_FORMAT_NV12;
		else if ("NV21" == sExt || "nv21" == sExt)
			nFormat = MY_FORMAT_NV21;
		else if ("RGB24" == sExt || "rgb24" == sExt)
			nFormat = MY_FORMAT_RGB;
		else if ("RGB32" == sExt || "rgb32" == sExt)
			nFormat = MY_FORMAT_RGBA;
		else
			LOGE("GetImageFormatByExt unsupported format");
	}

};

