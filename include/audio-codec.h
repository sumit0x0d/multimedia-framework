#ifndef AUDIO_CODEC_H
#define AUDIO_CODEC_H

#include <stdint.h>

typedef struct audio_codec AudioCodec;
typedef void * AudioCodecBackend;
typedef enum audio_codec_backend_type AudioCodecBackendType;
typedef struct audio_codec_create_info AudioCodecCreateInfo;
typedef struct audio_codec_interface AudioCodecInterface;

enum audio_codec_backend_type {
	AUDIO_CODEC_BACKEND_TYPE_PIPEWIRE,
	AUDIO_CODEC_BACKEND_TYPE_PULSEAUDIO,
};

struct audio_codec_create_info {
	AudioCodecBackendType backend_type;
	uint32_t sample_rate;
	uint8_t channel_count;
};

struct audio_codec_interface {
	void (*create)(const AudioCodecCreateInfo *accInfo, AudioCodecBackend backend);
	void (*destroy)(AudioCodecBackend backend);
};

AudioCodec *AudioCodec_Create(const AudioCodecCreateInfo *accInfo);
void AudioCodec_Destroy(AudioCodec *aCodec);

#endif
