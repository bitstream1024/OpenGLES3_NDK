#pragma once

#define KIT_FMT_RGB24           0x100
#define KIT_FMT_RGB32           0x200
#define KIT_FMT_NV12            0x300
#define KIT_FMT_NV21            0x301
#define KIT_FMT_I420            0x400
#define KIT_FMT_GRAY            0x500
#define KIT_FMT_GRAY10LE        0x501
#define KIT_FMT_P010_LSB        0x601
#define KIT_FMT_P010_MSB        0x602

typedef struct __tag_kit_image_ {
    int width;
    int height;
    int format;
    int wStride[4];
    int hStride[4]; // not used
    unsigned char *data[4];
} KitImage, *LPKitImage;