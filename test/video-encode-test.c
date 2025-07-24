#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "video-codec.h"

#define WIDTH 1920
#define HEIGHT 1080
#define BPP 4
#define FRAME_COUNT 1080
#define FRAME_RATE 60
#define MJPEG_QUALITY 90

int main(int argc, char *argv[])
{
	struct video_codec_create_config create_config = {
		.backend_type = VIDEO_CODEC_BACKEND_TYPE_GSTREAMER,
		.coding_format = VIDEO_CODEC_CODING_FORMAT_MJPEG,
		.chroma_subsample = VIDEO_CODEC_CHROMA_SUBSAMPLE_YUV444,
		.encoder = true,
		.width = WIDTH,
		.height = HEIGHT,
		.frame_rate = FRAME_RATE,
		.mjpeg.quality = MJPEG_QUALITY
	};
	struct video_codec *video_codec = video_codec_create(&create_config);

#if 0
	// printf("large buf creating\n");
	// for (int i = 0; i < WIDTH * HEIGHT * 32/8 * FRAME_COUNT; i++) {
	// 	buffer[i] = (uint8_t)(rand() % 255);		
	// }
	// printf("large buf created\n");

	FILE *file = fopen("/media/sumit/3BA3312168C31CE4/blender-open-movies/dota2.bin", "r");

	fseek(file, 0L, SEEK_END);

	size_t file_size = ftell(file);

	fseek(file, 0L, SEEK_SET);

	uint8_t *buffer = (uint8_t *)malloc(file_size);

	fwrite(buffer, 1, file_size, file);
	printf("file size %zu\n", file_size);
	fclose(file);
	
	struct timespec time_before;
	struct timespec time_after;
	int fc = FRAME_COUNT;
	uint8_t *p = buffer;

	clock_gettime(CLOCK_MONOTONIC, &time_before);

	// while (fc) {
	// 	// printf("enoded frame %d\n", fc);
	// 	video_codec_encode(video_codec, p);
	// 	// video_codec_encode(video_codec, p);
	// 	p = p + WIDTH * HEIGHT * 4;
	// 	fc--;
	// }

	clock_gettime(CLOCK_MONOTONIC, &time_after);

	unsigned int time_difference = ((time_after.tv_sec * 1000) + (time_after.tv_nsec / 1000000)) -
		((time_before.tv_sec * 1000) + (time_before.tv_nsec / 1000000));

	printf("Time Difference : %d\n", time_difference);
#endif	

	return 0;
}
