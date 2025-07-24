#include "audio-playback.h"

int main()
{
	struct audio_playback_create_config create_config = {
		.backend_type = AUDIO_PLAYBACK_BACKEND_TYPE_PULSEAUDIO,
		.format = 0,
		.sample_rate = 41000,
		.channel_count = 2
	};

	struct audio_playback *audio_playback = audio_playback_create(&create_config);
	// audio_playback_mic(audio_playback);
	return 0;
}
