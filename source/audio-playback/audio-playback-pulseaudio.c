#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include "audio-playback-pulseaudio.h"

#define MODULE_NAME     "module-null-sink"
#define MODULE_ARGUMENT "sink_name=audio-playback-pulseaudio "		\
	"sink_properties=device.description=audio-playback-pulseaudio"
#define SINK_NAME_PREFIX ""


typedef struct audio_playback_pulseaudio    Pulseaudio;
typedef struct audio_playback_create_config CreateConfig;

static void *
_pulseaudio_create(const CreateConfig *create_config);
static void
_pulseaudio_destroy(void *backend);

struct audio_playback_interface audio_playback_interface_pulseaudio = {
	.create  = _pulseaudio_create,
	.destroy = _pulseaudio_destroy
};

static void
_pa_context_notify_cb(pa_context *c, void *userdata);
static void
_pa_context_index_cb(pa_context *c, uint32_t idx, void *userdata);
static void
_pa_context_success_cb(pa_context *c, int success, void *userdata);

static void
_pa_operation_notify_cb(pa_operation *o, void *userdata);

static void
_pa_module_info_cb(pa_context *c, const pa_module_info* module_info, int eol, void *userdata);

static void
_pa_source_info_cb(pa_context *c, const pa_source_info *source_info, int eol, void *userdata);

static void
_pa_sink_info_cb(pa_context *c, const pa_sink_info *sink_info, int eol, void *userdata);

static void
_pa_stream_success_cb(pa_stream *pa_stream, int success, void *userdata);
static void
_pa_stream_request_cb(pa_stream *pa_stream, size_t nbytes, void *userdata);
static void
_pa_stream_notify_cb(pa_stream *pa_stream, void *userdata);
static void
_pa_stream_event_cb(pa_stream *pa_stream, const char *name, pa_proplist *pa_proplist, void *userdata);

static void *
_pulseaudio_create(const CreateConfig *cconfig)
{
	Pulseaudio *pulseaudio = (Pulseaudio *)malloc(sizeof (Pulseaudio));
	assert(audio_playback_pulseaudio);
	pa_error_code_t result = PA_OK;
	pulseaudio->pa_mainloop = pa_mainloop_new();
	pa_mainloop_api *pmapi = pa_mainloop_get_api(pulseaudio->pa_mainloop);
	pulseaudio->pa_context = pa_context_new(pmapi, "audio-playback-pulseaudio");
	const pa_sample_spec pa_sample_spec = {
		.format = cconfig->format,
		.rate = cconfig->sample_rate,
		.channels = cconfig->channel_count
	};

	pa_context_connect(pulseaudio->pa_context, NULL, PA_CONTEXT_NOFLAGS, NULL);
	pa_context_state_t pcstate;
	pa_context_set_state_callback(pulseaudio->pa_context, _pa_context_notify_cb, &pcstate);
	while (pcstate < PA_CONTEXT_READY) {
		pa_mainloop_iterate(pulseaudio->pa_mainloop, 1, NULL);
	}
	if (pcstate > PA_CONTEXT_READY) {
		return NULL;
	}

	pa_sink_info psinfo;
	pulseaudio->pa_operation = pa_context_get_sink_info_list(pulseaudio->pa_context, _pa_sink_info_cb, &psinfo);
	while (pa_operation_get_state(pulseaudio->pa_operation) != PA_OPERATION_DONE) {
		pa_mainloop_iterate(pulseaudio->pa_mainloop, 1, NULL);
	}

#if 0
	pulseaudio->pa_operation = pa_context_load_module(pulseaudio->pa_context, MODULE_NAME, MODULE_ARGUMENT, _pa_context_index_cb, NULL);
	while (pa_operation_get_state(pulseaudio->pa_operation) != PA_OPERATION_DONE) {
		pa_mainloop_iterate(pulseaudio->pa_mainloop, 1, NULL);
	}

	pulseaudio->pa_operation = pa_context_unload_module(pulseaudio->pa_context, -1, _pa_context_success_cb, NULL);
	while (pa_operation_get_state(pulseaudio->pa_operation) != PA_OPERATION_DONE) {
		pa_mainloop_iterate(pulseaudio->pa_mainloop, 1, NULL);
	}
#endif
	
	// pulseaudio->pa_operation = pa_context_get_module_info_list(pulseaudio->pa_context, _pa_module_info_cb, NULL);
	// while (pa_operation_get_state(pulseaudio->pa_operation) != PA_OPERATION_DONE) {
	// 	pa_mainloop_iterate(pulseaudio->pa_mainloop, 1, NULL);
	// }

	// /** Unload a module. */

	// pulseaudio->pa_stream = pa_stream_new(pulseaudio->pa_context, "audio-playback", &pa_sample_spec, NULL);
	// pa_stream_set_state_callback(pulseaudio->pa_stream, _pa_stream_notify_cb, NULL);

	// result = pa_stream_connect_record(pulseaudio->pa_stream, NULL, NULL, PA_STREAM_NOFLAGS);
	// if(result) {
	// 	printf("FAILED %d\n", result);
	// };
	// pa_stream_set_read_callback(pulseaudio->pa_stream, pa_stream_request_cb, pulseaudio);
	// pa_usec_t l;
	// int negative;
	// printf("latency %d\n", pa_stream_get_latency(pulseaudio->pa_stream, &l, &negative));
	// pa_mainloop_run(pulseaudio->pa_mainloop, NULL);
	// pa_context_unref(pulseaudio->pa_context);
	// pa_mainloop_free(pulseaudio->pa_mainloop);
	return pulseaudio;
}

