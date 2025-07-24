#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "video-encode-onevpl.h"

// #include <va/va_drm.h>

typedef struct video_encode_onevpl _onevpl_t;
typedef struct video_encode_create_config _create_config_t;
typedef struct video_encode_interface _interface_t;

static void *
_onevpl_create(const _create_config_t *create_config);
static void
_onevpl_destroy(void *backend);
static void
_onevpl_dispatch(void *backend, const void *buffer, void *encoded_buffer, size_t *encoded_buffer_size);

_interface_t video_encode_interface_onevpl = {
	.create = _onevpl_create,
	.destroy = _onevpl_destroy,
	.dispatch = _onevpl_dispatch,
};

static void *
_onevpl_create(const _create_config_t *create_config)
{
	_onevpl_t *onevpl = (_onevpl_t *)malloc(sizeof (_onevpl_t));
	assert(onevpl);

	// mfxVersion version = {1, 0};
	mfxLoader loader = MFXLoad();
	mfxStatus status = 0;

	mfxConfig cfg = MFXCreateConfig(loader);

	// mfxVariant value = {
	// 	.Type = MFX_VARIANT_TYPE_U32,
	// 	.Data.U32 = MFX_ENCODE_JPEG
	// };
	// mfxImplDescription x;

	// status  = MFXSetConfigFilterProperty(cfg, x.Dec.Codecs->CodecID, value);

	// ImplValue.Type = MFX_VARIANT_TYPE_U32;
	// ImplValue.Data.U32 = MFX_ENCODE_HEVC;
	// while(1) {
	// 	if(is_good(idesc)) {
	// 		MFXCreateSession(loader, i,&session);
	// 		// ...
	// 	}
	// 	else
	// 		{
	// 			MFXDispReleaseImplDescription(loader, idesc);
	// 			break;
	// 		}
	// }

	// mfxImplDescription *idesc;
	// MFXEnumImplementations(loader, 0, MFX_IMPLCAPS_IMPLDESCSTRUCTURE, (mfxHDL*)&idesc);
	status = MFXCreateSession(loader, 0, &onevpl->mfx_session);
	// MFXDispReleaseImplDescription(loader, idesc);

	if (status < MFX_ERR_NONE || status == MFX_WRN_PARTIAL_ACCELERATION) {
		printf("[MFX] ERROR: MFXCreateSession() %d\n", status);
		// return NULL;
	} else {
		printf("[MFX] INFO: MFXCreateSession() %d\n", status);
	}

	onevpl->mfx_impl = MFX_IMPL_HARDWARE;

	status = MFXQueryIMPL(onevpl->mfx_session, &onevpl->mfx_impl);
	// status = MFXQueryVersion(onevpl->mfx_session, &onevpl->mfx_version);
	if (status < MFX_ERR_NONE || status == MFX_WRN_PARTIAL_ACCELERATION) {
		printf("[MFX] ERROR: MFXQueryIMPL() %d\n", status);
		// return NULL;
	} else {
		printf("[MFX] INFO: MFXQueryIMPL() %d\n", status);
	}

	// int card = open("/dev/dri/card0", O_RDWR);
	// assert(card >= 0);

	// VADisplay vd = vaGetDisplayDRM(card);
	// int majorVersion = VA_MAJOR_VERSION, minorVersion = VA_MINOR_VERSION;
	// VAStatus s = vaInitialize(vd, &majorVersion, &minorVersion);

	// if (s != VA_STATUS_SUCCESS) {
	// 	printf("vaInitialize failed");
	// }

	// status = MFXVideoCORE_SetHandle(onevpl->mfx_session, MFX_HANDLE_VA_DISPLAY, vd);

	if (status != MFX_ERR_NONE) {
		printf("[MFX] ERROR: MFXVideoCORE_SetHandle() %d\n", status);
	}

	memset(&onevpl->mfx_bitstream, 0, sizeof (mfxBitstream));
	onevpl->mfx_bitstream.MaxLength = create_config->width * create_config->height * 4;
	onevpl->mfx_bitstream.Data = (mfxU8 *)calloc(onevpl->mfx_bitstream.MaxLength, sizeof (mfxU8));
	// onevpl->mfx_bitstream.DataLength = onevpl->mfx_bitstream.MaxLength;
	// onevpl->mfx_bitstream.DataOffset = 0;

	switch (create_config->coding_format) {
	case VIDEO_ENCODE_CODING_FORMAT_MJPEG:
		onevpl->mfx_bitstream.CodecId = MFX_CODEC_JPEG;
		break;
	case VIDEO_ENCODE_CODING_FORMAT_AVC:
		break;
	}

	onevpl->mfx_bitstream.PicStruct  = MFX_PICSTRUCT_PROGRESSIVE;
	// onevpl->mfx_bitstream
	assert(onevpl->mfx_bitstream.Data);

	memset(&onevpl->mfx_video_param, 0, sizeof (mfxVideoParam));

	onevpl->mfx_video_param.mfx.CodecId = onevpl->mfx_bitstream.CodecId;

	onevpl->mfx_video_param.mfx.FrameInfo.FrameRateExtN = create_config->frame_rate;
	onevpl->mfx_video_param.mfx.FrameInfo.FrameRateExtD = 1;
	onevpl->mfx_video_param.mfx.FrameInfo.FourCC = MFX_FOURCC_NV12;
	onevpl->mfx_video_param.mfx.FrameInfo.Width = create_config->width;
	onevpl->mfx_video_param.mfx.FrameInfo.Height = create_config->height + 8;
	onevpl->mfx_video_param.mfx.FrameInfo.CropW = create_config->width;
	onevpl->mfx_video_param.mfx.FrameInfo.CropH = create_config->height;
	onevpl->mfx_video_param.mfx.FrameInfo.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;

	if (onevpl->mfx_video_param.mfx.CodecId == MFX_CODEC_JPEG) {
		// if (!create_config->encoder) {
			onevpl->mfx_video_param.mfx.RestartInterval = 1;
		// }
		onevpl->mfx_video_param.mfx.Interleaved = MFX_SCANTYPE_INTERLEAVED;
		onevpl->mfx_video_param.mfx.Quality = 90;
		switch (create_config->chroma_subsample) {
		case VIDEO_ENCODE_CHROMA_SUBSAMPLE_YUV444:
		case VIDEO_ENCODE_CHROMA_SUBSAMPLE_YUV422:
		case VIDEO_ENCODE_CHROMA_SUBSAMPLE_YUV420:
			onevpl->mfx_video_param.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
			break;
		}
	} else {
		// if (create_config->encoder) {
			onevpl->mfx_video_param.mfx.TargetUsage = MFX_TARGETUSAGE_BALANCED;
			onevpl->mfx_video_param.mfx.RateControlMethod = MFX_RATECONTROL_VBR;
			onevpl->mfx_video_param.mfx.TargetKbps = 40000;
			onevpl->mfx_video_param.mfx.GopPicSize = 1;
		// }
		onevpl->mfx_video_param.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
	}

	mfxVideoParam mvp = {0};
	mvp.mfx.CodecId = MFX_CODEC_JPEG;

	if (create_config->encoder) {
		onevpl->mfx_video_param.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY;
		status = MFXVideoENCODE_Query(onevpl->mfx_session, &onevpl->mfx_video_param, &onevpl->mfx_video_param);
		if (status != MFX_ERR_NONE) {
			printf("[MFX] ERROR: MFXVideoENCODE_Query() %d\n", status);
			exit(0);
		}
		printf("mfx_video_param %d\n", onevpl->mfx_video_param.mfx.FrameInfo.CropH);		
		status = MFXVideoENCODE_Init(onevpl->mfx_session, &onevpl->mfx_video_param);
		if (status != MFX_ERR_NONE) {
			printf("[MFX] ERROR: MFXVideoENCODE_Init() %d\n", status);
			exit(0);
		}
		status = MFXVideoENCODE_QueryIOSurf(onevpl->mfx_session, &onevpl->mfx_video_param, &onevpl->mfx_frame_alloc_request);
		if (status != MFX_ERR_NONE) {
			printf("[MFX] ERROR: MFXVideoENCODE_QueryIOSurf() %d\n", status);
			exit(0);
		}
	} else {
		onevpl->mfx_video_param.IOPattern = MFX_IOPATTERN_OUT_VIDEO_MEMORY;
		status = MFXVideoDECODE_GetVideoParam(onevpl->mfx_session, &onevpl->mfx_video_param);
		if (status != MFX_ERR_NONE) {
			printf("[MFX] ERROR: MFXVideoDECODE_GetVideoParam() %d\n", status);
			exit(0);
		}		
		// status = MFXVideoDECODE_DecodeHeader(onevpl->mfx_session, &onevpl->mfx_bitstream, &onevpl->mfx_video_param);
		// if (status != MFX_ERR_NONE) {
		// 	printf("[MFX] ERROR: MFXVideoDECODE_DecodeHeader() %d\n", status);
		// 	exit(0);
		// }
		status = MFXVideoDECODE_Query(onevpl->mfx_session, &onevpl->mfx_video_param, &mvp);
		if (status != MFX_ERR_NONE) {
			printf("[MFX] ERROR: MFXVideoDECODE_Query() %d\n", status);
			exit(0);
		}
		// status = MFXVideoDECODE_Init(onevpl->mfx_session, &onevpl->mfx_video_param);
		// if (status != MFX_ERR_NONE) {
		// 	printf("[MFX] ERROR: MFXVideoDECODE_Init() %d\n", status);
		// 	exit(0);
		// }
		// status = MFXVideoDECODE_QueryIOSurf(onevpl->mfx_session, &onevpl->mfx_video_param, &onevpl->mfx_frame_alloc_request);
		// if (status != MFX_ERR_NONE) {
		// 	printf("[MFX] ERROR: MFXVideoDECODE_QueryIOSurf() %d\n", status);
		// 	exit(0);
		// }
	}

	// printf("onevpl->mfx_frame_alloc_request %d\n", onevpl->mfx_frame_alloc_request.Info.Height);

	return onevpl;
}

