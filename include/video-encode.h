#ifndef VIDEO_ENCODE_H
#define VIDEO_ENCODE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum video_encode_backend_type {
	VIDEO_ENCODE_BACKEND_TYPE_JPEG,
	VIDEO_ENCODE_BACKEND_TYPE_TURBOJPEG,
	VIDEO_ENCODE_BACKEND_TYPE_X264,
	VIDEO_ENCODE_BACKEND_TYPE_ONEVPL,
	VIDEO_ENCODE_BACKEND_TYPE_VAAPI,
	VIDEO_ENCODE_BACKEND_TYPE_VULKAN,
	VIDEO_ENCODE_BACKEND_TYPE_FFMPEG,
	VIDEO_ENCODE_BACKEND_TYPE_GSTREAMER
};

enum video_encode_coding_format {
	VIDEO_ENCODE_CODING_FORMAT_MJPEG,
	VIDEO_ENCODE_CODING_FORMAT_AVC,
};

enum video_encode_chroma_subsample {
	VIDEO_ENCODE_CHROMA_SUBSAMPLE_YUV444,
	VIDEO_ENCODE_CHROMA_SUBSAMPLE_YUV422,
	VIDEO_ENCODE_CHROMA_SUBSAMPLE_YUV420,
};

struct video_encode_create_config {
	enum video_encode_backend_type backend_type;
	enum video_encode_coding_format coding_format;
	enum video_encode_chroma_subsample chroma_subsample;
	uint16_t width;
	uint16_t height;
	uint16_t frame_rate;
	union {
		struct {
			int quality;
		} mjpeg;
		struct {
			int bitrate;
		} avc;
	};
};

struct video_encode_encoded_buffer {
	void *data;
	size_t size;
};

struct video_encode_interface {
	void *(*create)(const struct video_encode_create_config *create_config);
	void (*destroy)(void *backend);
	struct video_encode_encoded_buffer (*dispatch)(void *backend, const void *buffer);
};

struct video_encode *video_encode_create(const struct video_encode_create_config *create_config);
void video_encode_destroy(struct video_encode *video_encode);
struct video_encode_encoded_buffer video_encode_dispatch(struct video_encode *video_encode, const void *buffer);

#endif
