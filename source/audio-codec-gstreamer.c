#include <audio-codec-gstreamer.h>

struct audio_codec_gstreamer *
audio_codec_gstreamer_create(bool encoder, const char *codec_name, int width, int height, uint32_t frame_rate)
{
	// Initialize GStreamer
	gst_init(NULL, NULL);

	// Create a GStreamer pipeline
	GstElement *pipeline, *filesrc, *jpegdecode, *videosink;
	GstBus *bus;
	GstMessage *msg;

	// Create elements for file source, JPEG decoder, and video sink
	pipeline = gst_pipeline_new("jpeg-decode-pipeline");
	filesrc = gst_element_factory_make("filesrc", "file-source");
	jpegdecode = gst_element_factory_make("jpegdec", "jpeg-decoder");
	// vaapijpegdec = gst_element_factory_make("vaapijpegdec", "vaapi-jpeg-decoder");
	videosink = gst_element_factory_make("autovideosink", "video-sink");

	if (!pipeline || !filesrc || !jpegdecode || !videosink) {
		g_print("One or more elements could not be created. Exiting.\n");
		return NULL;
	}

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
		return NULL;
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

void
audio_codec_gstreamer_destroy(struct audio_codec_gstreamer *gstreamer)
{}

void
audio_codec_gstreamer_encode(struct audio_codec_gstreamer *gstreamer, uint8_t *buffer)
{}

void
audio_codec_gstreamer_decode(struct audio_codec_gstreamer *gstreamer, uint8_t *encoded_buffer, uint32_t encoded_buffer_size)
{}
