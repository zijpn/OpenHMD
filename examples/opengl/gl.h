/*
 * OpenHMD - Free and Open Source API and drivers for immersive technology.
 * Copyright (C) 2013 Fredrik Hultin.
 * Copyright (C) 2013 Jakob Bornecrantz.
 * Distributed under the Boost 1.0 licence, see LICENSE for full text.
 */

/* OpenGL Test - Interface For GL Helper Functions */

#ifndef GL_H
#define GL_H

#include <SDL2/SDL.h>

#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

typedef struct {
	int w, h;
	SDL_Window* window;
	SDL_GLContext context;
	int is_fullscreen;
} gl_ctx;

void ortho(gl_ctx* ctx);
void perspective(gl_ctx* ctx);
void init_gl(gl_ctx* ctx, int w, int h);
void fini_gl(gl_ctx* ctx);
void draw_cube();
GLuint compile_shader(const char* vertex, const char* fragment);
void create_fbo(int eye_width, int eye_height, GLuint* fbo, GLuint* color_tex, GLuint* depth_tex);


#endif
