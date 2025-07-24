#include <stdlib.h>
#include <assert.h>

#include <audio-playback.h>

#include "audio-playback-pulseaudio.h"

typedef struct audio_playback_interface Interface;
typedef struct audio_playback_create_config CreateConfig;

struct audio_playback {
     void *backend;
     Interface interface;
};

typedef struct audio_playback AudioPlayback;

AudioPlayback *audio_playback_create(const CreateConfig *cconfig)
{
     AudioPlayback *audio_playback = (AudioPlayback *)malloc(sizeof (AudioPlayback));
     assert(audio_playback);
     switch (cconfig->backend_type) {
     case AUDIO_PLAYBACK_BACKEND_TYPE_PULSEAUDIO:
         audio_playback->interface = audio_playback_interface_pulseaudio;
         break;
     case AUDIO_PLAYBACK_BACKEND_TYPE_PIPEWIRE:
         // audio_playback->interface = audio_playback_interface_vulkan;
         break;
     }
     audio_playback->backend = audio_playback->interface.create(cconfig);
     return audio_playback;
}

void audio_playback_destroy(AudioPlayback *audio_playback)
{
     audio_playback->interface.destroy(audio_playback->backend);
}
