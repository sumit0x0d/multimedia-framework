#ifndef AUDIO_CODEC_H
#define AUDIO_CODEC_H

#include <stdint.h>

struct audio_codec;

typedef void * AudioCodecBackend;
typedef struct audio_codec AudioCodec;

typedef enum audio_codec_backend_type {
	AUDIO_CODEC_BACKEND_TYPE_PIPEWIRE,
	AUDIO_CODEC_BACKEND_TYPE_PULSEAUDIO,
} AudioCodecBackendType;

typedef struct audio_codec_create_config {
	AudioCodecBackendType backend_type;
	uint32_t sample_rate;
	uint8_t channel_count;
} AudioCodecCreateConfig;

typedef struct audio_codec_interface {
	void (*create)(const AudioCodecCreateConfig *accconfig, AudioCodecBackend backend);
	void (*destroy)(AudioCodecBackend backend);
} AudioCodecInterface;

void audio_codec_create(const AudioCodecCreateConfig *accconfig, AudioCodec *acodec);
void audio_codec_destroy(AudioCodec *acodec);

#endif
