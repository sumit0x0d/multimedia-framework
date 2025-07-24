#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "video-playback-render.h"

#include "video-playback-render-opengles.h"
#include "video-playback-render-vulkan.h"


Render *video_playback_render_create(const CreateConfig *cconfig, const RenderCreateConfig *rcconfig)
{
     Render *render = (Render *)malloc(sizeof (Render));
     assert(render);
     switch (cconfig->render_backend_type) {
     case VIDEO_PLAYBACK_RENDER_BACKEND_TYPE_OPENGLES:
          render->interface = video_playback_render_interface_opengles;
          break;
     case VIDEO_PLAYBACK_RENDER_BACKEND_TYPE_VULKAN:
          render->interface = video_playback_render_interface_vulkan;
          break;
     }
     render->backend = render->interface.create(cconfig, rcconfig);
     return render;
}

void
video_playback_render_destroy(Render *render)
{
     render->interface.destroy(render->backend);
     free(render);
}

void
video_playback_render_dispatch(Render *render, void *buffer)
{
     render->interface.dispatch(render->backend, buffer);
}
