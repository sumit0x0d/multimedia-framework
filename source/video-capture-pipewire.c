#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include <spa/param/audio/format-utils.h>

#include "video-capture-pipewire.h"

typedef struct video_capture_pipewire pipewire_t;

static void
destroy(void *data)
{}

static void
state_changed(void *data, enum pw_stream_state old, enum pw_stream_state state, const char *error)
{}

static void
control_info(void *data, uint32_t id, const struct pw_stream_control *control)
{}

static void
io_changed(void *data, uint32_t id, void *area, uint32_t size)
{}

static void
param_changed(void *data, uint32_t id, const struct spa_pod *param)
{
	struct video_capture_pipewire *pipewire = NULL;
	int result = 0;	
	pipewire = (struct video_capture_pipewire *)data;
	if (param == NULL || id != SPA_PARAM_Format) {
		return;
	}
	result = spa_format_parse(param, &pipewire->format.media_type, &pipewire->format.media_subtype);
	if (result < 0) {
		return;
	}
	if (pipewire->format.media_type != SPA_MEDIA_TYPE_video || pipewire->format.media_subtype != SPA_MEDIA_SUBTYPE_raw) {
		return;
	}
	result = spa_format_video_raw_parse(param, &pipewire->format.info.raw);
	if (result < 0) {
		return;
	}
	printf("got video format:\n");
	printf("  format: %d (%s)\n", pipewire->format.info.raw.format,
		spa_debug_type_find_name(spa_type_video_format,
			pipewire->format.info.raw.format));
	printf("  size: %dx%d\n", pipewire->format.info.raw.size.width,
		pipewire->format.info.raw.size.height);
	printf("  framerate: %d/%d\n", pipewire->format.info.raw.framerate.num,
		pipewire->format.info.raw.framerate.denom);
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
	struct video_capture_pipewire *pipewire = NULL;
	struct pw_buffer *pw_buffer = NULL;
	struct spa_buffer *spa_buffer = NULL;
	pipewire = (struct video_capture_pipewire *)data;
	pw_buffer = pw_stream_dequeue_buffer(pipewire->pw_stream);
	if (pw_buffer == NULL) {
		pw_log_warn("out of buffers: %m");
		return;
	}
	spa_buffer = pw_buffer->buffer;
	if (spa_buffer->datas[0].data == NULL) {
		return;
	}
	printf("got a frame of size %d\n", spa_buffer->datas[0].chunk->size);
	pw_stream_queue_buffer(pipewire->pw_stream, pw_buffer);
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
	.process = process,
	.state_changed = state_changed,
	.param_changed = param_changed
};

extern const struct pw_registry_events pw_registry_events;
extern const struct pw_core_events pw_core_events;

