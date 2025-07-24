#ifndef VIDEO_CODEC_VAAPI_H
#define VIDEO_CODEC_VAAPI_H

#include <stdint.h>
#include <stdbool.h>

#include <va/va.h>
#include <va/va_drm.h>

struct video_codec_vaapi {
	VADisplay va_display;
};

struct video_codec_vaapi *
video_codec_vaapi_create();
void
video_codec_vaapi_destroy(struct video_codec_vaapi *vaapi);

#endif
