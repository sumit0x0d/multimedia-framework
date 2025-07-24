#ifndef VIDEO_PLAYBACK_RENDER_H
#define VIDEO_PLAYBACK_RENDER_H

#include <stdint.h>

#include <video-playback.h>

typedef uint32_t xcb_window_t;

struct video_playback_render_create_config {
	union {
		struct {
			struct wl_display *wl_display;
			struct wl_surface *wl_surface;
		};
		struct {
			struct xcb_connection_t *xcb_connnection;
			xcb_window_t xcb_window;
		};
	};
};

struct video_playback_render_interface {
	void *(*create)(const struct video_playback_create_config *create_config,
		const struct video_playback_render_create_config *render_create_config);
	void (*destroy)(void *backend);
	void (*dispatch)(void *backend, void *buffer);
};

struct video_playback_render {
	void *backend;
	struct video_playback_render_interface interface;
};

struct video_playback_render *
video_playback_render_create(const struct video_playback_create_config *create_config,
	const struct video_playback_render_create_config *render_create_config);

void
video_playback_render_destroy(struct video_playback_render *video_playback);

void
video_playback_render_dispatch(struct video_playback_render *video_playback,
     void *buffer);

#endif
