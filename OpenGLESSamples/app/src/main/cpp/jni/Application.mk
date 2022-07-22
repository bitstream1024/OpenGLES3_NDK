	APP_OPTIM:= release
	APP_CPPFLAGS:= --std=c++11 -frtti
	#for 32 & 64bits targets
#	NDK_TOOLCHAIN_VERSION := 4.9
#	APP_STL := gnustl_static
	NDK_TOOLCHAIN_VERSION:= clang
	APP_STL:= c++_static
	APP_PLATFORM:= android-24
    APP_ABI:= armeabi-v7a,arm64-v8a
#	armeabi-v7a,arm64-v8a,x86,x86_64