#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "video-playback-render-vulkan.h"

typedef struct video_playback_render_vulkan Vulkan;
typedef struct video_playback_create_info VideoPlaybackCreateInfo;
typedef struct video_playback_render_create_info VideoPlaybackRenderCreateInfo;

static void *_vulkan_create(const VideoPlaybackCreateInfo *cinfo, const VideoPlaybackRenderCreateInfo *rcinfo);
static void _vulkan_destroy(void *backend);
static void _vulkan_render(void *backend, void *buffer);

struct video_playback_render_interface video_playback_render_interface_vulkan = {
     .create = _vulkan_create,
     .destroy = _vulkan_destroy,
     .dispatch = _vulkan_render
};

static inline void _vulkan_create_vk_instance(Vulkan *vulkan);
static inline void _vulkan_create_vk_device(Vulkan *vulkan);
static inline void _vulkan_create_vk_image(Vulkan *vulkan, uint32_t width, uint32_t height);
static inline void _vulkan_create_vk_semaphore(Vulkan *vulkan);
static inline void _vulkan_create_vk_wayland_surface(Vulkan *vulkan, struct wl_display *wdisplay, struct wl_surface *wsurface);
static inline void _vulkan_create_vk_xcb_surface(Vulkan *vulkan, xcb_connection_t *xconnection, xcb_window_t xwindow);
static inline void _vulkan_create_vk_command_pool(Vulkan *vulkan);
static inline void _vulkan_create_vk_swapchain(Vulkan *vulkan);

static void *
_vulkan_create(const VideoPlaybackCreateInfo *vpcinfo, const VideoPlaybackRenderCreateInfo *vprcinfo)
{
     Vulkan *vulkan = (Vulkan *)malloc(sizeof (Vulkan));
     assert(vulkan);
     _vulkan_create_vk_instance(vulkan);
     _vulkan_create_vk_device(vulkan);
     vkGetDeviceQueue(vulkan->vk_device, vulkan->queue_family_index, 0, &vulkan->vk_queue);
     switch (vpcinfo->backend_type) {
     case VIDEO_PLAYBACK_BACKEND_TYPE_WAYLAND_CLIENT:
          _vulkan_create_vk_wayland_surface(vulkan, vprcinfo->wl_display, vprcinfo->wl_surface);
          break;
     case VIDEO_PLAYBACK_BACKEND_TYPE_XCB:
          // _vulkan_create_vk_xcb_surface(vulkan, xcb_connection, xcb_window);
          break;
     }
     _vulkan_create_vk_semaphore(vulkan);
     _vulkan_create_vk_command_pool(vulkan);
     _vulkan_create_vk_swapchain(vulkan);
     return vulkan;
}

Vulkan *
_vulkan_create_xcb(xcb_connection_t *xconnection, xcb_window_t xwindow)
{
     Vulkan *vulkan = (Vulkan *)malloc(sizeof (Vulkan));
     assert(vulkan);
     _vulkan_create_vk_instance(vulkan);
     _vulkan_create_vk_device(vulkan);
     vkGetDeviceQueue(vulkan->vk_device, vulkan->queue_family_index, 0, &vulkan->vk_queue);
     _vulkan_create_vk_semaphore(vulkan);
     _vulkan_create_vk_xcb_surface(vulkan, xconnection, xwindow);
     // _vulkan_create_vk_xcb_surface(vulkan, xcb_connection, xcb_window);
     _vulkan_create_vk_swapchain(vulkan);
     return vulkan;
}

void
_vulkan_destroy(void *backend)
{
     Vulkan *vulkan = (Vulkan *)backend;
     vkDestroySwapchainKHR(vulkan->vk_device, vulkan->vk_swapchain, NULL);
     vkDestroySurfaceKHR(vulkan->vk_instance, vulkan->vk_surface, NULL);
     vkDestroyDevice(vulkan->vk_device, NULL);
     vkDestroyInstance(vulkan->vk_instance, NULL);
}

