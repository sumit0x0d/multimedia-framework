#include <assert.h>
#include <stdlib.h>

#include "video-encode-x264.h"

typedef struct video_encode_x264 X264;

X264 *
video_encode_x264_create()
{
	X264 *x264 = (X264 *)malloc(sizeof (X264));
	assert(x264);

	return x264;
}

void
video_encode_x264_destroy(X264 *x264)
{
	free(x264);
}
