#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "video-playback-render-opengles-egl.h"

typedef struct video_playback_egl Egl;

Egl *
video_playback_egl_create(EGLint interval, EGLNativeDisplayType native_display_type,
    EGLNativeWindowType native_window_type)
{
	Egl *egl = (Egl *)malloc(sizeof (Egl));
	assert(egl);
	EGLint num_config;
	EGLint major, minor;
	EGLint context_attrib_list[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	EGLint config_attrib_list[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};
	EGLint surface_attrib_list[] = {EGL_NONE};
	egl->display = eglGetDisplay(native_display_type);
	eglBindAPI(EGL_OPENGL_ES_API);
	eglInitialize(egl->display, &major, &minor);
	eglGetConfigs(egl->display, NULL, 0, &num_config);
	EGLConfig config;
	eglChooseConfig(egl->display, config_attrib_list, &config, 1, &num_config);
	egl->surface = eglCreateWindowSurface(egl->display, config, native_window_type,
        surface_attrib_list);
	egl->context = eglCreateContext(egl->display, config, EGL_NO_CONTEXT,
        context_attrib_list);
	eglMakeCurrent(egl->display, egl->surface, egl->surface, egl->context);
	eglSwapInterval(egl->display, interval);
	return egl;
}

void
video_playback_egl_destroy(Egl *egl)
{
	eglDestroyContext(egl->display, egl->context);
	eglDestroySurface(egl->display, egl->surface);
	eglTerminate(egl->display);
}
