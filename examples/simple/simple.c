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
char* buf[512];
char* hmdinfo = (char*)buf;

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
	printf("Interrupt signal (%d) received\n", signum);
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
	printf("%-27s", name);
	for(int i = 0; i < len; i++)
		printf("%f ", f[i]);
	printf("\n");
}

// gets int values from the device and prints them
void print_infoi(ohmd_device* hmd, const char* name, int len, ohmd_int_value val)
{
	int iv[len];
	ohmd_device_geti(hmd, val, iv);
	printf("%-27s", name);
	for(int i = 0; i < len; i++)
		printf("%d ", iv[i]);
	printf("\n");
}

void get_hmd_info(ohmd_device* hmd, int idx, int num_devices)
{
	int ivals[2];
	float fvals[6];
	hmdinfo += sprintf(hmdinfo, "#devices: %d\n", num_devices);
	for(int i = 0; i < num_devices; i++){
		hmdinfo += sprintf(hmdinfo, "  device %d: %s, %s, %s\n", i,
			ohmd_list_gets(ctx, i, OHMD_VENDOR),
			ohmd_list_gets(ctx, i, OHMD_PRODUCT),
			ohmd_list_gets(ctx, i, OHMD_PATH));
	}
	hmdinfo += sprintf(hmdinfo, "\n");
	hmdinfo += sprintf(hmdinfo, "device %d info\n", idx);
	ohmd_device_geti(hmd, OHMD_SCREEN_HORIZONTAL_RESOLUTION, ivals);
	ohmd_device_geti(hmd, OHMD_SCREEN_VERTICAL_RESOLUTION, ivals + 1);
	hmdinfo += sprintf(hmdinfo, "  resolution:              %i x %i\n", ivals[0], ivals[1]);
	ohmd_device_getf(hmd, OHMD_SCREEN_HORIZONTAL_SIZE, fvals);
	hmdinfo += sprintf(hmdinfo, "  hsize:                   %f\n", fvals[0]);
	ohmd_device_getf(hmd, OHMD_SCREEN_VERTICAL_SIZE, fvals);
	hmdinfo += sprintf(hmdinfo, "  vsize:                   %f\n", fvals[0]);
	ohmd_device_getf(hmd, OHMD_LENS_HORIZONTAL_SEPARATION, fvals);
	hmdinfo += sprintf(hmdinfo, "  lens separation:         %f\n", fvals[0]);
	ohmd_device_getf(hmd, OHMD_LENS_VERTICAL_POSITION, fvals);
	hmdinfo += sprintf(hmdinfo, "  lens vcenter:            %f\n", fvals[0]);
	ohmd_device_getf(hmd, OHMD_LEFT_EYE_FOV, fvals);
	hmdinfo += sprintf(hmdinfo, "  left eye fov:            %f\n", fvals[0]);
	ohmd_device_getf(hmd, OHMD_RIGHT_EYE_FOV, fvals);
	hmdinfo += sprintf(hmdinfo, "  right eye fov:           %f\n", fvals[0]);
	ohmd_device_getf(hmd, OHMD_LEFT_EYE_ASPECT_RATIO, fvals);
	hmdinfo += sprintf(hmdinfo, "  left eye aspect:         %f\n", fvals[0]);
	ohmd_device_getf(hmd, OHMD_RIGHT_EYE_ASPECT_RATIO, fvals);
	hmdinfo += sprintf(hmdinfo, "  right eye aspect:        %f\n", fvals[0]);
	ohmd_device_getf(hmd, OHMD_DISTORTION_K, fvals);
	hmdinfo += sprintf(hmdinfo, "  distortion k:            %f %f %f %f %f %f\n", fvals[0], fvals[1], fvals[2], fvals[3], fvals[4], fvals[5]);
	ohmd_device_geti(hmd, OHMD_BUTTON_COUNT, ivals);
	hmdinfo += sprintf(hmdinfo, "  digital button count:    %d\n", ivals[0]);
	// reset char pointer
	hmdinfo = (char*)buf;
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

	// Open default device (0)
	int idx = 0;
	ohmd_device* hmd = ohmd_list_open_device(ctx, idx);
	if(!hmd){
		printf("failed to open device %d: %s\n", idx, ohmd_ctx_get_error(ctx));
		return -1;
	}

	// Print hardware information for the opened device
	get_hmd_info(hmd, idx, num_devices);
	int buttons = 0;
	ohmd_device_geti(hmd, OHMD_BUTTON_COUNT, &buttons);
	printf("%s\n", hmdinfo);

	// Ask for rotation quaternions
	while(1){
		ohmd_ctx_update(ctx);

		float zero[] = {.0, .1, .2, 1};
		ohmd_device_setf(hmd, OHMD_ROTATION_QUAT, zero);
		ohmd_device_setf(hmd, OHMD_POSITION_VECTOR, zero);

		clearscreen();
		printf("%s\n", hmdinfo);
		print_infof(hmd, "  rotation quat:", 4, OHMD_ROTATION_QUAT);
		print_infoi(hmd, "  button event count:", 1, OHMD_BUTTON_EVENT_COUNT);
		int event_count = 0;
		ohmd_device_geti(hmd, OHMD_BUTTON_EVENT_COUNT, &event_count);
		for(int i = 0; i < event_count; i++){
			int event[2] = {0, 0};
			ohmd_device_geti(hmd, OHMD_BUTTON_POP_EVENT, event);
			printf("  button %d:                %s\n", event[0], event[1] == OHMD_BUTTON_DOWN ? "down" : "up");
		}
		printf("\nPress ^C to quit\n");

		ohmd_sleep(.01);
	}

	ohmd_ctx_destroy(ctx);
	return 0;
}
