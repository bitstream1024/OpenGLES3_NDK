//
// Created by chauncy on 2020/4/1.
//

#pragma once

#include <MotionState.h>

typedef void	Handle;
typedef void* 	PHandle;

int CreateProcessor (PHandle *ppProcessorHandle);
int SetupResource (PHandle const pProcessorHandle);
int DestroyProcessor (PHandle *ppProcessorHandle);

int onSurfaceCreated (PHandle const pProcessorHandle, const int effectType = 0);
int onSurfaceChanged (PHandle const pProcessorHandle, const int width, const int height);
int setMotionState (PHandle const pProcessorHandle, MotionState const motionState);
int onDrawFrame (PHandle const pProcessorHandle);
int getTextureFromFrameBuffer (PHandle const pProcessorHandle);
int onSurfaceDestroyed (PHandle const pProcessorHandle);


