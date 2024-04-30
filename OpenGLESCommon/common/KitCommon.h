//
// Created by bitstream1024 on 2020/4/21.
//

#ifndef HELLOOPENGLES_KIT_COMMON_H
#define HELLOOPENGLES_KIT_COMMON_H

#define IF_DEBUG        1

#define PATH_LEN_MAX    256

#define RESULT          int
typedef enum _enum_error_code_ {
  NONE_ERROR = 0x00,
  ERROR_UNKNOWN,
  ERROR_INPUT,
  ERROR_MEMORY,
  ERROR_IMAGE,
  ERROR_GL_STATE,
  ERROR_EGL_STATE,
  ERROR_ASSIMP,
  ERROR_FILE_NOT_EXIT,
  ERROR_FILE_COMMON,
  ERROR_NATIVE_MEDIA
} KitErrorCode;

typedef struct _tag_kit_rect_ {
  int left;
  int top;
  int right;
  int bottom;
} KitRect, *LPKitRect;

#endif //HELLOOPENGLES_KIT_COMMON_H
