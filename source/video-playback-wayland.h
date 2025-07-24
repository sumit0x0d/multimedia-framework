#ifndef VIDEO_PLAYBACK_WAYLAND_H
#define VIDEO_PLAYBACK_WAYLAND_H

#include <stdint.h>

#include <wayland-client.h>
#include <wayland-cursor.h>
#include <wayland-egl.h>

#include <video-playback.h>

#include "wayland-protocols/presentation-time-client-protocol.h"
#include "wayland-protocols/viewporter-client-protocol.h"
#include "wayland-protocols/xdg-shell-client-protocol.h"

struct video_playback_wayland {
	struct wl_display *wl_display;
	struct wl_compositor *wl_compositor;
	struct wl_shm *wl_shm;
	struct wl_seat *wl_seat;
	struct wl_surface *wl_surface;
	struct wl_egl_window *wl_egl_window;
	struct wl_cursor_theme *wl_cursor_theme;
	struct wl_surface *wl_surface_cursor;
	struct xdg_wm_base *xdg_wm_base;
	struct wp_presentation *wp_presentation;
	struct wp_viewporter *wp_viewporter;
};

extern struct video_playback_interface video_playback_interface_wayland;

#endif
