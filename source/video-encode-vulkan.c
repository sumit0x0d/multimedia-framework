#include <assert.h>
#include <stdlib.h>

#include "video-encode-vulkan.h"

typedef struct video_encode_vulkan video_encode_vulkan_t;

video_encode_vulkan_t *
video_encode_vulkan_create()
{
	video_encode_vulkan_t *vulkan = (video_encode_vulkan_t *)malloc(sizeof (video_encode_vulkan_t));
	assert(vulkan);
	return vulkan;
}

void
video_encode_vulkan_destroy(video_encode_vulkan_t *vulkan)
{
	free(vulkan);
}
