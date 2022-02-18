//
// Created by chauncy on 2020/4/21.
//

#ifndef HELLOOPENGLES_COMMON_H
#define HELLOOPENGLES_COMMON_H

#define PATH_LEN_MAX     256

#define RESULT           int
typedef enum _enum_error_code_
{
	ERROR_OK = 0x00,
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
}ERROR_CODE;

typedef struct _tag_rect_
{
	int left;
	int top;
	int right;
	int bottom;
}SRECT, *LPSRECT;

#endif //HELLOOPENGLES_COMMON_H
