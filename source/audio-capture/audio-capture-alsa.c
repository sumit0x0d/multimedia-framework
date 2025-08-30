#include "audio-capture-alsa.h"

typedef struct audio_capture_alsa Alsa;
typedef struct audio_capture_interface Interface;
typedef struct audio_capture_create_config CreateConfig;

static void _alsa_create(const CreateConfig *create_config, audio_capture_backend_t backend);
static void _alsa_destroy(audio_capture_backend_t backend);

struct audio_capture_interface audio_capture_interface_alsa = {
	.create = _alsa_create,
	.destroy = _alsa_destroy
};

static void _alsa_create(const CreateConfig *create_config, audio_capture_backend_t backend)
{
	Alsa * alsa = (Alsa *)backend;
	alsa = (Alsa *)malloc(sizeof (Alsa));
	assert(audio_capture_alsa);
}

static void _alsa_destroy(audio_capture_backend_t backend)
{
	Alsa * alsa = (Alsa *)backend;
	free(alsa);
}
