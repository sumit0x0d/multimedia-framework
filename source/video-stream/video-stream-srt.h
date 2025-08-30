#ifndef VIDEO_STREAM_SRT_H
#define VIDEO_STREAM_SRT_H

#include <stdint.h>

#include <srt/srt.h>

struct video_codec_srt {

};

struct video_codec_srt *
video_codec_srt_create();
void
video_codec_srt_destroy(struct video_codec_srt *srt);

void
video_codec_srt_send();
void
video_codec_srt_receive();

#endif
