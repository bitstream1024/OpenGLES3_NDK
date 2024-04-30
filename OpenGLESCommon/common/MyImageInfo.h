#pragma once

#define MY_FORMAT_RGB24		    0x100
#define MY_FORMAT_RGB32		    0x200
#define MY_FORMAT_NV12		    0x300
#define MY_FORMAT_NV21		    0x301
#define MY_FORMAT_I420		    0x400
#define MY_FORMAT_GRAY		    0x500
#define MY_FORMAT_GRAY10LE	    0x501
#define MY_FORMAT_P010_LSB	    0x601
#define MY_FORMAT_P010_MSB	    0x602

typedef struct __tag_image_info_
{
	int width;
	int height;
	int format;
	int wPitch[4];
	int hPitch[4];
	unsigned char *ppBuffer[4];
}MyImageInfo, *LPMyImageInfo;