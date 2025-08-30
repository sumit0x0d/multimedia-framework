#ifndef AUDIO_CODEC_GSTREAMER_H
#define AUDIO_CODEC_GSTREAMER_H

#include <stdint.h>
#include <stdbool.h>

#include <gst/gst.h>

struct audio_codec_gstreamer {
};

static const char *AUDIO_CODEC_GSTREAMER_AAC = "aac";

struct audio_codec_gstreamer *
audio_codec_gstreamer_create(bool encoder, const char *codec_name, int width, int height, uint32_t frame_rate);
void
audio_codec_gstreamer_destroy(struct audio_codec_gstreamer *gstreamer);

void
audio_codec_gstreamer_encode(struct audio_codec_gstreamer *gstreamer, uint8_t *buffer);
void
audio_codec_gstreamer_decode(struct audio_codec_gstreamer *gstreamer, uint8_t *encoded_buffer, uint32_t encoded_buffer_size);

#endif
