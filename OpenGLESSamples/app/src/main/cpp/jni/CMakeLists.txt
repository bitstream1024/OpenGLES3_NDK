# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

project(openglessamples)
include(config.cmake)

if(${IF_BUILD_FREETYPE_LIB} EQUAL 1)
    MESSAGE(STATUS "add_subdirectory: freetype")
    add_subdirectory(freetype)
endif()

add_subdirectory(sample)