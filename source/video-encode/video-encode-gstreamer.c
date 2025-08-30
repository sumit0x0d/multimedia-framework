#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "video-encode-gstreamer.h"

typedef struct video_encode_gstreamer _gstreamer_t;
typedef struct video_encode_create_config _create_config_t;
typedef struct video_encode_interface _interface_t;

static void *
_gstreamer_create(const _create_config_t *create_config);
static void
_gstreamer_destroy(void *backend);
static void
_gstreamer_encode(void *backend, const void *buffer, void *encoded_buffer, size_t *encoded_buffer_size);
static void
_gstreamer_decode(void *backend, const void *encoded_buffer, size_t encoded_buffer_size, void *buffer);

struct video_encode_interface video_encode_interface_gstreamer = {
	.create = _gstreamer_create,
	.destroy = _gstreamer_destroy,
	.dispatch = _gstreamer_encode
};

static void *
_gstreamer_create(const _create_config_t *create_config)
{
	const gchar *nano_str;
	guint major, minor, micro, nano;

	gst_init (NULL, NULL);

	gst_version (&major, &minor, &micro, &nano);

	if (nano == 1)
		nano_str = "(CVS)";
	else if (nano == 2)
		nano_str = "(Prerelease)";
	else
		nano_str = "";

	printf ("This program is linked against GStreamer %d.%d.%d %s\n",
		major, minor, micro, nano_str);
#if 0	
	_gstreamer_t *gstreamer = (_gstreamer_t *)backend;

	gstreamer = (_gstreamer_t *)malloc(sizeof (_gstreamer_t));
	assert(gstreamer);

	gstreamer->pipeline = gst_pipeline_new("video-encode");
	assert(gstreamer->pipeline);

	gstreamer->filesrc = gst_element_factory_make("filesrc", "file-source");

	if (!gstreamer->filesrc) {
		g_print("One or more elements could not be created. Exiting.\n");
	}

	gstreamer->jpegdecode = gst_element_factory_make("jpegdec", "jpeg-decoder");
	if (!gstreamer->jpegdecode) {
		g_print("One or more elements could not be created. Exiting.\n");
	}

	gstreamer->videosink = gst_element_factory_make("autovideosink", "video-sink");
	// vaapijpegdec = gst_element_factory_make("vaapijpegdec", "vaapi-jpeg-decoder");

	if (!gstreamer->videosink) {
		g_print("One or more elements could not be created. Exiting.\n");
	}
#endif
}

static void
_gstreamer_destroy(void *backend)
{
	_gstreamer_t *gstreamer = (_gstreamer_t *)backend;

	free(gstreamer);
}

static void
_gstreamer_encode(void *backend, const void *buffer, void *encoded_buffer, size_t *encoded_buffer_size)
{
}

static void
_gstreamer_decode(void *backend, const void *encoded_buffer, size_t encoded_buffer_size, void *buffer)
{
}

#if 0
int main(int argc, char *argv[])
{
	// Initialize GStreamer
	gst_init(&argc, &argv);

	// Create a GStreamer pipeline
	// GstElement *pipeline, *filesrc, *jpegdecode, *videosink;

	GstBus *bus;
	GstMessage *msg;

	// Create elements for file source, JPEG decoder, and video sink



	// Set the input file or URL
	g_object_set(G_OBJECT(filesrc), "location", "your_image.jpg", NULL);

	// Add elements to the pipeline
	gst_bin_add(GST_BIN(pipeline), filesrc);
	gst_bin_add(GST_BIN(pipeline), jpegdecode);
	// gst_bin_add(GST_BIN(pipeline), vaapijpegdec);
	gst_bin_add(GST_BIN(pipeline), videosink);

	// Link the elements together
	if (!gst_element_link(filesrc, jpegdecode) || !gst_element_link(jpegdecode, videosink)) {
		g_print("Elements could not be linked. Exiting.\n");
		return -1;
	}

	// Set the pipeline to the "playing" state
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	// Create a bus to listen for messages from the pipeline
	bus = gst_element_get_bus(pipeline);
	msg = gst_bus_poll(bus, GST_MESSAGE_EOS | GST_MESSAGE_ERROR, -1);

	// Process messages
	if (msg != NULL) {
		GError *err;
		switch (GST_MESSAGE_TYPE(msg)) {
		case GST_MESSAGE_ERROR:
			gst_message_parse_error(msg, &err, NULL);
			g_print("Error: %s\n", err->message);
			g_error_free(err);
			break;
		case GST_MESSAGE_EOS:
			g_print("End of stream reached.\n");
			break;
		default:
			break;
		}
		gst_message_unref(msg);
	}

	// Clean up
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(bus);
	gst_object_unref(pipeline);

	return 0;
}
#endif
