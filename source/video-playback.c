#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <video-playback.h>

#include "video-playback-render.h"
#include "video-playback-wayland.h"
#include "video-playback-xcb.h"

typedef struct video_playback_create_config VideoPlaybackCreateConfig;
typedef struct video_playback_render_create_config VideoPlaybackRenderCreateConfig;

struct video_playback {
     void *backend;
     void *render_backend;
     struct video_playback_interface interface;
};

typedef struct video_playback VideoPlayback;

VideoPlayback *video_playback_create(const VideoPlaybackCreateConfig *vpcconfig)
{
     VideoPlayback *vplayback = (VideoPlayback *)malloc(sizeof (VideoPlayback));
     assert(video_playback);
     switch (vpcconfig->backend_type) {
     case VIDEO_PLAYBACK_BACKEND_TYPE_WAYLAND_CLIENT:
          vplayback->interface = video_playback_interface_wayland;
          break;
     case VIDEO_PLAYBACK_BACKEND_TYPE_XCB:
          vplayback->interface = video_playback_interface_xcb;
          break;
     }
     vplayback->backend = vplayback->interface.create(vpcconfig);
     VideoPlaybackRenderCreateConfig vprcconfig = {0};
     switch (vpcconfig->backend_type) {
     case VIDEO_PLAYBACK_BACKEND_TYPE_WAYLAND_CLIENT:
          vprcconfig.wl_display =
               ((struct video_playback_wayland *)(vplayback->backend))->wl_display;
          vprcconfig.wl_surface =
               ((struct video_playback_wayland *)(vplayback->backend))->wl_surface;
          break;
     case VIDEO_PLAYBACK_BACKEND_TYPE_XCB:
          vprcconfig.xcb_connnection =
               ((struct video_playback_xcb *)(vplayback->backend))->xcb_connection;
          vprcconfig.xcb_window =
               ((struct video_playback_xcb *)(vplayback->backend))->xcb_window;
          break;
     }
     vplayback->render_backend = video_playback_render_create(vpcconfig, &vprcconfig);
     return vplayback;
}

void video_playback_destroy(VideoPlayback *vplayback)
{
     vplayback->interface.destroy(vplayback->backend);
     video_playback_render_destroy(vplayback->render_backend);
     free(vplayback);
}

void video_playback_dispatch(VideoPlayback *vplayback, void *buffer)
{
     struct wl_display *wdisplay =
          ((struct video_playback_wayland *)(vplayback->backend))->wl_display;
     while (wl_display_dispatch(wdisplay)) {
           // video_playback_opengles_render(video_playback->opengles, buffer, video_playback->width, video_playback->height);
           // eglSwapBuffers(video_playback->egl->display, video_playback->egl->surface);
          video_playback_render_dispatch(vplayback->render_backend, buffer);
     }
}
