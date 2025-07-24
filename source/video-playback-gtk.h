#ifndef VIDEO_PLAYBACK_GTK_H
#define VIDEO_PLAYBACK_GTK_H

#include <stddef.h>
#include <stdint.h>

struct video_playback_gtk {
};

struct video_playback_gtk *
video_playback_gtk_create(uint8_t *buffer, size_t size);
void
video_playback_gtk_destroy(struct video_playback_gtk *gtk);

void
video_playback_gtk_encode(struct video_playback_gtk *gtk);
void
video_playback_gtk_decode(struct video_playback_gtk *gtk);

#endif
