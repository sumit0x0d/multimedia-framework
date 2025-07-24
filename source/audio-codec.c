#include <assert.h>
#include <stdlib.h>

#include "audio-codec.h"

#include "audio-codec-ffmpeg.h"

typedef struct audio_codec audio_codec_t;

audio_codec_t *
audio_codec_create(unsigned int width, unsigned int height, unsigned int frame_rate)
{
	audio_codec_t *audio_codec = (audio_codec_t *)malloc(sizeof (audio_codec_t));
	assert(audio_codec);

	audio_codec->ffmpeg = audio_codec_ffmpeg_create(true, VIDEO_CODEC_FFMPEG_H264_VAAPI, 1920, 1080, 60);

	return audio_codec;
}

void
audio_codec_destroy(audio_codec_t *audio_codec)
{
	free(audio_codec);
}

void
audio_codec_encode(struct audio_codec *audio_codec, uint8_t *buffer)
{
	audio_codec_ffmpeg_encode(audio_codec->ffmpeg, buffer);
	audio_codec->encoded_buffer = audio_codec->ffmpeg->av_packet->data;
	audio_codec->encoded_buffer_size = audio_codec->ffmpeg->av_packet->size;
}

void
audio_codec_decode(struct audio_codec *audio_codec, uint8_t *encoded_buffer, uint32_t encoded_buffer_size)
{
	audio_codec_ffmpeg_decode(audio_codec->ffmpeg, encoded_buffer, encoded_buffer_size);
}