pipewire_t *
video_capture_pipewire_create()
{
	pipewire_t *pipewire = NULL;
	struct pw_loop *pw_loop = NULL;	
	pw_init(NULL, NULL);
	fprintf(stdout, "Compiled with libpipewire %s\n" "Linked with libpipewire %s\n", pw_get_headers_version(),  pw_get_library_version());
	pipewire = (pipewire_t *)malloc(sizeof (pipewire_t));
	assert(pipewire);
	pipewire->pw_main_loop = pw_main_loop_new(NULL);
	pw_loop = pw_main_loop_get_loop(pipewire->pw_main_loop);
	pipewire->pw_context = pw_context_new(pw_loop, NULL, 0);
	pipewire->pw_core = pw_context_connect(pipewire->pw_context, NULL, 0);
	pipewire->pw_registry = pw_core_get_registry(pipewire->pw_core, PW_VERSION_REGISTRY, 0);
	struct spa_hook pw_registry_listener;
	spa_zero(pw_registry_listener);
	pw_registry_add_listener(pipewire->pw_registry, &pw_registry_listener, &pw_registry_events, NULL);
	struct spa_hook pw_core_listener;
	spa_zero(pw_core_listener);
	pw_core_add_listener(pipewire->pw_core, &pw_core_listener, &pw_core_events, pipewire);
	// pipewire->pending = pw_core_sync(pipewire->core, PW_ID_CORE, 0);
	struct pw_properties *pw_stream_properties = pw_properties_new(PW_KEY_MEDIA_TYPE, "Video",
		PW_KEY_MEDIA_CATEGORY, "Capture",
		PW_KEY_MEDIA_ROLE, "Screen",
		NULL);
	// pw_properties_set(pw_stream_properties, PW_KEY_TARGET_OBJECT, argv[1]);
	pipewire->pw_stream = pw_stream_new_simple(pw_loop, "video-capture-pipewire", pw_stream_properties, &pw_stream_events, pipewire);
	const struct spa_pod *params[1];
	uint8_t buffer[1024];
	struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));
	params[0] = spa_pod_builder_add_object(&b,
		SPA_TYPE_OBJECT_Format, SPA_PARAM_EnumFormat,
		SPA_FORMAT_mediaType,       SPA_POD_Id(SPA_MEDIA_TYPE_video),
		SPA_FORMAT_mediaSubtype,    SPA_POD_Id(SPA_MEDIA_SUBTYPE_raw),
		SPA_FORMAT_VIDEO_format,    SPA_POD_CHOICE_ENUM_Id(7,
			SPA_VIDEO_FORMAT_RGB,
			SPA_VIDEO_FORMAT_RGB,
			SPA_VIDEO_FORMAT_RGBA,
			SPA_VIDEO_FORMAT_RGBx,
			SPA_VIDEO_FORMAT_BGRx,
			SPA_VIDEO_FORMAT_YUY2,
			SPA_VIDEO_FORMAT_I420),
		SPA_FORMAT_VIDEO_size,      SPA_POD_CHOICE_RANGE_Rectangle(
									   &SPA_RECTANGLE(320, 240),
									   &SPA_RECTANGLE(1, 1),
									   &SPA_RECTANGLE(4096, 4096)),
		SPA_FORMAT_VIDEO_framerate, SPA_POD_CHOICE_RANGE_Fraction(
									  &SPA_FRACTION(25, 1),
									  &SPA_FRACTION(0, 1),
									  &SPA_FRACTION(1000, 1)));
	pw_stream_connect(pipewire->pw_stream, PW_DIRECTION_INPUT, PW_ID_ANY, PW_STREAM_FLAG_AUTOCONNECT | PW_STREAM_FLAG_MAP_BUFFERS, params,
		1);
	// pw_stream_new(pipewire->pw_core, "OBS",
	// 	pw_properties_new(PW_KEY_NODE_NAME, "OBS", PW_KEY_NODE_DESCRIPTION, "OBS Audio Capture",
	// 		PW_KEY_MEDIA_TYPE, "Audio", PW_KEY_MEDIA_CATEGORY, "Capture", PW_KEY_MEDIA_ROLE, "Production",
	// 		PW_KEY_NODE_WANT_DRIVER, stream_want_driver ? "true" : "false",
	// 		PW_KEY_STREAM_CAPTURE_SINK, stream_capture_sink ? "true" : "false", NULL));
	// const struct spa_pod *params[1];
	// uint8_t buffer[1024];
	// struct spa_pod_builder spa_pod_builder = {
	// 	.data = buffer,
	// 	.size = sizeof(buffer),
	// 	._padding = 0,
	// 	.state = {},
	// 	.callbacks = {}
	// };
	// // SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));
	// struct spa_audio_info_raw spa_audio_info_raw = {
	// 	.format = SPA_AUDIO_FORMAT_S16,
	// 	.flags = 0,
	// 	.channels = DEFAULT_CHANNELS,
	// 	.rate = DEFAULT_RATE
	// };
	// params[0] = spa_format_audio_raw_build(&spa_pod_builder, SPA_PARAM_EnumFormat, &spa_audio_info_raw);
	// pw_stream_connect(pipewire->pw_stream, PW_DIRECTION_OUTPUT, PW_ID_ANY,
	// 	PW_STREAM_FLAG_AUTOCONNECT | PW_STREAM_FLAG_MAP_BUFFERS | PW_STREAM_FLAG_RT_PROCESS, params, 1);
	pw_main_loop_run(pipewire->pw_main_loop);
	// spa_hook_remove(&pipewire->coreListener);
	pw_proxy_destroy((struct pw_proxy *)pipewire->pw_registry);
	pw_core_disconnect(pipewire->pw_core);
	pw_context_destroy(pipewire->pw_context);
	pw_main_loop_destroy(pipewire->pw_main_loop);
	return EXIT_SUCCESS;
}
