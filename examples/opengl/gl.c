/*
 * OpenHMD - Free and Open Source API and drivers for immersive technology.
 * Copyright (C) 2013 Fredrik Hultin.
 * Copyright (C) 2013 Jakob Bornecrantz.
 * Distributed under the Boost 1.0 licence, see LICENSE for full text.
 */

/* OpenGL Test - GL Helper Functions Implementation */

#include "gl.h"
#include <string.h>
#include <math.h>

#ifdef __unix
#include <signal.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265359
#endif

void init_gl(gl_ctx* ctx, int w, int h)
{
	memset(ctx, 0, sizeof(gl_ctx));

	// Initialize SDL
	int ret = SDL_Init(SDL_INIT_VIDEO);
	if(ret < 0){
		printf("SDL_Init failed\n");
		exit(-1);
	}

	ctx->window = SDL_CreateWindow("openglexample",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		w, h,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_MAXIMIZED);
	if(ctx->window == NULL){
		printf("SDL_CreateWindow failed\n");
		exit(-1);
	}

	// set window bounds into oculus display
	SDL_DisplayMode current;
	int rc = SDL_GetCurrentDisplayMode(0, &current);
	if (rc != 0) {
    	SDL_Log("Could not get display mode for video display #0: %s", SDL_GetError());
		exit(-1);
	}
	SDL_SetWindowPosition(ctx->window, current.w, 0);

	ctx->context = SDL_GL_CreateContext(ctx->window);
	ctx->is_fullscreen = 0;
	ctx->w = w;
	ctx->h = h;

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	rc = SDL_GL_SetSwapInterval(1);
	if (rc != 0) {
    	SDL_Log("SetSwapInterval: %s", SDL_GetError());
		exit(-1);
	}

	// Disable ctrl-c catching on linux (and OS X?)
	#ifdef __unix
	signal(SIGINT, SIG_DFL);
	#endif

	// Load extensions.
	glewInit();

	printf("OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL Vendor: %s\n", glGetString(GL_VENDOR));
	printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

	// == Initialize OpenGL ==
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);
	glDisable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glEnable(GL_POLYGON_SMOOTH);
	glLoadIdentity();

	glViewport(0, 0, w, h);
}

void fini_gl(gl_ctx* ctx)
{
	// Delete OpenGL context
	SDL_GL_DeleteContext(ctx->context);
	// Destroy window
	SDL_DestroyWindow(ctx->window);
	// Shutdown SDL
	SDL_Quit();
}

void ortho(gl_ctx* ctx)
{
	glMatrixMode(GL_PROJECTION);
	//glPushMatrix();
	glLoadIdentity();

	glOrtho(0.0f, ctx->w, ctx->h, 0.0f, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	//glPushMatrix();
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH);

	glDisable(GL_MULTISAMPLE);
}

void draw_cube()
{
	glBegin(GL_QUADS);

	glVertex3f(  0.5f,  0.5f, -0.5f); /* Top Right Of The Quad (Top)      */
	glVertex3f( -0.5f,  0.5f, -0.5f); /* Top Left Of The Quad (Top)       */
	glVertex3f( -0.5f,  0.5f,  0.5f); /* Bottom Left Of The Quad (Top)    */
	glVertex3f(  0.5f,  0.5f,  0.5f); /* Bottom Right Of The Quad (Top)   */

	glVertex3f(  0.5f, -0.5f,  0.5f); /* Top Right Of The Quad (Botm)     */
	glVertex3f( -0.5f, -0.5f,  0.5f); /* Top Left Of The Quad (Botm)      */
	glVertex3f( -0.5f, -0.5f, -0.5f); /* Bottom Left Of The Quad (Botm)   */
	glVertex3f(  0.5f, -0.5f, -0.5f); /* Bottom Right Of The Quad (Botm)  */

	glVertex3f(  0.5f,  0.5f,  0.5f); /* Top Right Of The Quad (Front)    */
	glVertex3f( -0.5f,  0.5f,  0.5f); /* Top Left Of The Quad (Front)     */
	glVertex3f( -0.5f, -0.5f,  0.5f); /* Bottom Left Of The Quad (Front)  */
	glVertex3f(  0.5f, -0.5f,  0.5f); /* Bottom Right Of The Quad (Front) */

	glVertex3f(  0.5f, -0.5f, -0.5f); /* Bottom Left Of The Quad (Back)   */
	glVertex3f( -0.5f, -0.5f, -0.5f); /* Bottom Right Of The Quad (Back)  */
	glVertex3f( -0.5f,  0.5f, -0.5f); /* Top Right Of The Quad (Back)     */
	glVertex3f(  0.5f,  0.5f, -0.5f); /* Top Left Of The Quad (Back)      */

	glVertex3f( -0.5f,  0.5f,  0.5f); /* Top Right Of The Quad (Left)     */
	glVertex3f( -0.5f,  0.5f, -0.5f); /* Top Left Of The Quad (Left)      */
	glVertex3f( -0.5f, -0.5f, -0.5f); /* Bottom Left Of The Quad (Left)   */
	glVertex3f( -0.5f, -0.5f,  0.5f); /* Bottom Right Of The Quad (Left)  */

	glVertex3f(  0.5f,  0.5f, -0.5f); /* Top Right Of The Quad (Right)    */
	glVertex3f(  0.5f,  0.5f,  0.5f); /* Top Left Of The Quad (Right)     */
	glVertex3f(  0.5f, -0.5f,  0.5f); /* Bottom Left Of The Quad (Right)  */
	glVertex3f(  0.5f, -0.5f, -0.5f); /* Bottom Right Of The Quad (Right) */

	glEnd();

}

static void compile_shader_src(GLuint shader, const char* src)
{
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	GLint status;
	GLint length;
	char log[4096] = {0};

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	glGetShaderInfoLog(shader, 4096, &length, log);
	if(status == GL_FALSE){
		printf("compile failed %s\n", log);
	}
}

GLuint compile_shader(const char* vertex, const char* fragment)
{
	// Create the handels
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint programShader = glCreateProgram();

	// Attach the shaders to a program handel.
	glAttachShader(programShader, vertexShader);
	glAttachShader(programShader, fragmentShader);

	// Load and compile the Vertex Shader
	compile_shader_src(vertexShader, vertex);

	// Load and compile the Fragment Shader
	compile_shader_src(fragmentShader, fragment);

	// The shader objects are not needed any more,
	// the programShader is the complete shader to be used.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glLinkProgram(programShader);

	GLint status;
	GLint length;
	char log[4096] = {0};

	glGetProgramiv(programShader, GL_LINK_STATUS, &status);
	glGetProgramInfoLog(programShader, 4096, &length, log);
	if(status == GL_FALSE){
		printf("link failed %s\n", log);
	}

	return programShader;
}

void create_fbo(int eye_width, int eye_height, GLuint* fbo, GLuint* color_tex, GLuint* depth_tex)
{
	glGenTextures(1, color_tex);
	glGenTextures(1, depth_tex);
	glGenFramebuffers(1, fbo);

	glBindTexture(GL_TEXTURE_2D, *color_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, eye_width, eye_height, 0, GL_RGBA, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, *depth_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, eye_width, eye_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, *fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *color_tex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *depth_tex, 0);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

	if(status != GL_FRAMEBUFFER_COMPLETE_EXT){
		printf("failed to create fbo %x\n", status);
	}
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}
