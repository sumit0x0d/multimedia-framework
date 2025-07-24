#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "audio-capture-pipewire.h"

typedef struct audio_capture_pipewire _pipewire_t;
typedef struct audio_capture_interface _interface_t;
typedef struct audio_capture_create_config _create_config_t;

static void _pipewire_create(const _create_config_t *create_config, void *backend);
static void _pipewire_destroy(void *backend);

struct audio_capture_interface audio_capture_interface_pipewire = {
	.create = _pipewire_create,
	.destroy = _pipewire_destroy
};

extern const struct pw_core_events pw_core_events;
extern const struct pw_stream_events pw_stream_events;

static void _pipewire_core_done(void *object, uint32_t id, int seq);

static void _pipewire_create(const _create_config_t *create_config, void *backend)
{
	_pipewire_t *pipewire = (_pipewire_t *)backend;
	pipewire = (_pipewire_t *)malloc(sizeof (_pipewire_t));
	assert(pipewire);
	pw_init(NULL, NULL);
	pipewire->main_loop = pw_main_loop_new(NULL);
	struct pw_loop *pw_loop = pw_main_loop_get_loop(pipewire->main_loop);
	pipewire->context = pw_context_new(pw_loop, NULL, 0);
	pipewire->core = pw_context_connect(pipewire->context, NULL, 0);
	pipewire->registry = pw_core_get_registry(pipewire->core, PW_VERSION_REGISTRY, 0);
	// pw_registry_add_listener(pipewire->registry, &pipewire->registry_listener, &registry_events, NULL);
	// static struct pw_core_events core_events = {
	// 	PW_VERSION_CORE_EVENTS,
	// 	.done = _pipewire_core_done
	// };
    	// pw_core_add_listener(pipewire->core, &pipewire->core_listener, &core_events, pipewire);
	// pw_stream_add_listener(pipewire->stream, &pipewire->stream_listener, &pw_stream_events, pipewire);
	// pipewire->seq = pw_core_sync(pipewire->core, PW_ID_CORE, 0);
	struct pw_properties *pw_properties = pw_properties_new(PW_KEY_MEDIA_TYPE, "Audio",
		PW_KEY_CONFIG_NAME, "client-rt.conf",
		PW_KEY_MEDIA_CATEGORY, "Capture",
		PW_KEY_MEDIA_ROLE, "Music",
		NULL);
	pipewire->stream = pw_stream_new(pipewire->core, "audio-capture", pw_properties);
        const struct spa_pod *spa_pod[1];
	uint8_t buffer[1024];
        struct spa_pod_builder spa_pod_builder = {
		.data = buffer,
		.size = sizeof (buffer),
		._padding = 0,
		.state = {},
		.callbacks = {}
	};
	struct spa_audio_info_raw spa_audio_info_raw = {
		.format = SPA_AUDIO_FORMAT_F32,
		.flags = 0,
		.rate = 0,
		.channels = 0,
		.position = 0
	};
	spa_pod[0] = spa_format_audio_raw_build(&spa_pod_builder, SPA_PARAM_EnumFormat, &spa_audio_info_raw);
	pw_stream_connect(pipewire->stream, PW_DIRECTION_INPUT, PW_ID_ANY,
		PW_STREAM_FLAG_AUTOCONNECT | PW_STREAM_FLAG_MAP_BUFFERS | PW_STREAM_FLAG_RT_PROCESS, spa_pod, 1);
	pw_main_loop_run(pipewire->main_loop);
}

static void _pipewire_destroy(void *backend)
{
	_pipewire_t *pipewire = (_pipewire_t *)backend;
	// pw_registry_destroy(pipewire->registry);
	// pw_proxy_destroy((struct pw_proxy *)pipewire->registry);
	spa_hook_remove(&pipewire->core_listener);
	pw_core_disconnect(pipewire->core);
	spa_hook_remove(&pipewire->registry_listener);
	pw_context_destroy(pipewire->context);
	pw_main_loop_destroy(pipewire->main_loop);	
	free(pipewire);
}

static void destroy(void *data)
{
}

static void state_changed(void *data, enum pw_stream_state old, enum pw_stream_state state, const char *error)
{
}

static void control_info(void *data, uint32_t id, const struct pw_stream_control *control)
{
}

static void io_changed(void *data, uint32_t id, void *area, uint32_t size)
{
}

static void param_changed(void *data, uint32_t id, const struct spa_pod *param)
{
        _pipewire_t *pipewire = data;
 
        /* NULL means to clear the format */
        if (param == NULL || id != SPA_PARAM_Format)
                return;
 
        if (spa_format_parse(param, &pipewire->format.media_type, &pipewire->format.media_subtype) < 0)
                return;
 
        /* only accept raw audio */
        if (pipewire->format.media_type != SPA_MEDIA_TYPE_audio ||
            pipewire->format.media_subtype != SPA_MEDIA_SUBTYPE_raw)
                return;
 
        /* call a helper function to parse the format for us. */
        spa_format_audio_raw_parse(param, &pipewire->format.info.raw);
 
        fprintf(stdout, "capturing rate:%d channels:%d\n", pipewire->format.info.raw.rate, pipewire->format.info.raw.channels);
}

static void
add_buffer(void *data, struct pw_buffer *buffer)
{}

static void
remove_buffer(void *data, struct pw_buffer *buffer)
{}

static void
process(void *data)
{
        _pipewire_t *pipewire = data;
        struct pw_buffer *b;
        struct spa_buffer *buf;
        float *samples, max;
        uint32_t c, n, n_channels, n_samples, peak;
 
        if ((b = pw_stream_dequeue_buffer(pipewire->stream)) == NULL) {
                pw_log_warn("out of buffers: %m");
                return;
        }
 
        buf = b->buffer;
        if ((samples = buf->datas[0].data) == NULL)
                return;
 
        n_channels = pipewire->format.info.raw.channels;
        n_samples = buf->datas[0].chunk->size / sizeof(float);
 
        /* move cursor up */
        if (pipewire->move)
                fprintf(stdout, "%c[%dA", 0x1b, n_channels + 1);
        fprintf(stdout, "captured %d samples\n", n_samples / n_channels);
        for (c = 0; c < pipewire->format.info.raw.channels; c++) {
                max = 0.0f;
                for (n = c; n < n_samples; n += n_channels)
                        max = fmaxf(max, fabsf(samples[n]));
 
                peak = SPA_CLAMP(max * 30, 0, 39);
 
                fprintf(stdout, "channel %d: |%*s%*s| peak:%f\n",
                                c, peak+1, "*", 40 - peak, "", max);
        }
        pipewire->move = true;
        fflush(stdout);
 
        pw_stream_queue_buffer(pipewire->stream, b);
}

static void
drained(void *data)
{}

static void
command(void *data, const struct spa_command *command)
{}

static void
trigger_done(void *data)
{}

const struct pw_stream_events pw_stream_events = {
	PW_VERSION_STREAM_EVENTS,
	.destroy = destroy,
	.state_changed = state_changed,
	.control_info = control_info,
	.io_changed = io_changed,
	// .param_changed = param_changed,
	.add_buffer = add_buffer,
	.remove_buffer = remove_buffer,
	// .process = process,
	.drained = drained,
	.command = command,
	.trigger_done = trigger_done
};


static void
_pipewire_core_done(void *object, uint32_t id, int seq)
{
	_pipewire_t *pipewire = (_pipewire_t *)object;

	if (id == PW_ID_CORE && seq == pipewire->seq) {
		pw_main_loop_quit(pipewire->main_loop);
	}
}
