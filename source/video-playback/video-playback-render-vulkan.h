#ifndef VIDEO_PLAYBACK_VULKAN_H
#define VIDEO_PLAYBACK_VULKAN_H

#include <stdint.h>

// #define VK_USE_PLATFORM_WAYLAND_KHR 1
#define VK_USE_PLATFORM_XCB_KHR 1
#define VK_ENABLE_BETA_EXTENSIONS 1

#include <wayland-client-protocol.h>
#include <xcb/xcb.h>

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_wayland.h>
#include <vulkan/vulkan_xcb.h>

#include "video-playback-render.h"

struct video_playback_render_vulkan {
	VkInstance vk_instance;
	VkSurfaceKHR vk_surface;
	VkSwapchainKHR vk_swapchain;
	VkPhysicalDevice vk_physical_device;
	VkDevice vk_device;
	VkQueue vk_queue;
	VkCommandBuffer vk_command_buffer;
	VkCommandPool vk_command_pool;
	VkImageView vk_image_view;
	VkSemaphore vk_semaphore[2];
	VkDeviceMemory vk_device_memory;
	VkImage vk_image;
	VkFramebuffer vk_framebuffer;
	VkImage *vk_image_swapchain;
	uint32_t queue_family_index;
};

extern struct video_playback_render_interface video_playback_render_interface_vulkan;

#endif
