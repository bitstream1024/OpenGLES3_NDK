# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
#project
project (freetype)

#set the module so name
set (MODULE_SO_NAME  freetype)

#set the module output path
set (SO_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../libs/${TARGET_ABI})

add_definitions(" -D__LINUX_CPL__ -DANDROID_NDK -DUNICODE -DFT2_BUILD_LIBRARY")
#add_compile_options(-Wall)

include_directories(
                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/src
                    )

set(BASE_SRCS
        src/autofit/autofit.c
        src/base/ftbase.c
        src/base/ftbbox.c
        src/base/ftbdf.c
        src/base/ftbitmap.c
        src/base/ftcid.c
        src/base/ftfstype.c
        src/base/ftgasp.c
        src/base/ftglyph.c
        src/base/ftgxval.c
        src/base/ftinit.c
        src/base/ftmm.c
        src/base/ftotval.c
        src/base/ftpatent.c
        src/base/ftpfr.c
        src/base/ftstroke.c
        src/base/ftsynth.c
        src/base/fttype1.c
        src/base/ftwinfnt.c
        src/bdf/bdf.c
        src/bzip2/ftbzip2.c
        src/cache/ftcache.c
        src/cff/cff.c
        src/cid/type1cid.c
        src/gzip/ftgzip.c
        src/lzw/ftlzw.c
        src/pcf/pcf.c
        src/pfr/pfr.c
        src/psaux/psaux.c
        src/pshinter/pshinter.c
        src/psnames/psnames.c
        src/raster/raster.c
        src/sdf/sdf.c
        src/sfnt/sfnt.c
        src/smooth/smooth.c
        src/svg/svg.c
        src/truetype/truetype.c
        src/type1/type1.c
        src/type42/type42.c
        src/winfonts/winfnt.c
   )


add_library( ${MODULE_SO_NAME}
             STATIC
             ${BASE_SRCS}
             )