static void
_vulkan_render(void *backend, void *buffer)
{
     Vulkan *vulkan = (Vulkan *)backend;
     uint32_t iindex = 0;
     VkResult result = vkAcquireNextImageKHR(vulkan->vk_device, vulkan->vk_swapchain, 0, vulkan->vk_semaphore[0], 0, &iindex);
     // if (result) {
     //      _vulkan_create_vk_swapchain(vulkan);
     //      uint32_t sicount = 0;
     //      result = vkGetSwapchainImagesKHR(vulkan->vk_device, vulkan->vk_swapchain,
     //           &sicount, NULL);
     //      assert(result == 0);
     //      vulkan->vk_image_swapchain = (VkImage *)malloc(sicount * sizeof (VkImage));
     //      assert(vulkan->vk_swapchain_images);
     //      result = vkGetSwapchainImagesKHR(vulkan->vk_device, vulkan->vk_swapchain,
     //           &sicount, vulkan->vk_image_swapchain);
     //      assert(result == 0);
     // }
     VkCommandBufferAllocateInfo vcbainfo = {
          .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
          .pNext = NULL,
          .commandPool = vulkan->vk_command_pool,
          .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
          .commandBufferCount = 1
     };
     result = vkAllocateCommandBuffers(vulkan->vk_device, &vcbainfo, &vulkan->vk_command_buffer);
     assert(result == 0);
     VkCommandBufferBeginInfo vcbbinfo = {
          .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
          .pNext = NULL,
          .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
          .pInheritanceInfo = NULL
     };
     result = vkBeginCommandBuffer(vulkan->vk_command_buffer, &vcbbinfo);
     assert(result == 0);
     VkClearColorValue color = {
          .float32 = {1,0,0,1},
     };
     VkImageSubresourceRange range = {
          .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
          .layerCount = 1,
          .levelCount = 1
     };
     // VkDeviceMemory deviceMemory;
     // vkBindImageMemory(vk_device, vulkan->vk_swapchain_images[image_index],
     //      deviceMemory, 12) ;
     vkCmdClearColorImage(vulkan->vk_command_buffer, vulkan->vk_image_swapchain[iindex], VK_IMAGE_LAYOUT_GENERAL, &color, 1, &range);
     result = vkEndCommandBuffer(vulkan->vk_command_buffer);
     VkPipelineStageFlags wdsmask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
     VkSubmitInfo vsinfo = {
          .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
          .pNext = NULL,
          .waitSemaphoreCount = 1,
          .pWaitSemaphores = &vulkan->vk_semaphore[0],
          .pWaitDstStageMask = &wdsmask,
          .commandBufferCount = 1,
          .pCommandBuffers = &vulkan->vk_command_buffer,
          .signalSemaphoreCount = 1,
          .pSignalSemaphores = &vulkan->vk_semaphore[1]
     };
     result = vkQueueSubmit(vulkan->vk_queue, 1, &vsinfo, 0);
     VkPresentInfoKHR vpinfo = {
          .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
          .pNext = NULL,
          .waitSemaphoreCount = 1,
          .pWaitSemaphores = &vulkan->vk_semaphore[1],
          .swapchainCount = 1,
          .pSwapchains = &vulkan->vk_swapchain,
          .pImageIndices = &iindex,
          .pResults = NULL
     };
     result = vkQueuePresentKHR(vulkan->vk_queue, &vpinfo);
     result = vkDeviceWaitIdle(vulkan->vk_device);
     vkFreeCommandBuffers(vulkan->vk_device, vulkan->vk_command_pool, 1, &vulkan->vk_command_buffer);
}

static inline void
_vulkan_enumerate_instance()
{
     VkResult result = 0;
     uint32_t pcount = 0;
     result = vkEnumerateInstanceLayerProperties(&pcount, NULL);
     VkLayerProperties *vlproperties = (VkLayerProperties *)malloc(pcount * sizeof (VkLayerProperties));
     result = vkEnumerateInstanceLayerProperties(&pcount, vlproperties);
     assert(result == 0);
     for (uint32_t i = 0; i < pcount; i++) {
          result = vkEnumerateInstanceExtensionProperties(vlproperties[i].layerName, &pcount, NULL);
          VkExtensionProperties *veproperties = (VkExtensionProperties *)malloc(pcount * sizeof (VkExtensionProperties));
          result = vkEnumerateInstanceExtensionProperties(vlproperties[i].layerName, &pcount, veproperties);
          assert(result == 0);
     } 
}

static inline void
_vulkan_create_vk_instance(Vulkan *vulkan)
{
     VkApplicationInfo vainfo = {
          .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
          .pNext = NULL,
          .pApplicationName = "video-render",
          .applicationVersion = 1,
          .pEngineName = NULL,
          .engineVersion = 0,
          .apiVersion = 0
     };
     const char *elnames[] = {
          "VK_LAYER_MESA_device_select",
          "VK_LAYER_KHRONOS_validation"
     };
     const char *eenames[] = {
          VK_KHR_SURFACE_EXTENSION_NAME,
          VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME
     };
     VkInstancevideoPlaybackCreateInfo vicinfo = {
          .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
          .pNext = NULL,
          .flags = 0,
          .pApplicationInfo = &vainfo,
          .enabledLayerCount = 2,
          .ppEnabledLayerNames = elnames,
          .enabledExtensionCount = 2,
          .ppEnabledExtensionNames = eenames,
     };
     VkResult result = vkCreateInstance(&vicinfo, NULL, &vulkan->vk_instance);
     assert(result == 0);
}

