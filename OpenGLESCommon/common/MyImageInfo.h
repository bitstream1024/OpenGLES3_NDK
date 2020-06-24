#pragma once

#define MY_FORMAT_RGB		0x100
#define MY_FORMAT_RGBA		0x101
#define MY_FORMAT_NV12		0x102
#define MY_FORMAT_NV21		0x103

typedef struct __tag_image_info_
{
	int width;
	int height;
	int format;
	int channel[4];
	unsigned char *buffer[4];
}MyImageInfo, *LPMyImageInfo;