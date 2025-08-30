#ifndef AUDIO_CAPTURE_PULSEAUDIO_H
#define AUDIO_CAPTURE_PULSEAUDIO_H

#include <audio-capture.h>

#include <pulse/pulseaudio.h>

struct audio_capture_pulseaudio {
	pa_mainloop *pa_mainloop;
	pa_context *pa_context;
	pa_stream *pa_stream;
};

extern struct audio_capture_interface audio_capture_interface_pulseaudio;

#endif
