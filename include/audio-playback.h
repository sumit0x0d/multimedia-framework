#ifndef AUDIO_PLAYBACK_H
#define AUDIO_PLAYBACK_H

#include <stdint.h>

struct audio_playback;

typedef void * AudioPlaybackBackend;
typedef struct audio_playback AudioPlayback;

typedef enum audio_playback_backend_type {
	AUDIO_PLAYBACK_BACKEND_TYPE_PIPEWIRE,
	AUDIO_PLAYBACK_BACKEND_TYPE_PULSEAUDIO,
} AudioPlaybackBackendType;

typedef struct audio_playback_create_config {
	AudioPlaybackBackendType backend_type;
	uint32_t format;
	uint32_t sample_rate;
	uint8_t channel_count;
} AudioPlaybackCreateConfig;

typedef struct audio_playback_interface {
	void *(*create)(const AudioPlaybackCreateConfig *apcconfig);
	void (*destroy)(AudioPlaybackBackend backend);
} AudioPlaybackInterface;

AudioPlayback *audio_playback_create(const AudioPlaybackCreateConfig *apcconfig);
void audio_playback_destroy(AudioPlayback *aplayback);

#endif
