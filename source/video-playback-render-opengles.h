#ifndef VIDEO_RENDER_OPENGLES_H
#define VIDEO_RENDER_OPENGLES_H

#include "video-playback-render.h"

#include <GLES3/gl32.h>
#include <GLES2/gl2ext.h>

struct video_playback_render_opengles {
	GLuint vertex_buffer;
	GLuint index_buffer;
	GLuint shader_program;
	GLuint texture;	
};

extern struct video_playback_render_interface video_playback_render_interface_opengles;

#endif
