#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <turbojpeg.h>

#include "video-encode-turbojpeg.h"

typedef struct video_encode_turbojpeg Turbojpeg;
typedef struct video_encode_create_config CreateConfig;
typedef struct video_encode_interface Interface;
typedef struct video_encode_encoded_buffer EncodedBuffer;

static void *
_turbojpeg_create(const CreateConfig *create_config);
static void
_turbojpeg_destroy(void *backend);
static EncodedBuffer
_turbojpeg_dispatch(void *backend, const void *buffer);

Interface video_encode_interface_turbojpeg = {
	.create = _turbojpeg_create,
	.destroy = _turbojpeg_destroy,
	.dispatch = _turbojpeg_dispatch
};

static void *
_turbojpeg_create(const CreateConfig *create_config)
{
	Turbojpeg *turbojpeg = (Turbojpeg *)malloc(sizeof (Turbojpeg));
	assert(turbojpeg);
	turbojpeg->handle = tjInitCompress();	
	turbojpeg->width = create_config->width;
	turbojpeg->height = create_config->height;
	turbojpeg->pitch = create_config->width * 4;
	// turbojpeg->pixel_format = create_config->pixel_format;
	switch (create_config->chroma_subsample) {
	case VIDEO_ENCODE_CHROMA_SUBSAMPLE_YUV444:
		turbojpeg->chroma_subsample = TJSAMP_444;
		break;
	case VIDEO_ENCODE_CHROMA_SUBSAMPLE_YUV422:
		turbojpeg->chroma_subsample = TJSAMP_422;
		break;
	case VIDEO_ENCODE_CHROMA_SUBSAMPLE_YUV420:
		turbojpeg->chroma_subsample = TJSAMP_420;
		break;
	};
	turbojpeg->quality = create_config->mjpeg.quality;
	return turbojpeg;
}

void
_turbojpeg_destroy(void *backend)
{
	Turbojpeg *turbojpeg = (Turbojpeg *)backend;
	tjDestroy(turbojpeg->handle);
	free(turbojpeg);
}

EncodedBuffer
_turbojpeg_dispatch(void *backend, const void *buffer)
{
	Turbojpeg *turbojpeg = (Turbojpeg *)backend;
	EncodedBuffer encoded_buffer = {0};
	tjCompress2(turbojpeg->handle, buffer, turbojpeg->width, 0, turbojpeg->height, turbojpeg->pixel_format, encoded_buffer.data,
		&encoded_buffer.size, turbojpeg->chroma_subsample, turbojpeg->quality, 0);
	return encoded_buffer;
}
