//
// Created by wcg3031 on 2020/4/2.
//

#ifndef HELLOTRIANGLE_UTILS_H
#define HELLOTRIANGLE_UTILS_H

#include "sys/time.h"

class Utils
{
	static void getCurrentTime (long *milliSecond)
	{
		struct timeval tv;
		gettimeofday (&tv, NULL);
		*milliSecond = tv.tv_sec * 1000 + tv.tv_usec/1000;
	}
};

#endif //HELLOTRIANGLE_UTILS_H
