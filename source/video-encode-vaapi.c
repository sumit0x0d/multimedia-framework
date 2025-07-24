#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>

#include "video-codec-vaapi.h"

typedef struct video_codec_vaapi video_codec_vaapi_t;

void
encode_frame(VADisplay va_display, VASurfaceID input_surface, VABufferID coded_buf) {
	VAConfigID config_id;
	VAContextID context_id;
	VAStatus va_status;

	// Configuration
	VAConfigAttrib config_attrib = {VAConfigAttribRTFormat, VA_RT_FORMAT_YUV420};
	va_status = vaCreateConfig(va_display, VAProfileH264Main, VAEntrypointEncSlice, &config_attrib, 1, &config_id);
	if (va_status != VA_STATUS_SUCCESS) {
		fprintf(stderr, "Error: vaCreateConfig failed\n");
		return;
	}

	// Create a context
	va_status = vaCreateContext(va_display, config_id, 1920, 1080, VA_PROGRESSIVE, &input_surface, 1, &context_id);
	if (va_status != VA_STATUS_SUCCESS) {
		fprintf(stderr, "Error: vaCreateContext failed\n");
		vaDestroyConfig(va_display, config_id);
		return;
	}

	// Begin encoding
	va_status = vaBeginPicture(va_display, context_id, input_surface);
	if (va_status != VA_STATUS_SUCCESS) {
		fprintf(stderr, "Error: vaBeginPicture failed\n");
		vaDestroyContext(va_display, context_id);
		vaDestroyConfig(va_display, config_id);
		return;
	}

	// Render and encode a frame
	va_status = vaRenderPicture(va_display, context_id, &coded_buf, 1);
	if (va_status != VA_STATUS_SUCCESS) {
		fprintf(stderr, "Error: vaRenderPicture failed\n");
		vaEndPicture(va_display, context_id);
		vaDestroyContext(va_display, context_id);
		vaDestroyConfig(va_display, config_id);
		return;
	}

	va_status = vaEndPicture(va_display, context_id);
	if (va_status != VA_STATUS_SUCCESS) {
		fprintf(stderr, "Error: vaEndPicture failed\n");
		vaDestroyContext(va_display, context_id);
		vaDestroyConfig(va_display, config_id);
		return;
	}

	// Destroy resources
	vaDestroyContext(va_display, context_id);
	vaDestroyConfig(va_display, config_id);
}


video_codec_vaapi_t *
video_codec_vaapi_create(unsigned int width, unsigned int height)
{
	video_codec_vaapi_t *vaapi = (video_codec_vaapi_t *)malloc(sizeof (video_codec_vaapi_t));
	assert(va);

	int fd = open("/dev/dri/card0", O_RDONLY);

	if (fd < 0) {
		printf("drm->fd failed\n");
	}

	vaapi->va_display = vaGetDisplayDRM(fd);

	if (vaInitialize(vaapi->va_display, NULL, NULL) != VA_STATUS_SUCCESS) {
		fprintf(stderr, "Error: vaInitialize failed\n");
		vaTerminate(vaapi->va_display);
		exit(EXIT_FAILURE);
	}

	VASurfaceID input_surface;

	if (vaCreateSurfaces(vaapi->va_display, VA_RT_FORMAT_YUV420, width, height, &input_surface, 1, NULL, 0) != VA_STATUS_SUCCESS) {
		fprintf(stderr, "Error: vaCreateSurfaces failed\n");
		vaTerminate(vaapi->va_display);
		exit(EXIT_FAILURE);
	}

	return vaapi;
}

void
video_codec_vaapi_destroy(video_codec_vaapi_t *vaapi)
{
	free(vaapi);
}
