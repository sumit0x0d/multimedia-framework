#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include <unistd.h>

#include "audio-codec-wavpack.h"

typedef struct audio_codec_wavpack _wavpack_t;
typedef struct audio_codec_create_config _create_config;

void
_wavpack_create(const _create_config *create_config, audio_codec_backend_t backend);
void
_wavpack_destroy(audio_codec_backend_t backend);

struct audio_codec_interface audio_codec_interface = {
	.create = _wavpack_create,
	.destroy = _wavpack_destroy
};

void
_wavpack_create(const _create_config *create_config, audio_codec_backend_t backend)
{
	_wavpack_t *wavpack = (_wavpack_t *)backend;
	wavpack = (_wavpack_t *)malloc(sizeof (_wavpack_t));
}

void
_wavpack_destroy(audio_codec_backend_t backend)
{
	_wavpack_t *wavpack = (_wavpack_t *)backend;
	free(wavpack);
}

void
_wavpack_encode(struct audio_codec_wavpack *wavpack, uint8_t *buffer)
{
}

void
_wavpack_decode(struct audio_codec_wavpack *wavpack, uint8_t *encoded_buffer, uint32_t encoded_buffer_size)
{
}
