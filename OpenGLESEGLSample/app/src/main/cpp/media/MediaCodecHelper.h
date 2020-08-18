//
// Created by ChauncyWang on 2020/8/17.
//

#pragma once
#include <string>
#include <media/NdkMediaCodec.h>
#include <media/NdkMediaMuxer.h>
#include <EGL/egl.h>

class MediaCodecHelper
{
public:
	static MediaCodecHelper* CreateInstance ();
	~MediaCodecHelper();
	int PrepareEncoderWithCtx (int width, int height, int fps, const std::string& path, EGLContext const *eglContext);
	void DrainEncoder (bool eof);
	void ReleaseEncoder ();
	ANativeWindow *GetInputWindow ();

private:
	MediaCodecHelper ();
	int PrepareEncoder (int width, int height, int fps, const std::string& path);

	static MediaCodecHelper* m_pNativeMediaCodecHelper;
	AMediaCodec* m_pMediaCodec;
	AMediaMuxer* m_pMediaMuxer;
	ANativeWindow *m_pNativeWindow;
	int IFRAME_INTERVAL;          // 10 seconds between I-frames
	int m_VideoWidth;
	int m_VideoHeight;
	int m_FPS;
	int m_BitRate;
	bool m_bMuxerStarted;
	int mTrackIndex;
};



