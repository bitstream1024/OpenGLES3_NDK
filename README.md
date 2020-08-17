# OpenGLES_NDK
## 介绍

本仓库是学习OpenGLES的一个练习代码存放仓库，主要在C层完成绘制，目前包括以下两个工程：

* OpenGLESSample
* OpenGLESEGLSample

其中OpenGLESSample工程使用GLSurfaceView创建GL环境，并在GL线程中处理工作；OpenGLESEGLSample则为自己创建EGL环境，并结合MediaCode和MediaMuxer来实现录屏功能。

## OpenGLESSample

OpenGLESSample主要完成以下功能：

* 绘制三角形
* 绘制纹理
* FBO Render
* AHardwareBuffer封装

## OpenGLESEGLSample

该工程主要功能：

使用TextureView的TextureSurface和MediaCode对象创建的Surface来创建两个EGLSurface，一个用于Render渲染显示，一个用于MediaCodec对象接收帧数据，并使用MediaMuxer编码为mp4结果。



## 参考阅读

[google/grafika](https://github.com/google/grafika)