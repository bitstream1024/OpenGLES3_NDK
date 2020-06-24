# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(TARGET_ARCH_FLAG ${ANDROID_ABI})
set(TARGET_ABI ${TARGET_ARCH_FLAG})
message(STATUS "TARGET_ABI is ${TARGET_ABI}")

set(PROCESSOR_SO_NAME processor.draw)

set(IF_USE_STATIC_PNG_LIB 1)
if (IF_USE_STATIC_PNG_LIB EQUAL 1)
    set(LIB_PNG_NAME png16_static)
else ()
    set(LIB_PNG_NAME png16)
endif ()

#for STL path and libs
set(CLANG_STL_PATH ${NDK_ROOT}/sources/cxx-stl/llvm-libc++/libs/${TARGET_ABI})
set(CLANG_STL_LIB_NAME c++_static c++abi)

add_definitions ("-DGL_GLEXT_PROTOTYPES")
add_definitions ("-DEGL_EGLEXT_PROTOTYPES")

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../OpenGLESCommon/libpng/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../OpenGLESCommon/utils
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../OpenGLESCommon/common
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../OpenGLESCommon/glrender
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../OpenGLESCommon/glrender/libassimp
        ${CMAKE_CURRENT_SOURCE_DIR}/header_out
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
        ${CMAKE_CURRENT_SOURCE_DIR}/draw
        ${CMAKE_CURRENT_SOURCE_DIR}/source
        ${CMAKE_CURRENT_SOURCE_DIR}/shader
        ${CMAKE_CURRENT_SOURCE_DIR}/shader_helper
        ${CMAKE_CURRENT_SOURCE_DIR}/render3d
        ${CMAKE_CURRENT_SOURCE_DIR}/model
)

link_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../OpenGLESCommon/libpng/lib/${TARGET_ABI}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../OpenGLESCommon/glrender/libassimp/lib/${TARGET_ABI}
        ${CLANG_STL_PATH}
)

set(LIST_SRC_FILES native.cpp)

list(APPEND LIST_SRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/shader_helper/ShaderHelper.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/draw/SampleBase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/draw/draw_utils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/processor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/draw/AHardwareBufferHelper.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/draw/SampleTransform.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/draw/SampleRender3D.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/draw/SampleDrawFBO.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/draw/SampleRender3DMesh.cpp
        )

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections")

add_library(${PROCESSOR_SO_NAME} SHARED ${LIST_SRC_FILES})

set(LIST_TARGET_LINK
        ${LIB_PNG_NAME}
        EGL
        GLESv3
        nativewindow
        log
        assimp
        )

if (IF_USE_STATIC_PNG_LIB EQUAL 1)
    list(APPEND LIST_TARGET_LINK z)
endif()

list(APPEND LIST_TARGET_LINK ${CLANG_STL_LIB_NAME})

target_link_libraries(
        ${PROCESSOR_SO_NAME}
        ${LIST_TARGET_LINK}
)

if (IF_USE_STATIC_PNG_LIB EQUAL 0)
    add_custom_command(
            TARGET ${PROCESSOR_SO_NAME}
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../OpenGLESCommon/libpng/${TARGET_ABI}/lib${LIB_PNG_NAME}.so ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/${TARGET_ABI}/lib${LIB_PNG_NAME}.so
    )
endif ()

add_custom_command(
        TARGET ${PROCESSOR_SO_NAME}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../OpenGLESCommon/glrender/libassimp/lib/${TARGET_ABI}/libassimp.so ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/${TARGET_ABI}/libassimp.so
)
