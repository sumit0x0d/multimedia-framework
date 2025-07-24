#include <assert.h>
#include <stdbool.h>

#include <unistd.h>

#include "audio-codec-ffmpeg.h"

typedef struct audio_codec_ffmpeg ffmpeg_t;

static void
_ffmpeg_send_av_frame(AVCodecContext *av_codec_context, AVFrame *av_frame, AVFrame *av_hwframe, uint8_t *buffer);
static void
_ffmpeg_send_av_hwframe(AVCodecContext *av_codec_context, AVFrame *av_frame, AVFrame *av_hwframe, uint8_t *buffer);

struct audio_codec_ffmpeg *
audio_codec_ffmpeg_create(bool encoder, const char *codec_name, int width, int height, uint32_t frame_rate)
{
	int result = 0;
	ffmpeg_t *ffmpeg = (ffmpeg_t *)malloc(sizeof (ffmpeg_t));
	assert(ffmpeg);
	AVCodec *av_codec = NULL;

	if (encoder) {
		av_codec = avcodec_find_encoder_by_name(codec_name);
	} else {
		av_codec = avcodec_find_decoder_by_name(codec_name);
	}

	ffmpeg->av_codec_context = avcodec_alloc_context3(av_codec);

	ffmpeg->av_codec_context->width = width;
	ffmpeg->av_codec_context->height = height;
	ffmpeg->av_codec_context->global_quality = 90;
	ffmpeg->av_codec_context->bit_rate = 400000;
	ffmpeg->av_codec_context->gop_size = frame_rate;
	ffmpeg->av_codec_context->time_base = (AVRational){1, frame_rate};
	ffmpeg->av_codec_context->framerate = (AVRational){frame_rate, 1};
	ffmpeg->av_codec_context->max_b_frames = 1;

	const AVCodecHWConfig *av_codec_hw_config = avcodec_get_hw_config(av_codec, 0);

	if (av_codec_hw_config) {
		ffmpeg->av_codec_context->pix_fmt = av_codec_hw_config->pix_fmt;
		
		av_hwdevice_ctx_create(&ffmpeg->av_codec_context->hw_device_ctx, av_codec_hw_config->device_type, NULL, NULL, 0);
		ffmpeg->av_codec_context->hw_frames_ctx = av_hwframe_ctx_alloc(ffmpeg->av_codec_context->hw_device_ctx);

		AVHWFramesContext *av_hw_frames_context = (AVHWFramesContext *)(ffmpeg->av_codec_context->hw_frames_ctx->data);

		av_hw_frames_context->format = av_codec_hw_config->pix_fmt;
		av_hw_frames_context->sw_format = AV_PIX_FMT_NV12;
		av_hw_frames_context->width = width;
		av_hw_frames_context->height = height;
		av_hw_frames_context->initial_pool_size = 20;

		result = av_hwframe_ctx_init(ffmpeg->av_codec_context->hw_frames_ctx);
		if (result < 0) {
			printf("av_hwframe_ctx_init %d %s", result, av_err2str(result));
			return NULL;
		}
	} else {
		ffmpeg->av_codec_context->pix_fmt = AV_PIX_FMT_NV12;
	}

	result = avcodec_open2(ffmpeg->av_codec_context, av_codec, NULL);
	if (result < 0) {
		printf("cant open codec for this\n");
	} else {
		printf("codec context open\n");
	}

	ffmpeg->av_packet = av_packet_alloc();
	ffmpeg->av_frame = av_frame_alloc();

	ffmpeg->av_frame->format = AV_PIX_FMT_NV12;
	ffmpeg->av_frame->width = width;
	ffmpeg->av_frame->height = height;
	ffmpeg->av_frame->pts = 0;

	result = av_frame_get_buffer(ffmpeg->av_frame, 0);
	if (result < 0) {
		printf("av_frame_get_buffer() result = %d %s\n", result, av_err2str(result));
		exit(1);
	}

	if (av_codec_hw_config) {
		ffmpeg->av_hwframe = av_frame_alloc();
		result = av_hwframe_get_buffer(ffmpeg->av_codec_context->hw_frames_ctx, ffmpeg->av_hwframe, 0);
		if (result < 0) {
			printf("av_hwframe_get_buffer() result = %d %s\n", result, av_err2str(result));
			exit(1);
		}
		ffmpeg->send_av_frame = _ffmpeg_send_av_hwframe;
	} else {
		ffmpeg->av_hwframe = NULL;
		ffmpeg->send_av_frame = _ffmpeg_send_av_frame;
	}

	av_image_alloc(ffmpeg->av_frame->data, ffmpeg->av_frame->linesize, ffmpeg->av_frame->width, ffmpeg->av_frame->height, AV_PIX_FMT_NV12,
		1);

	return ffmpeg;
}

