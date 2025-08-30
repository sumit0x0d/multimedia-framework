#ifndef VIDEO_PLAYBACK_EGL_H
#define VIDEO_PLAYBACK_EGL_H

#include <EGL/egl.h>

struct video_playback_egl {
	EGLDisplay display;
	EGLContext context;
	EGLSurface surface;
};

struct video_playback_egl *
video_playback_egl_create(EGLint interval, EGLNativeDisplayType native_display_type,
    EGLNativeWindowType native_window_type);

void
video_playback_egl_destroy(struct video_playback_egl *egl);

#endif
