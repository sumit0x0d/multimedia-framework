#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <video-playback.h>

#define WIDTH 1920
#define HEIGHT 1080
#define FRAME_RATE 100
#define VIDEO_PLAYBACK_BACKEND_TYPE VIDEO_PLAYBACK_BACKEND_TYPE_WAYLAND_CLIENT
#define VIDEO_PLAYBACK_RENDER_TYPE VIDEO_PLAYBACK_RENDER_TYPE_VULKAN

int main(int argc, char *argv[])
{
	// if (argc < 2) {
	// 	fprintf(stderr, "usage : video-player <filename>");
	// 	return EXIT_FAILURE;
	// }
	// FILE *file = fopen(argv[1], "r");
	// assert(file);

	// void *jpeg_buffer = malloc(WIDTH * HEIGHT * 4);
	// assert(jpeg_buffer);

	// FILE *file2 = fopen("../buffer2", "r");
	// assert(file2);

	// void *jpegBuffer2 = malloc(WIDTH * HEIGHT * 4);
	// assert(jpegBuffer2);

	// size_t size1 = fread(jpeg_buffer, 1, WIDTH * HEIGHT * 4, file);
	// size_t size2 = fread(jpegBuffer2, 1, WIDTH * HEIGHT * 4, file2);

	struct video_playback_create_config vpcconfig = {
		.backend_type = VIDEO_PLAYBACK_BACKEND_TYPE_WAYLAND_CLIENT,
		.render_backend_type = VIDEO_PLAYBACK_RENDER_BACKEND_TYPE_VULKAN,
		.width = WIDTH,
		.height = HEIGHT,
		.frame_rate = FRAME_RATE
	};
	struct video_playback *vplayback = video_playback_create(&vpcconfig);
	// video_playback_destroy(video_playback);

	// struct video_playback *video_playback = video_playback_create(WIDTH, HEIGHT, FRAME_RATE);
	video_playback_dispatch(vplayback, NULL);

     printf("*********************\n");

	return EXIT_SUCCESS;
}
