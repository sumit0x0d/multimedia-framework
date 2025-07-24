#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <drm_mode.h>

#include "video-capture-drm.h"

typedef struct video_capture_drm Drm;
typedef struct video_capture_create_config _create_config_t;
typedef struct video_capture_interface _interface_t;

static void *
_drm_create(const _create_config_t *create_config);
static void
_drm_destroy(void *backend);

struct video_capture_interface video_capture_interface_ffmpeg = {
	.create = _drm_create,
	.destroy = _drm_destroy
};

static void *
_drm_create(const _create_config_t *create_config)
{
	Drm *drm = (Drm *)malloc(sizeof (Drm));
	struct drm_mode_card_res drm_mode_card_res = {0};
	drm = (Drm *)malloc(sizeof (Drm));
	assert(drm);
	drm->fd = open("/dev/dri/card0", O_RDONLY);
	if (drm->fd < 0) {
		printf("drm->fd failed\n");
	}
	int result = 0;
	result = ioctl(drm->fd, DRM_IOCTL_SET_MASTER, 0);
	if (result == -1) {
		printf("ioctl failed\n");
	}
	// ioctl(drm->fd, DRM_IOCTL_GET_CAP, drm_get_cap);
	result = ioctl(drm->fd, DRM_IOCTL_MODE_GETRESOURCES, &drm_mode_card_res);
	if (result == -1) {
		printf("ioctl failed\n");
	}
	if (drm_mode_card_res.count_fbs) {
		printf("found count_fbs %d\n", drm_mode_card_res.count_fbs);
		drm_mode_card_res.fb_id_ptr = (__u64)calloc(drm_mode_card_res.count_fbs, sizeof (__u64));
	}
	if (drm_mode_card_res.count_crtcs) {
		printf("found count_crtcs %d\n", drm_mode_card_res.count_crtcs);
		drm_mode_card_res.crtc_id_ptr = (__u64)calloc(drm_mode_card_res.count_crtcs, sizeof (__u64));
	}
	if (drm_mode_card_res.count_connectors) {
		printf("found count_connectors %d\n", drm_mode_card_res.count_connectors);
		drm_mode_card_res.connector_id_ptr = (__u64)calloc(drm_mode_card_res.count_connectors, sizeof (__u64));
	}
	if (drm_mode_card_res.count_encoders) {
		printf("found count_encoders %d\n", drm_mode_card_res.count_encoders);
		drm_mode_card_res.encoder_id_ptr = (__u64)calloc(drm_mode_card_res.count_encoders, sizeof (__u64));
	}
	result = ioctl(drm->fd, DRM_IOCTL_MODE_GETRESOURCES, &drm_mode_card_res);
	if (result == -1) {
		printf("ioctl failed\n");
	}
	for (__u32 i = 0; i < drm_mode_card_res.count_connectors; i++) {
		__u32 connector_id = ((__u32 *)drm_mode_card_res.connector_id_ptr)[i];
		struct drm_mode_get_connector drm_mode_get_connector = {0};
		drm_mode_get_connector.connector_id = connector_id;
		printf("connector_id %d\n", drm_mode_get_connector.connector_id);
		result = ioctl(drm->fd, DRM_IOCTL_MODE_GETCONNECTOR, &drm_mode_get_connector);
		if (result == -1) {
			printf("ioctl failed DRM_IOCTL_MODE_GETCONNECTOR\n");
		}
		if (drm_mode_get_connector.count_props) {
			drm_mode_get_connector.props_ptr = (__u64)calloc(drm_mode_get_connector.count_props, sizeof (__u32));
			drm_mode_get_connector.prop_values_ptr = (__u64)calloc(drm_mode_get_connector.count_props, sizeof (__u64));
		}
		if (drm_mode_get_connector.count_modes) {
			drm_mode_get_connector.modes_ptr = (__u64)calloc(drm_mode_get_connector.count_modes, sizeof (struct drm_mode_modeinfo));
		}
		if (drm_mode_get_connector.count_encoders) {
			drm_mode_get_connector.encoders_ptr = (__u64)calloc(drm_mode_get_connector.count_encoders, sizeof (__u32));
		}
		result = ioctl(drm->fd, DRM_IOCTL_MODE_GETCONNECTOR, &drm_mode_get_connector);
		if (result == -1) {
			printf("ioctl failed DRM_IOCTL_MODE_GETCONNECTOR\n");
		}
		struct drm_mode_modeinfo *drm_mode_modeinfo = (struct drm_mode_modeinfo *)drm_mode_get_connector.modes_ptr;
		for (int i = 0; i < drm_mode_get_connector.count_modes; i++) {
			printf("%dx%d\n",  drm_mode_modeinfo[i].hdisplay, drm_mode_modeinfo[i].vdisplay);
		}
		struct drm_mode_get_encoder *drm_mode_get_encoder = (struct drm_mode_get_encoder *)drm_mode_get_connector.encoders_ptr;
		for (int i = 0; i < drm_mode_get_connector.count_encoders; i++) {
			printf(" encoder_id %d\n",  drm_mode_get_encoder[i].encoder_id);
		}
	}
	for (__u32 i = 0; i < drm_mode_card_res.count_crtcs; i++) {
		__u32 crtc_id = ((__u32 *)drm_mode_card_res.crtc_id_ptr)[i];
		struct drm_mode_crtc drm_mode_crtc = {0};
		drm_mode_crtc.crtc_id = crtc_id;
		result = ioctl(drm->fd, DRM_IOCTL_MODE_GETCRTC, &drm_mode_crtc);
		if (result == -1) {
			printf("ioctl failed DRM_IOCTL_MODE_GETCRTC\n");
		}
		printf("drm_mode_crtc %d\n", drm_mode_crtc.fb_id);
		if (drm_mode_crtc.fb_id != 0) {
			struct drm_mode_fb_cmd2 drm_mode_fb_cmd2 = {0};
			drm_mode_fb_cmd2.fb_id = drm_mode_crtc.fb_id;
			result = ioctl(drm->fd, DRM_IOCTL_MODE_GETFB, &drm_mode_fb_cmd2);
			if (result == -1) {
				printf("ioctl failed DRM_IOCTL_MODE_GETFB\n");
			}
			printf("drm_mode_fb_cmd2 %d\n", drm_mode_fb_cmd2.handles[0]);
			// struct drm_mode_create_dumb drm_mode_create_dumb;
			// result = ioctl(drm->fd, DRM_IOCTL_MODE_CREATE_DUMB, &drm_mode_create_dumb);
			// if (result == -1) {
			// 	printf("ioctl failed DRM_IOCTL_MODE_CREATE_DUMB\n");
			// }
			// printf("dumb info %d, %dx%d", drm_mode_create_dumb.bpp, drm_mode_create_dumb.width, drm_mode_create_dumb.height);
			struct drm_mode_map_dumb drm_mode_map_dumb;
			drm_mode_map_dumb.handle = drm_mode_fb_cmd2.handles[0];
			result = ioctl(drm->fd, DRM_IOCTL_MODE_MAP_DUMB, &drm_mode_map_dumb);
			if (result == -1) {
				printf("ioctl failed DRM_IOCTL_MODE_MAP_DUMB\n");
			}
			printf("drm_mode_map_dumb %llu\n", drm_mode_map_dumb.offset);
			uint32_t *fb_ptr = (uint32_t *)(uintptr_t)(drm_mode_map_dumb.offset);
			printf("fb_ptr, %d", fb_ptr[1021]);
			// void *buffer = mmap(NULL, 1920*1080*4, PROT_READ, MAP_SHARED, drm->fd, drm_mode_map_dumb.offset);
			// if (buffer == MAP_FAILED) {
			// 	perror("Failed to mmap buffer");
			// }
			unsigned char *buffer = malloc(1920*1080*4);
			// memcpy(buffer, fb_ptr, 1920*1080*4);
			FILE *file = fopen("../framebuffer-data.bin", "wb");
			// fwrite(buffer, 1, 1920*1080*4, file);
			fclose(file);
			// munmap(buffer, drm_mode_map_dumb.pad);
			break;
		}
	}	
	// for (__u32 i = 0; i < drm_mode_card_res.count_encoders; i++) {
	// 	__u32 encoder_id = ((__u32 *)drm_mode_card_res.encoder_id_ptr)[i];
	// 	struct drm_mode_get_encoder drm_mode_get_encoder = {0};
	// 	drm_mode_get_encoder.encoder_id = encoder_id;
	// 	ioctl(drm->fd, DRM_IOCTL_MODE_GETENCODER, &drm_mode_get_encoder);
	// 	if (result == -1) {
	// 		printf("ioctl failed\n");
	// 	}
	// 	printf("drm_mode_get_encoder %d\n", drm_mode_get_encoder.encoder_id);		
	// }
	return drm;
}

static void
_drm_destroy(void *backend)
{
	Drm *drm = (Drm *)backend;
	free(drm);
}
