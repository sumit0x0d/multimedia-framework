#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "video-playback-xcb.h"

typedef struct video_playback_xcb Xcb;
typedef struct video_playback_create_config CreateConfig;

static void *
_xcb_create(const CreateConfig *create_config);

static void
_xcb_destroy(void *backend);

struct video_playback_interface video_playback_interface_xcb = {
	.create = _xcb_create,
	.destroy = _xcb_destroy
};

static void *
_xcb_create(const CreateConfig *create_config)
{
	Xcb *xcb = (Xcb *)malloc(sizeof (Xcb));
	assert(xcb);

	xcb->xcb_connection = xcb_connect(NULL, NULL);
	xcb->xcb_setup = xcb_get_setup(xcb->xcb_connection);

	xcb->xcb_screen_iterator = xcb_setup_roots_iterator(xcb->xcb_setup);
	xcb->xcb_screen = xcb->xcb_screen_iterator.data;
	xcb->xcb_window = xcb_generate_id(xcb->xcb_connection);

	xcb_void_cookie_t xcb_void_cookie = xcb_create_window(xcb->xcb_connection, XCB_COPY_FROM_PARENT, xcb->xcb_window,
		xcb->xcb_screen->root, 0, 0, create_config->width, create_config->height, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT,
		xcb->xcb_screen->root_visual, 0, NULL );

	char *title = "video-playback";

	xcb_change_property(xcb->xcb_connection, XCB_PROP_MODE_REPLACE, xcb->xcb_window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(title),
		title);

	xcb_map_window(xcb->xcb_connection, xcb->xcb_window);
	xcb_flush(xcb->xcb_connection);

	xcb_gcontext_t foreground = xcb_generate_id(xcb->xcb_connection);
	xcb_point_t points[5];

	// while(1);
	// xcb_disconnect(xcb->xcb_connection);
	return xcb;
}

static void
_xcb_destroy(void *backend)
{
	Xcb *xcb = (Xcb *)backend;
	free(xcb);
}

void
_xcb_display(Xcb *xcb)
{
	xcb_generic_event_t *e;
	while ((e = xcb_wait_for_event(xcb->xcb_connection))) {
		switch (e->response_type & ~0x80) {
		case XCB_EXPOSE:
			/* We draw the points */
			// xcb_poly_point (xcb->xcb_connection, XCB_COORD_MODE_ORIGIN, xcb->xcb_window, foreground, 4, points);

			/* We draw the polygonal line */
			// xcb_poly_line (xcb->xcb_connection, XCB_COORD_MODE_PREVIOUS, xcb->xcb_window, foreground, 4, polyline);

			/* We draw the segements */
			// xcb_poly_segment (xcb->xcb_connection, xcb->xcb_window, foreground, 2, segments);

			/* We draw the rectangles */
			// xcb_poly_rectangle (xcb->xcb_connection, xcb->xcb_window, foreground, 2, rectangles);

			/* We draw the arcs */
			// xcb_poly_arc (xcb->xcb_connection, xcb->xcb_window, foreground, 2, arcs);

			/* We flush the request */
			xcb_flush (xcb->xcb_connection);

			break;
		default:
			/* Unknown event type, ignore it */
			break;
		}
		/* Free the Generic Event */
		free(e);
	}
}
