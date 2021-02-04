//
// Created by chauncy on 2021/2/1.
//

#pragma once

class CommonUtils {

	static int getSdkVersion() {
		int sdkVersion = -1;
#ifdef __ANDROID__
		sdkVersion = getPropertyInteger("ro.build.version.sdk", -1);
#endif
		return sdkVersion;
	}
};