void
audio_codec_ffmpeg_destroy(struct audio_codec_ffmpeg *ffmpeg)
{
	avcodec_close(ffmpeg->av_codec_context);
	av_frame_free(&ffmpeg->av_frame);
	av_frame_free(&ffmpeg->av_hwframe);
	av_packet_free(&ffmpeg->av_packet);
	free(ffmpeg);
}

void
audio_codec_ffmpeg_encode(struct audio_codec_ffmpeg *ffmpeg, uint8_t *buffer)
{
	int result;

	ffmpeg->send_av_frame(ffmpeg->av_codec_context, ffmpeg->av_frame, ffmpeg->av_hwframe, buffer);

	while (result >= 0) {
		result = avcodec_receive_packet(ffmpeg->av_codec_context, ffmpeg->av_packet);
		if(AVERROR(result) == EAGAIN) {
			continue;
		}
		if (result < 0) {
			printf("result = %d %s\n", result, av_err2str(result));
		}
	}
}

void
audio_codec_ffmpeg_decode(struct audio_codec_ffmpeg *ffmpeg, uint8_t *encoded_buffer, uint32_t encoded_buffer_size)
{
	int result = avcodec_send_packet(ffmpeg->av_codec_context, ffmpeg->av_packet);

	if (result < 0) {
		fprintf(stderr, "Error sending a packet for decoding.\n");
	}

	result = avcodec_receive_frame(ffmpeg->av_codec_context, ffmpeg->av_frame);

	while (result == 0) {
		av_frame_unref(ffmpeg->av_frame);
	}
}

static void
_ffmpeg_send_av_frame(AVCodecContext *av_codec_context, AVFrame *av_frame, AVFrame *av_hwframe, uint8_t *buffer)
{
	(void)av_hwframe;
	int result = 0;

	av_frame->pts++;
	result = av_image_fill_pointers(av_frame->data, AV_PIX_FMT_NV12, av_frame->height, buffer, av_frame->linesize);

	if (result < 0) {
		printf("av_image_fill_pointers() result = %d %s\n", result, av_err2str(result));
	}

	result = avcodec_send_frame(av_codec_context, av_frame);

	if (result < 0) {
		printf("avcodec_send_frame() result = %d %s\n", result, av_err2str(result));
	}
}

static void
_ffmpeg_send_av_hwframe(AVCodecContext *av_codec_context, AVFrame *av_frame, AVFrame *av_hwframe, uint8_t *buffer)
{
	int result = 0;

	av_frame->pts++;
	result = av_image_fill_pointers(av_frame->data, AV_PIX_FMT_NV12, av_frame->height, buffer, av_frame->linesize);
	if (result < 0) {
		printf("av_image_fill_pointers() result = %d %s\n", result, av_err2str(result));
	}
	// av_image_fill_arrays(av_frame->data, av_frame->linesize, buffer, AV_PIX_FMT_RGB32, av_frame->width, av_frame->height, 0);
	// av_hwframe_transfer_get_formats(av_hwframe, AV_HWFRAME_TRANSFER_DIRECTION_FROM, );
	result = av_hwframe_transfer_data(av_hwframe, av_frame, 0);

	if (result < 0) {
		printf("av_hwframe_transfer_data() result = %d %s\n", result, av_err2str(result));
	}

	result = avcodec_send_frame(av_codec_context, av_hwframe);

	if (result < 0) {
		printf("avcodec_send_frame() result = %d %s\n", result, av_err2str(result));
	}
}
