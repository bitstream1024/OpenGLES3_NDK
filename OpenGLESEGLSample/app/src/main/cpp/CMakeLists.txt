# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

project(egl_project)

set(TARGET_ARCH_FLAG ${ANDROID_ABI})
set(TARGET_ABI ${TARGET_ARCH_FLAG})
message(STATUS "TARGET_ABI is ${TARGET_ABI}")

set(TARGET_LIB_NAME eglutils)

FILE(GLOB PROCESSOR_SRC_FILES *.cpp)
LIST(APPEND PROCESSOR_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/shader_helper/ShaderHelper.cpp)

link_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../OpenGLESCommon/libpng/lib/${TARGET_ABI}
)

add_library( ${TARGET_LIB_NAME}
             SHARED
             ${PROCESSOR_SRC_FILES}
        )

target_include_directories( ${TARGET_LIB_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../OpenGLESCommon/libpng/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../OpenGLESCommon/common
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../OpenGLESCommon/utils
        ${CMAKE_CURRENT_SOURCE_DIR}/shader_helper
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../OpenGLESCommon/glrender
        )

target_link_libraries( ${TARGET_LIB_NAME}
        EGL
        GLESv3
        png16
        log
        android
        )

add_custom_command(
        TARGET ${TARGET_LIB_NAME}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../OpenGLESCommon/libpng/lib/${TARGET_ABI}/libpng16.so ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/${TARGET_ABI}/libpng16.so
)