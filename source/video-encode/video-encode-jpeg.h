#ifndef VIDEO_CODEC_JPEG_H
#define VIDEO_CODEC_JPEG_H

#include <stdbool.h>

#include <stdio.h>
#include <jpeglib.h>

#include <video-encode.h>

struct video_encode_jpeg {
	int width;
	int height;
	int pitch;
	int buffer_size;
	unsigned char *jpeg_buffer;
	unsigned long jpeg_size;
	int qualilty;
};

extern struct video_codec_interface video_codec_interface_jpeg;

#endif