void
_onevpl_destroy(void *backend)
{
	_onevpl_t *onevpl = (_onevpl_t *)backend;
	
	free(onevpl);
}

#define VERIFY(x, y)				\
	if (!(x)) {				\
		printf("%s\n", y);		\
		isFailed = true;		\
		goto end;			\
	}

static void
_onevpl_dispatch(void *backend, const void *buffer, void *encoded_buffer, size_t *encoded_buffer_size)
{

	_onevpl_t *onevpl = (_onevpl_t *)backend;
	mfxU8 *buffer1 = (mfxU8 *)buffer;
	// mfxStatus status = MFXMemory_GetSurfaceForEncode(onevpl->mfx_session, &onevpl->mfx_frame_surface1);
	
	mfxSyncPoint syncp = NULL;
	mfxStatus status = MFX_ERR_NONE;
	bool isDraining                = false;
	bool isStillGoing              = true;
	bool isFailed                  = false;

	while (isStillGoing == true) {
		// Load a new frame if not draining
		if (isDraining == false) {
			status = MFXMemory_GetSurfaceForEncode(onevpl->mfx_session, &onevpl->mfx_frame_surface1);
			VERIFY(MFX_ERR_NONE == status, "Could not get encode surface");
			bool is_more_data = false;
			// Map makes surface writable by CPU for all implementations
			status = onevpl->mfx_frame_surface1->FrameInterface->Map(onevpl->mfx_frame_surface1, MFX_MAP_WRITE);
			if (status != MFX_ERR_NONE) {
				printf("mfxFrameSurfaceInterface->Map failed (%d)\n", status);
			}
			memcpy(onevpl->mfx_frame_surface1->Data.B, buffer, 1920*1080*4);
			// Unmap/release returns local device access for all implementations
			status = onevpl->mfx_frame_surface1->FrameInterface->Unmap(onevpl->mfx_frame_surface1);
			if (status != MFX_ERR_NONE) {
				printf("mfxFrameSurfaceInterface->Unmap failed (%d)\n", status);
			}
			if (status != MFX_ERR_NONE)
				isDraining = true;
		}
		status = MFXVideoENCODE_EncodeFrameAsync(onevpl->mfx_session, NULL, (isDraining == true) ? NULL : onevpl->mfx_frame_surface1,
			&onevpl->mfx_bitstream, &syncp);
		MFXVideoENCODE_GetEncodeStat(onevpl->mfx_session, &onevpl->mfx_encode_stat);
		printf("%d\n", onevpl->mfx_encode_stat.NumFrame);
		if (!isDraining) {
			status = onevpl->mfx_frame_surface1->FrameInterface->Release(onevpl->mfx_frame_surface1);
			if (status != MFX_ERR_NONE) {
				printf("onevpl->mfx_frame_surface1->FrameInterface->Release failed (%d)\n", status);
			}
			VERIFY(MFX_ERR_NONE == status, "mfxFrameSurfaceInterface->Release failed");
		}
		switch (status) {
		case MFX_ERR_NONE:
			// MFX_ERR_NONE and syncp indicate output is available
			if (syncp) {
				// Encode output is not available on CPU until sync operation
				// completes
				do {
					status = MFXVideoCORE_SyncOperation(onevpl->mfx_session, syncp, 10000);
					if (MFX_ERR_NONE == status) {
						printf("**\n");
						// WriteEncodedstream(bitstream, sink);
						// framenum++;
					}
				} while (status == MFX_WRN_IN_EXECUTION);
			}
			break;
		case MFX_ERR_NOT_ENOUGH_BUFFER:
			// This example deliberatly uses a large output buffer with immediate
			// write to disk for simplicity. Handle when frame size exceeds
			// available buffer here
			break;
		case MFX_ERR_MORE_DATA:
			// The function requires more data to generate any output
			if (isDraining == true)
				isStillGoing = false;
			break;
		case MFX_ERR_DEVICE_LOST:
			// For non-CPU implementations,
			// Cleanup if device is lost
			break;
		case MFX_WRN_DEVICE_BUSY:
			// For non-CPU implementations,
			// Wait a few milliseconds then try again
			printf("busy %d\n", status);
			break;
		default:
			isStillGoing = false;
			break;
		}
	}
 end:
	printf("here");
}

