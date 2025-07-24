#ifndef AUDIO_CODEC_WAVPACK_H
#define AUDIO_CODEC_WAVPACK_H

#include <stdint.h>
#include <stdbool.h>

#include <audio-codec.h>

#include <wavpack/wavpack.h>

struct audio_codec_wavpack {

};

static const char *AUDIO_CODEC_FFMPEG_AAC = "aac";

extern struct audio_codec_interface audio_codec_interface;

#endif
