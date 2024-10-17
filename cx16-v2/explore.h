/*
    Contents: "explore" C header file (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#ifndef _EXPLORE_H
#define _EXPLORE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "xm.h"

#define MAX_XYZ			16384
#define MIN_XYZ			0

#define VEHICLE_COUNT		4
#define PLAYER_INDEX		0

typedef struct {
	bool airborne, fire_missile;
	int8_t z_delta, angle_delta;
	int16_t x, y, z, angle, sin, cos;
} VEHICLE;

enum {FRAME_TO_FINISH, SCREEN_TO_FINISH};

/******** Function declarations ********/

/* Routines called by main */
void InitProgram(void);			/* Defined in initialize.c */
void ProcessVehicles(void);		/* Defined in process-vehicles.c */
void ScanField(void);			/* Defined in scan-field.c */
void RenderObjects(void);		/* Defined in render-objects.c */
void UpdateDisplay(void);		/* Defined in cx16-specific.c */

/* Routines defined in cx16-specific.c and called by InitProgram */
void InitSpecific(void);

/* Math routines defined in cx16-specific.c and called throughout the program */
int16_t MultiplyThenDivide(int16_t num1, int16_t num2, int16_t denom);
#define SpecialMultiply(num1, num2)	MultiplyThenDivide((num1), (num2), SCALE_1_0)
#define SpecialDivide(num, denom)	MultiplyThenDivide((num), SCALE_1_0, (denom))

/* Drawing routines */
void DrawLineFromTo16(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
void DrawLineJustTo16(int16_t x, int16_t y, uint8_t color);
void PlotPoint16(int16_t x, int16_t y, uint8_t color);
void ErasePoint16(int16_t x, int16_t y);

/* Keyboard/joystick routine defined in cx16-specific.c and called by ProcessVehicle */
void GetInput(VEHICLE *p);

/* Other routines called throughout the program */
int16_t Minimum(int16_t a, int16_t b);
int16_t Maximum(int16_t a, int16_t b);
int16_t Sin(int16_t angle);
int16_t Cos(int16_t angle);

/******** Variable declarations ********/
extern XM_HANDLE
	string_data,
	trig_data,
	arena_data;

extern VEHICLE
	vehicles[VEHICLE_COUNT];

extern bool
	exit_program;

extern uint16_t
	display_width,
	display_height;

extern int16_t
	arena_index,
	max_segments,
	max_vertices,
	vehicle_index;

#endif /* _EXPLORE_H */