static inline void
_vulkan_enumerate_physical_devices(Vulkan *vulkan)
{
     uint32_t pdcount = 0;
     VkResult result = vkEnumeratePhysicalDevices(vulkan->vk_instance, &pdcount, NULL);
     assert(result == 0);
     VkPhysicalDevice *vpdevice = (VkPhysicalDevice *)malloc(
          pdcount * sizeof (VkPhysicalDevice));
     assert(vk_physical_devices);
     result = vkEnumeratePhysicalDevices(vulkan->vk_instance, &pdcount, vpdevice);
     assert(result == 0);
     for (uint32_t i = 0; i < pdcount; i++) {
          uint32_t pcount = 0;
          vkEnumerateDeviceExtensionProperties(vpdevice[i], NULL, &pcount, NULL);
          VkExtensionProperties* veproperties = (VkExtensionProperties *)malloc(
               pcount * sizeof (VkExtensionProperties));
          vkEnumerateDeviceExtensionProperties(vpdevice[i], NULL, &pcount, veproperties);
          for (uint32_t i = 0; i < pcount; i++) {
               printf("property_ %i =  %s\n", i, veproperties[i].extensionName);
          }
          VkPhysicalDeviceProperties vpdproperties = {0};
          vkGetPhysicalDeviceProperties(vpdevice[i], &vpdproperties);
          if (vpdproperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
               vulkan->vk_physical_device = vpdevice[i];
               vulkan->queue_family_index = i;
               break;
          }
     }
     free(vpdevice);
}

static inline void
_vulkan_create_vk_device(Vulkan *vulkan)
{
     _vulkan_enumerate_physical_devices(vulkan);
     float qpriorities = 1.0f;
     VkDeviceQueuevideoPlaybackCreateInfo vdqcinfo = {
          .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
          .pNext = NULL,
          .flags = 0,
          .queueFamilyIndex = vulkan->queue_family_index,
          .queueCount = 1,
          .pQueuePriorities = &qpriorities
     };
     const char *eenames[] = {
          VK_KHR_SWAPCHAIN_EXTENSION_NAME
     };
     VkDevicevideoPlaybackCreateInfo vdcinfo = {
          .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
          .pNext = NULL,
          .flags = 0,
          .QueuevideoPlaybackCreateInfoCount = 1,
          .pQueuevideoPlaybackCreateInfos = &vdqcinfo,
          .enabledLayerCount = 0,
          .ppEnabledLayerNames = NULL,
          .enabledExtensionCount = 1,
          .ppEnabledExtensionNames = eenames,
          .pEnabledFeatures = NULL
     };
     VkResult result = vkCreateDevice(vulkan->vk_physical_device, &vdcinfo, NULL,
          &vulkan->vk_device);
     assert(result == 0);
}

static inline void
_vulkan_create_vk_command_pool(Vulkan *vulkan)
{
     VkCommandPoolvideoPlaybackCreateInfo vcpcinfo = {
          .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
          .pNext = NULL,
          .flags = 0,
          .queueFamilyIndex = vulkan->queue_family_index
     };
     VkResult result = vkCreateCommandPool(vulkan->vk_device, &vcpcinfo,
          NULL, &vulkan->vk_command_pool);
     assert(result == 0);
}

static inline void
_vulkan_create_vk_wayland_surface(Vulkan *vulkan, struct wl_display *wdisplay,
     struct wl_surface *wsurface)
{
     VkWaylandSurfacevideoPlaybackCreateInfoKHR vwscinfo = {
          .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
          .pNext = NULL,
          .flags = 0,
          .display = wdisplay,
          .surface = wsurface
     };
     VkResult result = vkCreateWaylandSurfaceKHR(vulkan->vk_instance,
          &vwscinfo, NULL, &vulkan->vk_surface);
     assert(result == 0);
     VkBool32 psupport = vkGetPhysicalDeviceWaylandPresentationSupportKHR(
          vulkan->vk_physical_device, vulkan->queue_family_index, wdisplay);
     assert(result == 0);
     printf("presentation_support %d\n", psupport);
}

static inline void
_vulkan_create_vk_xcb_surface(Vulkan *vulkan, xcb_connection_t *xconnection,
     xcb_window_t xwindow)
{
     VkXcbSurfacevideoPlaybackCreateInfoKHR vxscinfo = {
          .sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
          .pNext = NULL,
          .flags = 0,
          .connection = xconnection,
          .window = xwindow
     };
     VkResult result = vkCreateXcbSurfaceKHR(vulkan->vk_instance, &vxscinfo, NULL,
          &vulkan->vk_surface);
     assert(result == 0);
}

