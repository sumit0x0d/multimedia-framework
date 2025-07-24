#include <assert.h>
#include <stdbool.h>

#include <unistd.h>

#include "video-encode-ffmpeg.h"

#define CODEC_NAME VIDEO_ENCODE_FFMPEG_H264_QSV

typedef struct video_encode_ffmpeg Ffmpeg;
typedef struct video_encode_create_config CreateConfig;
typedef struct video_encode_encoded_buffer EncodedBuffer;

static void *
_ffmpeg_create(const CreateConfig *cconfig);
static void
_ffmpeg_destroy(void *backend);
static EncodedBuffer
_ffmpeg_dispatch(void *backend, const void *buffer);

struct video_encode_interface video_encode_interface_ffmpeg = {
     .create = _ffmpeg_create,
     .destroy = _ffmpeg_destroy,
     .dispatch = _ffmpeg_dispatch,
};

static void
_ffmpeg_send_av_frame(AVCodecContext *accontext, AVFrame *aframe, AVFrame *ahwframe,
     uint8_t *buffer);
static void
_ffmpeg_send_av_hwframe(AVCodecContext *accontext, AVFrame *aframe, AVFrame *ahwframe,
     uint8_t *buffer);

static void *
_ffmpeg_create(const CreateConfig *cconfig)
{
     Ffmpeg *ffmpeg = (Ffmpeg *)malloc(sizeof (Ffmpeg));
     assert(ffmpeg);
     AVCodec *acodec = NULL;
     acodec = avcodec_find_encoder_by_name(CODEC_NAME);
     const AVCodecHWConfig *achconfig = avcodec_get_hw_config(acodec, 0);
     ffmpeg->av_codec_context = avcodec_alloc_context3(acodec);
     ffmpeg->av_codec_context->width = cconfig->width;
     ffmpeg->av_codec_context->height = cconfig->height;
     ffmpeg->av_codec_context->bit_rate = cconfig->avc.bitrate;
     ffmpeg->av_codec_context->gop_size = 0;
     ffmpeg->av_codec_context->time_base = (AVRational){1, cconfig->frame_rate};
     ffmpeg->av_codec_context->global_quality = cconfig->mjpeg.quality;
     ffmpeg->av_codec_context->framerate = (AVRational){cconfig->frame_rate, 1};
     ffmpeg->av_codec_context->max_b_frames = 1;
     if (achconfig) {
          ffmpeg->av_codec_context->pix_fmt = achconfig->pix_fmt;
          av_hwdevice_ctx_create(&ffmpeg->av_codec_context->hw_device_ctx,
               achconfig->device_type, NULL, NULL, 0);
          ffmpeg->av_codec_context->hw_frames_ctx = av_hwframe_ctx_alloc(
               ffmpeg->av_codec_context->hw_device_ctx);
          AVHWFramesContext *ahfcontext = (AVHWFramesContext *)
               (ffmpeg->av_codec_context->hw_frames_ctx->data);
          ahfcontext->format = achconfig->pix_fmt;
          ahfcontext->sw_format = AV_PIX_FMT_NV12;
          ahfcontext->width = cconfig->width;
          ahfcontext->height = cconfig->height;
          ahfcontext->initial_pool_size = 20;
          int result = av_hwframe_ctx_init(ffmpeg->av_codec_context->hw_frames_ctx);
          if (result < 0) {
               printf("av_hwframe_ctx_init %d %s", result, av_err2str(result));
               return NULL;
          }
     } else {
          ffmpeg->av_codec_context->pix_fmt = AV_PIX_FMT_NV12;
     }
     int result = avcodec_open2(ffmpeg->av_codec_context, acodec, NULL);
     if (result < 0) {
          printf("cant open codec for this\n");
     } else {
          printf("codec context open\n");
     }
     ffmpeg->av_packet = av_packet_alloc();
     ffmpeg->av_frame = av_frame_alloc();
     ffmpeg->av_frame->format = AV_PIX_FMT_NV12;
     ffmpeg->av_frame->width = cconfig->width;
     ffmpeg->av_frame->height = cconfig->height;
     ffmpeg->av_frame->pts = 0;
     result = av_frame_get_buffer(ffmpeg->av_frame, 0);
     if (result < 0) {
          printf("av_frame_get_buffer() result = %d %s\n", result, av_err2str(result));
          exit(1);
     }
     if (achconfig) {
          ffmpeg->av_hwframe = av_frame_alloc();
          result = av_hwframe_get_buffer(ffmpeg->av_codec_context->hw_frames_ctx,
               ffmpeg->av_hwframe, 0);
          if (result < 0) {
               printf("av_hwframe_get_buffer() result = %d %s\n", result,
                    av_err2str(result));
               exit(1);
          }
          ffmpeg->_send_av_frame = _ffmpeg_send_av_hwframe;
     } else {
          ffmpeg->av_hwframe = NULL;
          ffmpeg->_send_av_frame = _ffmpeg_send_av_frame;
     }
     av_image_alloc(ffmpeg->av_frame->data, ffmpeg->av_frame->linesize,
          ffmpeg->av_frame->width, ffmpeg->av_frame->height, AV_PIX_FMT_NV12, 1);
     // ffmpeg->av_sws_context = sws_alloc_context();
     return ffmpeg;
}

