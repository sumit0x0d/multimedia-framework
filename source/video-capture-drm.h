#ifndef VIDEO_CAPTURE_DRM_H
#define VIDEO_CAPTURE_DRM_H

#include <drm.h>

#include <video-capture.h>

struct video_capture_drm {
	int fd;
	struct drm_mode_get_encoder *drm_mode_get_encoder;
	struct drm_mode_get_plane *drm_mode_get_plane;
	struct drm_mode_modeinfo *drm_mode_modeinfo;
	struct drm_mode_card_res *drm_mode_card_res;
};

extern struct video_capture_interface video_capture_interface_drm;

#endif