static inline void
_vulkan_create_vk_image(Vulkan *vulkan, uint32_t width, uint32_t height)
{
     VkImagevideoPlaybackCreateInfo vicinfo = {
          .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
          .imageType = VK_IMAGE_TYPE_2D,
          .extent = {
               .width = width,
               .height = height,
               .depth = 1,
          },
          .mipLevels = 1,
          .arrayLayers = 1,
          .format = VK_FORMAT_R8G8B8A8_SRGB,
          .tiling = VK_IMAGE_TILING_OPTIMAL,
          .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
          .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
          .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
          .samples = VK_SAMPLE_COUNT_1_BIT,
     };
     VkResult result = vkCreateImage(vulkan->vk_device, &vicinfo, NULL,
          &vulkan->vk_image);
     assert(result == 0);
}

static inline void
_vulkan_create_vk_image_view(Vulkan *vulkan, VkFormat format)
{
     VkImageViewvideoPlaybackCreateInfo vivcinfo = {
          .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
          .pNext = NULL,
          .flags = 0,
          .image = vulkan->vk_image,
          .viewType = VK_IMAGE_VIEW_TYPE_2D,
          .format = VK_FORMAT_R8G8B8A8_SRGB,
          .components = {
               .a = 0,
               .b = 9,
               .g = 10,
               .r = 0
          },
          .subresourceRange = {
               .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
               .baseMipLevel = 1,
               .levelCount = 0,
               .baseArrayLayer = 1,
               .layerCount = 0
          }
     };
     VkResult result = vkCreateImageView(vulkan->vk_device, &vivcinfo, NULL,
          &vulkan->vk_image_view);
     assert(result == 0);
}

static inline void
_vulkan_create_vk_semaphore(Vulkan *vulkan)
{
     VkSemaphorevideoPlaybackCreateInfo vscinfo = {
          .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
          .pNext = NULL,
          .flags = 0
     };
     VkResult result = vkCreateSemaphore(vulkan->vk_device, &vscinfo, NULL,
          &vulkan->vk_semaphore[0]);
     assert(result == 0);
     result = vkCreateSemaphore(vulkan->vk_device, &vscinfo, NULL,
          &vulkan->vk_semaphore[1]);
     assert(result == 0);
}

static inline void
_vulkan_create_vk_swapchain(Vulkan *vulkan)
{
     VkSurfaceCapabilitiesKHR vscapabilities = {0};
     VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
          vulkan->vk_physical_device, vulkan->vk_surface, &vscapabilities);
     assert(result == 0);
     uint32_t sfcount = 0;
     result = vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan->vk_physical_device,
          vulkan->vk_surface, &sfcount, NULL);
     assert(result == 0);
     VkSurfaceFormatKHR *vsformat = (VkSurfaceFormatKHR *)malloc(
          sfcount * sizeof (VkSurfaceFormatKHR));
     assert(vsformat);
     result = vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan->vk_physical_device,
          vulkan->vk_surface, &sfcount, vsformat);
     assert(result == 0);
     uint32_t pmcount = 0;
     result = vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan->vk_physical_device,
          vulkan->vk_surface, &pmcount, NULL);
     assert(result == 0);
     VkPresentModeKHR *vpmode = (VkPresentModeKHR *)malloc(
          pmcount * sizeof (VkPresentModeKHR));
     assert(present_mode);
     result = vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan->vk_physical_device,
          vulkan->vk_surface, &pmcount, vpmode);
     assert(result == 0);
     VkSwapchainvideoPlaybackCreateInfoKHR vscinfo = {
          .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
          .pNext = NULL,
          .flags = 0,
          .surface = vulkan->vk_surface,
          .minImageCount = vscapabilities.minImageCount,
          .imageFormat = VK_FORMAT_B8G8R8A8_SRGB,
          .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
          .imageExtent = {
               .width = 1920,
               .height = 1080
          },
          .imageArrayLayers = vscapabilities.maxImageArrayLayers,
          .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
          // .imageSharingMode = ,
          // .queueFamilyIndexCount = ,
          // .pQueueFamilyIndices = ,
          .preTransform = vscapabilities.currentTransform,
          .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
          .presentMode = VK_PRESENT_MODE_FIFO_KHR,
          // .clipped = ,
          // .oldSwapchain =
     };
     result = vkCreateSwapchainKHR(vulkan->vk_device, &vscinfo, NULL,
          &vulkan->vk_swapchain);
     assert(result == 0);
     uint32_t sicount = 0;
     result = vkGetSwapchainImagesKHR(vulkan->vk_device, vulkan->vk_swapchain,
          &sicount, NULL);
     assert(result == 0);
     vulkan->vk_image_swapchain = (VkImage *)
          malloc(sicount * sizeof (VkImage));
     result = vkGetSwapchainImagesKHR(vulkan->vk_device, vulkan->vk_swapchain,
          &sicount, vulkan->vk_image_swapchain);
     assert(result == 0);
     for (uint32_t i = 0; i < sicount; i++) {
          // vulkan->vk_swapchain_image
     } 
     // free(present_mode);
     // free(surface_format);
     // free(swapchain_image);
}