static void
_ffmpeg_destroy(void *backend)
{
     Ffmpeg *ffmpeg = (Ffmpeg *)backend;
     avcodec_close(ffmpeg->av_codec_context);
     av_frame_free(&ffmpeg->av_frame);
     av_frame_free(&ffmpeg->av_hwframe);
     av_packet_free(&ffmpeg->av_packet);
     free(ffmpeg);
}

static EncodedBuffer
_ffmpeg_dispatch(void *backend, const void *buffer)
{
     Ffmpeg *ffmpeg = (Ffmpeg *)backend;
     EncodedBuffer encoded_buffer = {0};
     int result;
     ffmpeg->_send_av_frame(ffmpeg->av_codec_context, ffmpeg->av_frame,
          ffmpeg->av_hwframe, (uint8_t *)buffer);
     while (result >= 0) {
          result = avcodec_receive_packet(ffmpeg->av_codec_context, ffmpeg->av_packet);
          if(AVERROR(result) == EAGAIN) {
               continue;
          }
          if (result < 0) {
               printf("result = %d %s\n", result, av_err2str(result));
          }
     }
     encoded_buffer.data = ffmpeg->av_packet->data;
     encoded_buffer.size = (size_t)ffmpeg->av_packet->size;
     return encoded_buffer;
}

static void
_ffmpeg_decode(void *backend, const void *ebuffer, size_t ebsize, void *buffer)
{
     Ffmpeg *ffmpeg = (Ffmpeg *)backend;
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
_ffmpeg_send_av_frame(AVCodecContext *accontext, AVFrame *aframe, AVFrame *ahwframe,
     uint8_t *buffer)
{
     (void)ahwframe;
     aframe->pts++;
     int result = av_image_fill_pointers(aframe->data, AV_PIX_FMT_NV12, aframe->height,
          buffer, aframe->linesize);
     if (result < 0) {
          printf("av_image_fill_pointers() result = %d %s\n", result,
               av_err2str(result));
     }
     result = avcodec_send_frame(accontext, aframe);
     if (result < 0) {
          printf("avcodec_send_frame() result = %d %s\n", result, av_err2str(result));
     }
}

static void
_ffmpeg_send_av_hwframe(AVCodecContext *accontext, AVFrame *aframe, AVFrame *ahwframe,
     uint8_t *buffer)
{
     aframe->pts++;
     int result = av_image_fill_pointers(aframe->data, AV_PIX_FMT_NV12, aframe->height,
          buffer, aframe->linesize);
     if (result < 0) {
          printf("av_image_fill_pointers() result = %d %s\n", result,
               av_err2str(result));
     }
     // av_image_fill_arrays(av_frame->data, av_frame->linesize, buffer, AV_PIX_FMT_RGB32,
     //      av_frame->width, av_frame->height, 0);
     // av_hwframe_transfer_get_formats(av_hwframe, AV_HWFRAME_TRANSFER_DIRECTION_FROM, );
     result = av_hwframe_transfer_data(ahwframe, aframe, 0);
     if (result < 0) {
          printf("av_hwframe_transfer_data() result = %d %s\n", result,
               av_err2str(result));
     }
     result = avcodec_send_frame(accontext, ahwframe);
     if (result < 0) {
          printf("avcodec_send_frame() result = %d %s\n", result, av_err2str(result));
     }
}