void video_encode_onevpl_decode(_onevpl_t *onevpl, mfxU8* encoded_buffer, mfxU32 encoded_buffer_size)
{
	mfxSyncPoint mfx_sync_point;
	memset(&mfx_sync_point, 0, sizeof(mfxSyncPoint));
	mfxU16 index = 0;
	mfxStatus status = MFX_ERR_NONE;

	onevpl->mfx_bitstream.Data = encoded_buffer;
	onevpl->mfx_bitstream.DataOffset = 0;
	onevpl->mfx_bitstream.DataLength = encoded_buffer_size;

	// index = getFreeSurfaceIndex(&mfx->frameSurface1In, numFrameSuggested);
	for (;;) {
		status = MFXVideoDECODE_DecodeFrameAsync(onevpl->mfx_session, &onevpl->mfx_bitstream, &onevpl->mfx_frame_surface1[index],
			&onevpl->mfx_frame_surface1, &mfx_sync_point);
		if (status < MFX_ERR_NONE) {
			printf("[MFX] ERROR: MFXVideoDECODE_Init() %d\n", status);
			exit(0);
		} else if (status == MFX_WRN_DEVICE_BUSY) {
			sleep(1);
		} else {
			break;
		}
	}

	if (status == MFX_ERR_NONE && mfx_sync_point) {
		status = MFXVideoCORE_SyncOperation(onevpl->mfx_session, mfx_sync_point, 60000);
		if (status < MFX_ERR_NONE) {
			printf("[MFX] ERROR: MFXVideoCORE_SyncOperation() %d\n", status);
			exit(0);
		} else {
			// saveToFile(mfx->sharedMemory);
			// exit(0);
			// printf("[MFX] INFO: MFXVideoCORE_SyncOperation() %d\n", status);
		}
	}
}
