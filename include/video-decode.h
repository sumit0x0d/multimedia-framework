#ifndef VIDEO_DECODE_H
#define VIDEO_DECODE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct video_decode VideoDecode;
typedef struct video_decode_create_info VideoDecodeCreateInfo;
typedef enum video_decode_backend_type VideoDecodeBackendType;

enum video_decode_backend_type {
     VIDEO_DECODE_BACKEND_TYPE_JPEG,
     VIDEO_DECODE_BACKEND_TYPE_TURBOJPEG,
     VIDEO_DECODE_BACKEND_TYPE_X264,
     VIDEO_DECODE_BACKEND_TYPE_ONEVPL,
     VIDEO_DECODE_BACKEND_TYPE_VAAPI,
     VIDEO_DECODE_BACKEND_TYPE_VULKAN,
     VIDEO_DECODE_BACKEND_TYPE_FFMPEG,
     VIDEO_DECODE_BACKEND_TYPE_GSTREAMER
};

enum video_decode_coding_format {
     VIDEO_DECODE_CODING_FORMAT_MJPEG,
     VIDEO_DECODE_CODING_FORMAT_AVC,
};

enum video_decode_chroma_subsample {
     VIDEO_DECODE_CHROMA_SUBSAMPLE_YUV444,
     VIDEO_DECODE_CHROMA_SUBSAMPLE_YUV422,
     VIDEO_DECODE_CHROMA_SUBSAMPLE_YUV420,
};

struct video_decode_create_info {
     VideoDecodeBackendType backend_type;
     enum video_decode_coding_format coding_format;
     enum video_decode_chroma_subsample chroma_subsample;
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

struct video_decode_interface {
     void *(*create)(const VideoDecodeCreateInfo *vdcinfo);
     void (*destroy)(void *backend);
     void *(*dispatch)(void *backend, const void *buffer);
};

struct video_decode {
     void *backend;
     struct video_decode_interface interface;
};

VideoDecode *video_decode_create(const VideoDecodeCreateInfo *vdcinfo);
void video_decode_destroy(VideoDecode *vdecode);
void *video_decode_dispatch(VideoDecode *vdecode, const void *buffer);

#endif
