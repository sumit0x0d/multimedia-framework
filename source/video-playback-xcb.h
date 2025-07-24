#ifndef VIDEO_PLAYBACK_XCB_H
#define VIDEO_PLAYBACK_XCB_H

#include <stdint.h>

#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/render.h>

#include <video-playback.h>

struct video_playback_xcb {
	xcb_connection_t *xcb_connection;
	const xcb_setup_t *xcb_setup;
	xcb_screen_iterator_t xcb_screen_iterator;
	xcb_screen_t *xcb_screen;
	xcb_atom_t *xcb_atom;
	xcb_intern_atom_cookie_t *xcb_intern_atom_cookie;
	xcb_window_t xcb_window;
};

extern struct video_playback_interface video_playback_interface_xcb;

#endif
