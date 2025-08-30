#ifndef VIDEO_ENCODE_VULKAN_H
#define VIDEO_ENCODE_VULKAN_H

#include <stdint.h>

#define VK_ENABLE_BETA_EXTENSIONS 1

#include <vulkan/vulkan_core.h>

#include <video-encode.h>

struct video_encode_vulkan {
	
};

struct video_encode_vulkan *
video_encode_vulkan_create();

void
video_encode_vulkan_destroy(struct video_encode_vulkan *vulkan);

void
video_encode_vulkan_encode(struct video_encode_vulkan *vulkan);

void
video_encode_vulkan_decode(struct video_encode_vulkan *vulkan);

#endif
