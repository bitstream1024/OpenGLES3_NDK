LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#set the module so name
MODULE_SO_NAME  := freetype

OUTPUT_LIBS_PATH :=$(LOCAL_PATH)/../../libs/$(TARGET_ARCH_ABI)

MY_INCLUDES_LISTS :=$(LOCAL_PATH)/include  \
				  	$(LOCAL_PATH)/src

LOCAL_SRC_FILES:= \
				  src/autofit/autofit.c \
				  src/base/ftbase.c \
				  src/base/ftbbox.c \
				  src/base/ftbdf.c \
				  src/base/ftbitmap.c \
				  src/base/ftcid.c \
				  src/base/ftfstype.c \
				  src/base/ftgasp.c \
				  src/base/ftglyph.c \
				  src/base/ftgxval.c \
				  src/base/ftinit.c \
				  src/base/ftmm.c \
				  src/base/ftotval.c \
				  src/base/ftpatent.c \
				  src/base/ftpfr.c \
				  src/base/ftstroke.c \
				  src/base/ftsynth.c \
				  src/base/ftsystem.c \
				  src/base/ftdebug.c \
				  src/base/fttype1.c \
				  src/base/ftwinfnt.c \
				  src/bdf/bdf.c \
				  src/bzip2/ftbzip2.c \
				  src/cache/ftcache.c \
				  src/cff/cff.c \
				  src/cid/type1cid.c \
				  src/gzip/ftgzip.c \
				  src/lzw/ftlzw.c \
				  src/pcf/pcf.c \
				  src/pfr/pfr.c \
				  src/psaux/psaux.c \
				  src/pshinter/pshinter.c \
				  src/psnames/psnames.c \
				  src/raster/raster.c \
				  src/sdf/sdf.c \
				  src/sfnt/sfnt.c \
				  src/smooth/smooth.c \
				  src/svg/svg.c \
				  src/truetype/truetype.c \
				  src/type1/type1.c \
				  src/type42/type42.c \
				  src/winfonts/winfnt.c

LOCAL_MODULE:= $(MODULE_SO_NAME)

#All header includes
LOCAL_C_INCLUDES := $(MY_INCLUDES_LISTS)

#only 32bits arm support
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_CFLAGS   +=-mfloat-abi=softfp -mfpu=neon
LOCAL_CPPFLAGS +=-mfloat-abi=softfp -mfpu=neon
LOCAL_ARM_NEON := true
endif

LOCAL_CFLAGS += -W -Wall
LOCAL_CFLAGS += -fPIC -DPIC
LOCAL_CFLAGS += "-DDARWIN_NO_CARBON"
LOCAL_CFLAGS += "-DFT2_BUILD_LIBRARY"
LOCAL_CFLAGS += -O2

include $(BUILD_STATIC_LIBRARY)

