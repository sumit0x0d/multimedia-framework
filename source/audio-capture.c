#include <stdlib.h>
#include <assert.h>

#include <audio-capture.h>

#include "audio-capture-pulseaudio.h"

typedef struct audio_capture_interface _interface_t;
typedef struct audio_capture_create_config _create_config_t;

typedef struct audio_capture {
	audio_capture_backend_t backend;
	_interface_t interface;
} _audio_capture_t;

void
audio_capture_create(const _create_config_t *create_config, _audio_capture_t *audio_capture)
{
	audio_capture = (_audio_capture_t *)malloc(sizeof (_audio_capture_t));
	assert(audio_capture);
	switch (create_config->backend_type) {
	case AUDIO_CAPTURE_BACKEND_TYPE_PULSEAUDIO:
		audio_capture->interface = audio_capture_interface_pulseaudio;
		break;
	case AUDIO_CAPTURE_BACKEND_TYPE_PIPEWIRE:
		// audio_capture->interface = audio_capture_interface_vulkan;
		break;
	}
	audio_capture->interface.create(create_config, audio_capture->backend);
}

void
audio_capture_destroy(_audio_capture_t *audio_capture)
{
	audio_capture->interface.destroy(audio_capture->backend);
}
