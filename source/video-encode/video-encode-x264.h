#ifndef VIDEO_CODEC_X264_H
#define VIDEO_CODEC_X264_H

#include <stdint.h>
#include <stdbool.h>

#include <x264.h>

struct video_encode_x264 {
	x264_param_t x264_param;
};

struct video_encode_x264 *
video_encode_x264_create();

void
video_encode_x264_destroy(struct video_encode_x264 *x264);

#endif
