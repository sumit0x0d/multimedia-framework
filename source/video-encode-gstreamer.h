#ifndef VIDEO_ENCODE_GSTREAMER_H
#define VIDEO_ENCODE_GSTREAMER_H

#include <gst/gst.h>

#include <video-encode.h>

struct video_encode_gstreamer {
	GstElement *pipeline;
	GstElement *filesrc;
	GstElement *jpegdecode;
	GstElement *videosink;
};

extern struct video_encode_interface video_encode_interface_gstreamer;

#endif
