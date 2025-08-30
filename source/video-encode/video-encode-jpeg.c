#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "video-encode-jpeg.h"

typedef struct video_encode_jpeg Jpeg;

Jpeg *
video_encode_jpeg_create(bool encoder, int width, int height, int quality)
{
	Jpeg *jpeg = (Jpeg *)malloc(sizeof (Jpeg));
	assert(jpeg);

	return jpeg;
}

void
video_encode_jpeg_destroy(Jpeg *jpeg)
{
	free(jpeg);
}

void
video_encode_jpeg_encode(Jpeg *jpeg, unsigned char *buffer, unsigned char **jbuffer,
     unsigned long *jsize)
{
}
