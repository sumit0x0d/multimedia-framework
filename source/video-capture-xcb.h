#ifndef VIDEO_CAPTURE_XCB_H
#define VIDEO_CAPTURE_XCB_H

#include <stdint.h>

#include <xcb/xcb.h>

struct video_capture_xcb {
	xcb_connection_t *connection;
	const xcb_setup_t *setup;
	xcb_screen_iterator_t screen_iterator;
	xcb_screen_t *screen;
	xcb_atom_t *atom;
	xcb_intern_atom_cookie_t *intern_atom_cookie;
	xcb_window_t window;
};

struct video_capture_xcb *
video_capture_xcb_create();
void
video_capture_xcb_destroy(struct video_capture_xcb *xcb);

void
video_capture_xcb_render(struct video_capture_xcb *xcb, uint32_t width, uint32_t height);

#endif
