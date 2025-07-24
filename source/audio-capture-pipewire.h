#ifndef AUDIO_CAPTURE_PIPEWIRE_H
#define AUDIO_CAPTURE_PIPEWIRE_H

#include <math.h>

#include <pipewire/pipewire.h>
#include <spa/param/audio/format-utils.h>

#include <audio-capture.h>

struct audio_capture_pipewire {
	int seq;
	unsigned move:1;
	struct pw_main_loop *main_loop;
	struct pw_context *context;
	struct pw_core *core;
	struct spa_hook core_listener;
	struct pw_registry *registry;
	struct spa_hook registry_listener;
	struct pw_stream *stream;
	struct spa_hook stream_listener;
	struct spa_audio_info format;
};

extern struct audio_capture_interface audio_capture_interface_pipewire;

#endif
