#include <stdio.h>

#include "audio-capture-pulseaudio.h"

typedef struct audio_capture_pulseaudio Pulseaudio;
typedef struct audio_capture_create_config CreateConfig;

static void
_pulseaudio_create(const CreateConfig *cconfig, void *backend);
static void
_pulseaudio_destroy(void *backend);

struct audio_capture_interface audio_capture_interface_pulseaudio = {
    .create = _pulseaudio_create,
    .destroy = _pulseaudio_destroy
};

static void
_pa_stream_success_cb(pa_stream *pstream, int success, void *userdata);
static void
_pa_stream_request_cb(pa_stream *pstream, size_t nbytes, void *userdata);
static void
_pa_stream_notify_cb(pa_stream *pstream, void *userdata);
static void
_pa_stream_event_cb(pa_stream *pstream, const char *name, pa_proplist *pproplist, void *userdata);

static void
_pulseaudio_create(const CreateConfig *cconfig, void *backend)
{
    Pulseaudio *pulseaudio = (Pulseaudio *)backend;
    pulseaudio = (Pulseaudio *)malloc(sizeof (Pulseaudio));
    assert(audio_capture_pulseaudio);
    pa_error_code_t result = PA_OK;
    pulseaudio->pa_mainloop = pa_mainloop_new();
    pa_mainloop_api *pa_mainloop_api = pa_mainloop_get_api(pulseaudio->pa_mainloop);
    pulseaudio->pa_context = pa_context_new(pa_mainloop_api, "audio-capture-pulseaudio");
    const pa_sample_spec pa_sample_spec = {
        // .format = create_config->pa_sample_format,
        .rate = cconfig->sample_rate,
        .channels = cconfig->channel_count
    };
    pa_context_connect(pulseaudio->pa_context, NULL, PA_CONTEXT_NOFLAGS, NULL);
    pulseaudio->pa_stream = pa_stream_new(pulseaudio->pa_context, "audio-capture", &pa_sample_spec, NULL);
    pa_stream_set_state_callback(pulseaudio->pa_stream, _pa_stream_notify_cb, NULL);
    result = pa_stream_connect_record(pulseaudio->pa_stream, NULL, NULL, PA_STREAM_NOFLAGS);
    if(result) {
        printf("FAILED %d\n", result);
    };
    // pa_stream_set_read_callback(pulseaudio->pa_stream, pa_stream_request_cb, pulseaudio);
    pa_usec_t l;
    int negative;
    printf("latency %d\n", pa_stream_get_latency(pulseaudio->pa_stream, &l, &negative));
    pa_mainloop_run(pulseaudio->pa_mainloop, NULL);
    pa_context_unref(pulseaudio->pa_context);
    pa_mainloop_free(pulseaudio->pa_mainloop);
}

static void
_pulseaudio_destroy(void *backend)
{
    Pulseaudio *pulseaudio = (Pulseaudio *)backend;
    free(pulseaudio);
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
    // Process the captured audio data as needed
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
