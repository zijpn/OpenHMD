/*
 * OpenHMD - Free and Open Source API and drivers for immersive technology.
 * Copyright (C) 2013 Fredrik Hultin.
 * Copyright (C) 2013 Jakob Bornecrantz.
 * Distributed under the Boost 1.0 licence, see LICENSE for full text.
 */

/* Simple Test */

#include <openhmd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <termcap.h>

#include "openhmdi.h"

ohmd_context* ctx = NULL;

void clearscreen()
{
#ifdef WINDOWS
	system("cls");
#else
	char buf[1024];
	char* term = getenv("TERM");
	if (term == 0) {
		printf("No terminal specified\n");
		return;
	}
	int rc = tgetent(buf, term);
	if (rc < 0) {
		printf("Could not access termcap database\n");
		return;
	}
	if (rc == 0) {
		printf("Terminal type '%s' is not defined\n", term);
		return;
	}
	char* str = tgetstr("cl", NULL);
	fputs(str, stdout);
#endif
}

void signalHandler(int signum) {
	printf("Interrupt signal (%d) received.\n", signum);
	if (ctx) {
		ohmd_ctx_destroy(ctx);
	}
	exit(signum);
}

// gets float values from the device and prints them
void print_infof(ohmd_device* hmd, const char* name, int len, ohmd_float_value val)
{
	float f[len];
	ohmd_device_getf(hmd, val, f);
	printf("%-25s", name);
	for(int i = 0; i < len; i++)
		printf("%f ", f[i]);
	printf("\n");
}

// gets int values from the device and prints them
void print_infoi(ohmd_device* hmd, const char* name, int len, ohmd_int_value val)
{
	int iv[len];
	ohmd_device_geti(hmd, val, iv);
	printf("%-25s", name);
	for(int i = 0; i < len; i++)
		printf("%d ", iv[i]);
	printf("\n");
}

int main(int argc, char** argv)
{
	// register signal SIGINT and signal handler
	signal(SIGINT, signalHandler);

	ctx = ohmd_ctx_create();

	// Probe for devices
	int num_devices = ohmd_ctx_probe(ctx);
	if(num_devices < 0){
		printf("failed to probe devices: %s\n", ohmd_ctx_get_error(ctx));
		return -1;
	}

	// Print device information
	printf("num devices: %d\n\n", num_devices);
	for(int i = 0; i < num_devices; i++){
		printf("device %d\n", i);
		printf("  vendor:  %s\n", ohmd_list_gets(ctx, i, OHMD_VENDOR));
		printf("  product: %s\n", ohmd_list_gets(ctx, i, OHMD_PRODUCT));
		printf("  path:    %s\n\n", ohmd_list_gets(ctx, i, OHMD_PATH));
	}

	// Open default device (0)
	ohmd_device* hmd = ohmd_list_open_device(ctx, 0);

	if(!hmd){
		printf("failed to open device: %s\n", ohmd_ctx_get_error(ctx));
		return -1;
	}

	// Print hardware information for the opened device
	int ivals[2];
	ohmd_device_geti(hmd, OHMD_SCREEN_HORIZONTAL_RESOLUTION, ivals);
	ohmd_device_geti(hmd, OHMD_SCREEN_VERTICAL_RESOLUTION, ivals + 1);
	printf("resolution:              %i x %i\n", ivals[0], ivals[1]);
	print_infof(hmd, "hsize:",            1, OHMD_SCREEN_HORIZONTAL_SIZE);
	print_infof(hmd, "vsize:",            1, OHMD_SCREEN_VERTICAL_SIZE);
	print_infof(hmd, "lens separation:",  1, OHMD_LENS_HORIZONTAL_SEPARATION);
	print_infof(hmd, "lens vcenter:",     1, OHMD_LENS_VERTICAL_POSITION);
	print_infof(hmd, "left eye fov:",     1, OHMD_LEFT_EYE_FOV);
	print_infof(hmd, "right eye fov:",    1, OHMD_RIGHT_EYE_FOV);
	print_infof(hmd, "left eye aspect:",  1, OHMD_LEFT_EYE_ASPECT_RATIO);
	print_infof(hmd, "right eye aspect:", 1, OHMD_RIGHT_EYE_ASPECT_RATIO);
	print_infof(hmd, "distortion k:",     6, OHMD_DISTORTION_K);

	print_infoi(hmd, "digital button count:", 1, OHMD_BUTTON_COUNT);

	printf("\n");

	// Ask for n rotation quaternions
	while(1){
		ohmd_ctx_update(ctx);

		float zero[] = {.0, .1, .2, 1};
		ohmd_device_setf(hmd, OHMD_ROTATION_QUAT, zero);
		ohmd_device_setf(hmd, OHMD_POSITION_VECTOR, zero);

		clearscreen();
		print_infof(hmd, "rotation quat:", 4, OHMD_ROTATION_QUAT);
		print_infoi(hmd, "button event count:", 1, OHMD_BUTTON_EVENT_COUNT);

		int event_count = 0;

		ohmd_device_geti(hmd, OHMD_BUTTON_EVENT_COUNT, &event_count);

		for(int i = 0; i < event_count; i++){
			int event[2] = {0, 0};
			ohmd_device_geti(hmd, OHMD_BUTTON_POP_EVENT, event);
			printf("button %d: %s", event[0], event[1] == OHMD_BUTTON_DOWN ? "down" : "up");
		}

		ohmd_sleep(.01);
	}

	ohmd_ctx_destroy(ctx);

	return 0;
}
