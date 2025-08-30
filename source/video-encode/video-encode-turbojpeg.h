#ifndef VIDEO_ENCODE_TURBOJPEG_H
#define VIDEO_ENCODE_TURBOJPEG_H

#include <stdbool.h>

#include <turbojpeg.h>

#include <video-encode.h>

struct video_encode_turbojpeg {
	int width;
	int height;
	int pitch;
	enum TJPF pixel_format;
	enum TJSAMP chroma_subsample;
	int quality;
	tjhandle handle;
	void *encoded_buffer;
	size_t *encoded_buffer_size;
};

extern struct video_encode_interface video_encode_interface_turbojpeg;

#endif