static void
_pulseaudio_destroy(void *backend)
{
	Pulseaudio *pulseaudio = (Pulseaudio *)backend;
	free(pulseaudio);
}

static void
_pa_context_notify_cb(pa_context *context, void *userdata)
{
	pa_context_state_t state = pa_context_get_state(context);
	switch (state) {
	case PA_CONTEXT_UNCONNECTED:
	case PA_CONTEXT_CONNECTING:
	case PA_CONTEXT_AUTHORIZING:
	case PA_CONTEXT_SETTING_NAME:
		break;
	case PA_CONTEXT_READY:
		*(pa_context_state_t *)userdata = state;
		break;
	case PA_CONTEXT_FAILED:
	case PA_CONTEXT_TERMINATED:
		fprintf(stderr, "pa_context failed %d\n", state);
		break;
	}
}

static void
_pa_context_index_cb(pa_context *c, uint32_t idx, void *userdata)
{
	printf("*********** %d ************\n", idx);
}

static void
_pa_context_success_cb(pa_context *c, int success, void *userdata)
{
	printf("*********** %d ************\n", success);
}

static void
_pa_operation_notify_cb(pa_operation *o, void *userdata)
{

}

static void
_pa_module_info_cb(pa_context *c, const pa_module_info *module_info, int eol, void *userdata)
{
	if (eol > 0) {
		return;
	}
	printf("pa_module_info_cb name %s\n", module_info->name);	
}

static void
_pa_sink_info_cb(pa_context *c, const pa_sink_info *sink_info, int eol, void *userdata)
{
	if (eol > 0) {
		return;
	}
	for (uint32_t i = 0; i < sink_info->n_ports; i++) {
		if (strstr(sink_info->name, SINK_NAME_PREFIX)) {
			
		}
	}
	printf("pa_sink_info_cb active_port %s\n", sink_info->active_port->name);
}

static void
_pa_stream_success_cb(pa_stream *s, int success, void *userdata)
{
	Pulseaudio *p = userdata;
	if (success < 0) {
		fprintf(stderr, "Stream connection failed: %s\n", pa_strerror(success));
		// pa_mainloop_quit(p, 1);
		return;
	}
	fprintf(stderr, "Stream successfully connected.\n");
}

static void
_pa_stream_request_cb(pa_stream *p, size_t nbytes, void *userdata)
{
	Pulseaudio *pulseaudio = (Pulseaudio *)userdata;
	uint8_t buf[1024];
	pa_stream_peek(pulseaudio->pa_stream, (const void**)&buf, &nbytes);
	// Process the playbackd audio data as needed
	// For simplicity, this example just writes it to stdout
	fwrite(buf, 1, nbytes, stdout);
	printf("writing");
	pa_stream_drop(pulseaudio->pa_stream);
}

static void
_pa_stream_notify_cb(pa_stream *p, void *userdata)
{
	Pulseaudio *pulseaudio = (Pulseaudio *)userdata;
	pa_stream_state_t pa_stream_state = pa_stream_get_state(pulseaudio->pa_stream);
	switch (pa_stream_state) {
	case PA_STREAM_CREATING:
	case PA_STREAM_TERMINATED:
		break;
	case PA_STREAM_READY:
		pa_stream_set_read_callback(pulseaudio->pa_stream, _pa_stream_request_cb, userdata);
		pa_stream_set_state_callback(pulseaudio->pa_stream, _pa_stream_notify_cb, userdata);
		break;
	case PA_STREAM_FAILED:
	default:
		fprintf(stderr, "Stream error: %s\n", pa_strerror(pa_context_errno(pa_stream_get_context(pulseaudio->pa_stream))));
		// pa_mainloop_signal(pulseaudio->pa_mainloop, 0);
		break;
	}
}

static void
_pa_stream_event_cb(pa_stream *p, const char *name, pa_proplist *pl, void *userdata)
{
}
