#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "video-playback-render-opengles.h"

typedef struct video_playback_render_opengles Opengles;
typedef struct video_playback_create_config CreateConfig;
typedef struct video_playback_render_create_config RenderCreateConfig;

static const char *_vertex_shader_source = "#version 320 es\n"
	"layout (location = 0) in vec4 position;\n"
	"layout (location = 1) in vec2 texCoord;\n"
	"out vec2 vTexCoord;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = position;\n"
	"   	vTexCoord = texCoord;\n"
	"}\0";

static const char *_fragment_shader_source = "#version 320 es\n"
	"precision mediump float;\n"
	"uniform sampler2D uTex;\n"
	"layout (location = 0) out vec4 color;\n"
	"in vec2 vTexCoord;\n"
	"void main()\n"
	"{\n"
	"   	color = texture(uTex, vTexCoord);\n"
	"}\n\0";

static void *
_opengles_create(const CreateConfig *cconfig, const RenderCreateConfig *rcconfig);
static void
_opengles_destroy(void *backend);
static void
_opengles_dispatch(void *backend, void *buffer);

struct video_playback_render_interface video_playback_render_interface_opengles = {
	.create = _opengles_create,	
	.destroy = _opengles_destroy,
	.dispatch = _opengles_dispatch
};

static GLint
_opengles_get_shader(const char* ssource, GLenum stype);

static void *
_opengles_create(const CreateConfig *cconfig, const RenderCreateConfig *rcconfig)
{
	Opengles *opengles = opengles = (Opengles *)malloc(sizeof (Opengles));
	assert(opengles);
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	GLfloat vertices[] = {
		-1.0f, -1.0f, 1.0f, 1.0f,
		+1.0f, -1.0f, 0.0f, 1.0f,
		+1.0f, +1.0f, 0.0f, 0.0f,
		-1.0f, +1.0f, 1.0f, 0.0f
	};
	glGenBuffers(1, &opengles->vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, opengles->vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof (GLfloat), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof (GLfloat), (void *)(2 * sizeof (GLfloat)));
	glEnableVertexAttribArray(1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glGenBuffers(1, &opengles->index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengles->index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	GLint vertex_shader = _opengles_get_shader(_vertex_shader_source, GL_VERTEX_SHADER);
	GLint fragment_shader = _opengles_get_shader(_fragment_shader_source, GL_FRAGMENT_SHADER);
	opengles->shader_program = glCreateProgram();
	glAttachShader(opengles->shader_program, vertex_shader);
	glAttachShader(opengles->shader_program, fragment_shader);
	glLinkProgram(opengles->shader_program);
	glValidateProgram(opengles->shader_program);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glUseProgram(opengles->shader_program);
	glGenTextures(1, &opengles->texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, opengles->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	GLuint uniformTex0 = glGetUniformLocation(opengles->shader_program, "uTex");
	glUniform1i(opengles->texture, 0);
	glEnable(GL_BLEND);
	glViewport(0, 0, cconfig->width, cconfig->height);
	return opengles;
}

static void
_opengles_destroy(void *backend)
{
	Opengles *opengles = (Opengles *)backend;
	glDeleteBuffers(GL_ARRAY_BUFFER, &opengles->vertex_buffer);
	glDeleteProgram(opengles->shader_program);
	free(opengles);
}

static void
_opengles_dispatch(void *backend, void *buffer)
{
	glClearColor(1.0, 1.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	GLint gl_viewport[4];
	glGetIntegerv(GL_VIEWPORT, gl_viewport);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gl_viewport[2], gl_viewport[3], 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, buffer);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
}

static GLint
_opengles_get_shader(const char* ssource, GLenum stype)
{
	GLint shader;
	int result;
	shader = glCreateShader(stype);
	glShaderSource(shader, 1, &ssource, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		char *message;
		message = (char *)malloc(length * sizeof (char));
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		glGetShaderInfoLog(shader, length, &length, message);
		printf("vertex shader failed : %s\n", message);
		free(message);
	}
	return shader;
}
