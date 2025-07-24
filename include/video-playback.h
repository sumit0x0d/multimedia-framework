#ifndef VIDEO_PLAYBACK_H
#define VIDEO_PLAYBACK_H

#include <stdint.h>

struct wl_display;
struct wl_surface;

typedef uint32_t xcb_window_t;
struct xcb_connection_t;

struct video_playback;

typedef void * VideoPlaybackBackend;
typedef struct video_playback VideoPlayback;

typedef enum video_playback_backend_type {
     VIDEO_PLAYBACK_BACKEND_TYPE_WAYLAND_CLIENT,
     VIDEO_PLAYBACK_BACKEND_TYPE_XCB,
} VideoPlaybackBackendType;

typedef enum video_playback_render_backend_type {
     VIDEO_PLAYBACK_RENDER_BACKEND_TYPE_OPENGLES,
     VIDEO_PLAYBACK_RENDER_BACKEND_TYPE_VULKAN,
} VideoPlaybackRenderBackendType;

typedef struct video_playback_create_config {
     VideoPlaybackBackendType backend_type;
     VideoPlaybackRenderBackendType render_backend_type;
     int32_t width;
     int32_t height;
     uint32_t frame_rate;
} VideoPlaybackCreateConfig;

typedef struct video_playback_interface {
     VideoPlaybackBackend (*create)(const VideoPlaybackCreateConfig *vpcconfig);
     void (*destroy)(VideoPlaybackBackend backend);
     void (*dispatch)(VideoPlaybackBackend backend, void *buffer);
} VideoPlaybackInterface;

VideoPlayback *video_playback_create(const VideoPlaybackCreateConfig *vpcconfig);
void video_playback_destroy(VideoPlayback *vplayback);
void video_playback_dispatch(VideoPlayback *vplayback, void *buffer);

#endif
