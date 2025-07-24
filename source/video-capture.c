#include <stdlib.h>
#include <assert.h>

#include <video-capture.h>

// #include "video-capture-drm.h"

typedef struct video_capture VideoCapture;
typedef struct video_capture_create_config CreateConfig;

VideoCapture *
video_capture_create(const CreateConfig *create_config)
{
	VideoCapture *video_capture = (VideoCapture *)malloc(sizeof (VideoCapture));
	assert(video_capture);

	switch (create_config->backend_type) {
	case VIDEO_CAPTURE_BACKEND_TYPE_PIPEWIRE:
		// video_capture->interface = video_capture_interface_pipewire;
		break;
	case VIDEO_CAPTURE_BACKEND_TYPE_DRM:
		// video_capture->interface = video_capture_interface_drm;
		break;
	case VIDEO_CAPTURE_BACKEND_TYPE_XCB:
		break;
	}

	video_capture->backend = video_capture->interface.create(create_config);

	return video_capture;
}

void
video_capture_destroy(VideoCapture *video_capture)
{
	video_capture->interface.destroy(video_capture->backend);
}
