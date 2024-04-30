//
// Created by bitstream1024 on 2022/2/10.
//

#pragma once

#define MY_STR(_str_) #_str_
#define MY_STR_CONCATENATE(_str1_, _str2_) _str1_ ## _str2_

#if defined(ANDROID)
#define ROOT_FOLDER                     "/sdcard/OpenGLESTest/"
#else
#define ROOT_FOLDER                     "OpenGLESTest"
#endif

#define STR_IMAGE_PNG_TELEVISION        "television_0_1280x720.png"
#define STR_IMAGE_PNG_DOG               "dog_0_1280x1920.png"
#define STR_IMAGE_GRAY_DOG              "dog_0_1280x1920.gray"
#define STR_IMAGE_GRAY_10BIT_DOG        "dog_1_1280x1920.gray10le"
#define STR_IMAGE_NV12_DOG              "dog_1_1280x1920.NV12"
#define STR_IMAGE_P010_LSB_DOG          "dog_1_1280x1920.P010_LSB"
#define STR_IMAGE_P010_NIGHT            "night_0_1280x720.P010_LSB"
#define STR_IMAGE_NV21_NIGHT            "night_0_1280x720.NV21"
#define STR_IMAGE_NV12_NIGHT            "night_0_1280x720.NV12"

/*#define TEST_IMAGE_PATH	            MY_STR_CONCATENATE(ROOT_FOLDER, TEST_IMAGE_DOG_PNG)
#define TEST_IMAGE_PATH_YUV_GRAY	    MY_STR_CONCATENATE(ROOT_FOLDER, TEST_IMAGE_DOG_GRAY)
#define TEST_IMAGE_PATH_YUV_16Bit	    MY_STR_CONCATENATE(ROOT_FOLDER, TEST_IMAGE_DOG_GRAY_16Bit)*/
