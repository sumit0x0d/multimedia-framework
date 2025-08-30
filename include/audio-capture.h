#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H

#include <stdint.h>

typedef struct audio_capture AudioCapture;
typedef void * AudioCaptureBackend;
typedef enum audio_capture_backend_type AudioCaptureBackendType;
typedef struct audio_capture_create_info AudioCaptureCreateInfo;
typedef struct audio_capture_interface AudioCaptureInterface;

enum audio_capture_backend_type {
	AUDIO_CAPTURE_BACKEND_TYPE_PIPEWIRE,
	AUDIO_CAPTURE_BACKEND_TYPE_PULSEAUDIO,
};

struct audio_capture_create_info {
	AudioCaptureBackendType backend_type;
	uint8_t channel_count;
	uint32_t sample_rate;
};

struct audio_capture_interface {
	AudioCaptureBackend (*create)(const AudioCaptureCreateInfo *accinfo);
	void (*destroy)(AudioCaptureBackend acbackend);
};

AudioCapture *AudioCapture_Create(const AudioCaptureCreateInfo *accInfo);
void AudioCapture_Destroy(AudioCapture *aCapture);

#endif
