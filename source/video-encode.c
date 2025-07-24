#include <assert.h>
#include <stdlib.h>

#include "video-encode.h"

#include "video-encode-onevpl.h"
// #include "video-encode-gstreamer.h"

typedef struct video_encode_create_config CreateConfig;
typedef struct video_encode_encoded_buffer EncodedBuffer;

typedef struct video_encode {
	void *backend;
	struct video_encode_interface interface;
} VideoEncode;

VideoEncode *
video_encode_create(const CreateConfig *create_config)
{
	VideoEncode *video_encode = (VideoEncode *)malloc(sizeof (VideoEncode));
	assert(*video_encode);

	switch(create_config->backend_type) {
	case VIDEO_ENCODE_BACKEND_TYPE_JPEG:
		// video_encode->interface = video_encode_interface_jpeg;
		break;
	case VIDEO_ENCODE_BACKEND_TYPE_TURBOJPEG:
		// video_encode->interface = video_encode_interface_turbojpeg;
		break;
	case VIDEO_ENCODE_BACKEND_TYPE_X264:
		// video_encode->interface = video_encode_interface_x256;
		break;
	case VIDEO_ENCODE_BACKEND_TYPE_ONEVPL:
		video_encode->interface = video_encode_interface_onevpl;
		break;
	case VIDEO_ENCODE_BACKEND_TYPE_VAAPI:
		// video_encode->interface = video_encode_interface_vaapi;
		break;
	case VIDEO_ENCODE_BACKEND_TYPE_VULKAN:
		// video_encode->interface = video_encode_interface_vulkan;
		break;
	case VIDEO_ENCODE_BACKEND_TYPE_FFMPEG:
		// video_encode->interface = video_encode_interface_ffmpeg;
		break;
	case VIDEO_ENCODE_BACKEND_TYPE_GSTREAMER:
		// video_encode->interface = video_encode_interface_gstreamer;
		break;
	}

	video_encode->backend = video_encode->interface.create(create_config);

	return video_encode;
}

void
video_encode_destroy(VideoEncode *video_encode)
{
	video_encode->interface.destroy(video_encode->backend);
	free(video_encode);
}

EncodedBuffer
video_encode_dispatch(VideoEncode *video_encode, const void *buffer)
{
	return video_encode->interface.dispatch(video_encode->backend, buffer);
}
