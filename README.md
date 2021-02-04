# OpenGLES_NDK

## 概述

本仓库是一个存放学习 OpenGL ES 过程中编写的练习代码的仓库，绘制部分主要在 Native 层，使用 OpenGL ES 3.0，目前包括以下两个工程：

* OpenGLESSample
* OpenGLESEGLSample

其中，OpenGLESSample 工程使用 `GLSurfaceView` 创建 GL 环境，并在 GL 线程中处理绘制工作；OpenGLESEGLSample 则为自己创建 EGL 环境，并结合 `MediaCodec` 和 `MediaMuxer` 来实现录屏功能。特别的是，目前视频编码皆是在 Native 层完成，后续计划在这个[仓库](https://github.com/cgwang1580/GLMediaRecorder)编写一个用于录制 OpenGL ES 渲染图像的库。

## OpenGLESSample

OpenGLESSample主要完成以下功能：

* 三角形渲染
* 纹理渲染
* FBO 使用
* AHardwareBuffer 封装使用
* YUV 图像渲染
* 单通道图像渲染
* Obj 数据渲染

## OpenGLESEGLSample

该工程主要功能：

使用 `TextureView` 的 `TextureSurface` 和 `MediaCode` 对象创建的 Surface 来创建两个`EGLSurface`，一个用于渲染显示，一个用于 `MediaCodec` 对象接收帧数据，并使用 MediaMux`er 编码为 mp4 文件。这部分代码已经移置这个[仓库](https://github.com/cgwang1580/GLMediaRecorder)，后续录制库也会在这个仓库来实现。

目前这个项目有两个分支：

* master
* dev_native_codec

dev_native_codec 这个分支目前会有一些 Native 层音频获取的代码编写，等到这部分有阶段性成果之后会移到上述仓库中。

## 参考阅读

[google/grafika](https://github.com/google/grafika)