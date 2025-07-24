#ifndef VIDEO_ENCODE_ONEVPL_H
#define VIDEO_ENCODE_ONEVPL_H

#include <stdint.h>
#include <stdbool.h>

#include <vpl/mfx.h>

#include <video-encode.h>

struct video_encode_onevpl {
	mfxSession           mfx_session;
	mfxVersion           mfx_version;
	mfxVideoParam        mfx_video_param;
	mfxBitstream         mfx_bitstream;
	mfxFrameSurface1     *mfx_frame_surface1;
	mfxIMPL              mfx_impl;
	mfxFrameAllocRequest mfx_frame_alloc_request;
	mfxEncodeStat        mfx_encode_stat;
	mfxU8                *buffer;
	void                 *sharedMemory;
};

extern struct video_encode_interface video_encode_interface_onevpl;

#endif
