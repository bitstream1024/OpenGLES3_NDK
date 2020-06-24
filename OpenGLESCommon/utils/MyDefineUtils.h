//
// Created by wcg3031 on 2020/4/1.
//

#pragma once

#include "common.h"

#define MAX_PATH		256
#define MAX_CONTENT		512

#define SafeFree(_p_)				if (_p_) {free (_p_); _p_ = nullptr;}
#define SafeDelete(_p_)				if (_p_) {delete _p_; _p_ = nullptr;}
#define CHECK_NULL_MALLOC(_p_)		if (!_p_) {return ERROR_MEMORY;}
#define CHECK_NULL_INPUT(_p_)		if (!_p_) {return ERROR_INPUT;}

#define CHECK_NULL_INPUT_BREAK(_pVal_, _pRet_, _sInfoTitle_)		if (NULL == (_pVal_)) { \
																		*(_pRet_) = ERROR_INPUT; \
																		LOGE ("%s memory alloc failed", (_sInfoTitle_)); \
																		break; }

#define CHECK_MALLOC_BREAK(_p_, _pRet_, _sInfoTitle_)				if (NULL == _p_) { \
																		*(_pRet_) = ERROR_MEMORY; \
																		LOGE ("%s malloc failed", _sInfoTitle_); \
																		break;}

#define CHECK_OK_RETURN(_ret_)	if (ERROR_OK != (_ret_)) return _ret_;

