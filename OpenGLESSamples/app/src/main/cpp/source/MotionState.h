//
// Created by wcg3031 on 2020/6/18.
//

#pragma once

#include <LogAndroid.h>
#include <string.h>

enum MotionType
{
	eMOTION_UNKNOWN = 0,
	eMOTION_ROTATE,
	eMOTION_SCALE,
	eMOTION_TRANSLATE
};

typedef struct _tag_motion_state_
{
	MotionType eMotionType;
	float transform_x;
	float transform_y;
	float transform_z;

	_tag_motion_state_ ()
	{
		setZero();
	};

	_tag_motion_state_ (const MotionType type, const float x, const float y, const float z)
	{
		eMotionType = type;
		transform_x = x;
		transform_y = y;
		transform_z = z;
	}

	_tag_motion_state_ (const _tag_motion_state_* pMotionState)
	{
		memcpy(this, pMotionState, sizeof(_tag_motion_state_));
	}

	void setZero ()
	{
		memset(this, 0, sizeof(_tag_motion_state_));
	}

	void logMotionState (const char* logInfo)
	{
		LOGD ("%s eMotionType = %d, transform (%f, %f, %f)", logInfo, eMotionType, transform_x,
				transform_y, transform_z);
	}

}MotionState, *LPMotionState;



