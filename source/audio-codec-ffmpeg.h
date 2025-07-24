#ifndef AUDIO_CODEC_FFMPEG_H
#define AUDIO_CODEC_FFMPEG_H

#include <stdbool.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/hwcontext.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

struct audio_codec_ffmpeg {
	AVCodecContext *av_codec_context;
	AVFrame *av_frame;
	AVFrame *av_hwframe;
	AVPacket *av_packet;
	void (*send_av_frame)(AVCodecContext *, AVFrame *, AVFrame *, uint8_t *);
};

static const char *AUDIO_CODEC_FFMPEG_AAC = "aac";

struct audio_codec_ffmpeg *
audio_codec_ffmpeg_create(bool encoder, const char *codec_name, int width, int height, uint32_t frame_rate);
void
audio_codec_ffmpeg_destroy(struct audio_codec_ffmpeg *ffmpeg);

void
audio_codec_ffmpeg_encode(struct audio_codec_ffmpeg *ffmpeg, uint8_t *buffer);
void
audio_codec_ffmpeg_decode(struct audio_codec_ffmpeg *ffmpeg, uint8_t *encoded_buffer, uint32_t encoded_buffer_size);

#endif
