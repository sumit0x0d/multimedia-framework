#ifndef AUDIO_PLAYBACK_PULSEAUDIO_H
#define AUDIO_PLAYBACK_PULSEAUDIO_H

#include <audio-playback.h>

#include <pulse/pulseaudio.h>

struct audio_playback_pulseaudio {
	pa_mainloop *pa_mainloop;
	pa_context *pa_context;
	pa_stream *pa_stream;
	pa_operation *pa_operation;
};

extern struct audio_playback_interface audio_playback_interface_pulseaudio;

#endif
