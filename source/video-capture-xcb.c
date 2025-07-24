#include <stdio.h>
#include <stdlib.h>

#include "video-capture-xcb.h"

typedef struct video_capture_xcb video_capture_xcb_t;

video_capture_xcb_t *
video_capture_xcb_create()
{
	video_capture_xcb_t *xcb = (video_capture_xcb_t *)malloc(sizeof (video_capture_xcb_t));

	xcb->connection = xcb_connect(NULL, NULL);
	xcb->setup = xcb_get_setup(xcb->connection);
	xcb->screen_iterator = xcb_setup_roots_iterator(xcb->setup);
	xcb->screen = xcb->screen_iterator.data;
	xcb->window = xcb_generate_id(xcb->connection);

	xcb_void_cookie_t void_cookie = xcb_create_window(xcb->connection, XCB_COPY_FROM_PARENT, xcb->window,
		xcb->screen->root, 0, 0, 150, 150, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT, xcb->screen->root_visual, 0, NULL );

	void_cookie = xcb_map_window(xcb->connection, xcb->window);

	xcb_generic_error_t *err = NULL, *err2 = NULL;
	xcb_screen_iterator_t screen_iterator = xcb_setup_roots_iterator(xcb->setup);
	xcb_screen_t *screen = screen_iterator.data;
	int win_h, win_w, win_d;
	xcb_get_geometry_cookie_t gg_cookie = xcb_get_geometry(xcb->connection, xcb->window);
	xcb_get_geometry_reply_t *gg_reply = xcb_get_geometry_reply(xcb->connection, gg_cookie, &err);

	if (gg_reply) {
		win_w = gg_reply->width;
		win_h = gg_reply->height;
		win_d = gg_reply->depth;
		free(gg_reply);
	} else {
		if (err) {
			printf("get geometry: XCB error %d\n", err->error_code);
			free(err);
		}
		printf("exiting");
		exit(EXIT_FAILURE);
	}

	// create a pixmap
	xcb_pixmap_t win_pixmap = xcb_generate_id(xcb->connection);
	// xcb_composite_name_window_pixmap(xcb->xcb_connection, xcb->xcb_window, win_pixmap);
	xcb_get_image_cookie_t gi_cookie = xcb_get_image(xcb->connection, XCB_IMAGE_FORMAT_Z_PIXMAP, win_pixmap, 0, 0, win_w, win_h,
		(uint32_t)(~0UL));
	xcb_get_image_reply_t *gi_reply = xcb_get_image_reply(xcb->connection, gi_cookie, &err);

	if (gi_reply) {
		int data_len = xcb_get_image_data_length(gi_reply);
		printf("data_len = %d\n", data_len);
		printf("visual = %u\n", gi_reply->visual);
		printf("depth = %u\n", gi_reply->depth);
		printf("size = %dx%d\n", win_w, win_h);
		uint8_t *data = xcb_get_image_data(gi_reply);
		fwrite(data, data_len, 1, stdout);
		free(gi_reply);
	}
	// return EXIT_SUCCESS;

	xcb_flush(xcb->connection);
	xcb_disconnect(xcb->connection);

	// while(1) {};

	return xcb;
}
