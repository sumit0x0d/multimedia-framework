#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H

#include <stdint.h>

struct audio_capture;

typedef void * AudioCaptureBackend;
typedef struct audio_capture AudioCapture;

typedef enum audio_capture_backend_type {
	AUDIO_CAPTURE_BACKEND_TYPE_PIPEWIRE,
	AUDIO_CAPTURE_BACKEND_TYPE_PULSEAUDIO,
} AudioCaptureBackendType;

typedef struct audio_capture_create_info {
	AudioCaptureBackendType backend_type;
	uint8_t channel_count;
	uint32_t sample_rate;
} AudioCaptureCreateInfo;

typedef struct audio_capture_interface {
	AudioCaptureBackend (*create)(const AudioCaptureCreateInfo *accinfo);
	void (*destroy)(AudioCaptureBackend acbackend);
} AudioCaptureInterface;

AudioCapture *audio_capture_create(const AudioCaptureCreateInfo *accinfo);
void audio_capture_destroy(AudioCapture *acapture);

#endif
