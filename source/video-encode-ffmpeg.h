#ifndef VIDEO_ENCODE_FFMPEG_H
#define VIDEO_ENCODE_FFMPEG_H

#include <stdbool.h>

#include <video-encode.h>

#include <libavcodec/avcodec.h>
#include <libavutil/hwcontext.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

struct video_encode_ffmpeg {
	AVCodecContext *av_codec_context;
	AVFrame *av_frame;
	AVFrame *av_hwframe;
	AVPacket *av_packet;
	struct SwsContext *sws_context;
	void (*_send_av_frame)(AVCodecContext *av_codec_context, AVFrame *av_frame,
          AVFrame *av_hwframe, uint8_t *buffer);
};

static const char *VIDEO_ENCODE_FFMPEG_MJPEG = "mjpeg";
static const char *VIDEO_ENCODE_FFMPEG_MJPEG_VAAPI = "mjpeg_vaapi";
static const char *VIDEO_ENCODE_FFMPEG_MJPEG_QSV = "mjpeg_qsv";
static const char *VIDEO_ENCODE_FFMPEG_MJPEG_CUDA = "mjpeg_cuvid";

static const char *VIDEO_ENCODE_FFMPEG_H264 = "libx264";
static const char *VIDEO_ENCODE_FFMPEG_H264_VAAPI = "h264_vaapi";
static const char *VIDEO_ENCODE_FFMPEG_H264_QSV = "h264_qsv";
static const char *VIDEO_ENCODE_FFMPEG_H264_CUDA = "h264_cuvid";

static const char *VIDEO_ENCODE_FFMPEG_DECODER_MJPEG = "mjpeg";
static const char *VIDEO_ENCODE_FFMPEG_DECODER_MJPEG_VAAPI = "mjpeg_vaapi";
static const char *VIDEO_ENCODE_FFMPEG_DECODER_MJPEG_QSV = "mjpeg_qsv";
static const char *VIDEO_ENCODE_FFMPEG_DECODER_MJPEG_CUDA = "mjpeg_cuvid";

static const char *VIDEO_ENCODE_FFMPEG_DECODER_H264 = "h264";
static const char *VIDEO_ENCODE_FFMPEG_DECODER_H264_VAAPI = "h264";
static const char *VIDEO_ENCODE_FFMPEG_DECODER_H264_QSV = "h264_qsv";
static const char *VIDEO_ENCODE_FFMPEG_DECODER_H264_CUDA = "h264_cuvid";

extern struct video_encode_interface video_encode_interface_ffmpeg;

#endif
