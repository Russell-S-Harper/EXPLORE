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

#define MAX_XYZ			16383
#define MIN_XYZ			0

#define VEHICLE_COUNT		24
#define PLAYER_COUNT		4
#define PLAYER_INDEX		0

/* How +/- close in height needed to be considered targetable */
#define VEHICLE_Z_TOLERANCE	191

/* Appearance */
enum {APP_PRM, APP_AUX, APP_CNT};

/* Explosions are last two "vehicles" */
#define EXP_APP_PRM_OFFSET	2
#define EXP_APP_AUX_OFFSET	1

/* Counters (in frames) for how long a missile takes to reload and missile lifespan */
#define MSS_LOADING_COUNTER	5
#define MSS_COUNTDOWN_COUNTER	30

/* VEHICLE combines player, NPCs, and missiles */
typedef struct {
	bool active, airborne, exploding, fire;
	int8_t z_delta, angle_delta, countdown, gear, loading;
	int16_t x, y, z, angle, sin, cos;
	XM_HANDLE appearance[APP_CNT];
} VEHICLE;

/* Callback routine hint */
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
void GetPlayerInput(VEHICLE *p);

/* Other routines called throughout the program */
int16_t Minimum(int16_t a, int16_t b);
int16_t Maximum(int16_t a, int16_t b);
int16_t Sin(int16_t angle);
int16_t Cos(int16_t angle);

/******** Variable declarations ********/
extern XM_HANDLE
	g_string_data,
	g_trig_data,
	g_arena_data,
	g_exploding_prm,
	g_exploding_aux,
	*g_vehicle_data;

extern VEHICLE
	*g_vehicles;

extern bool
	g_exit_program;

extern uint16_t
	g_display_width,
	g_display_height;

extern int16_t
	g_arena_index,
	g_max_arena_segments,
	g_max_arena_vertices,
	g_vehicle_index,
	g_max_vehicle_segments,
	g_max_vehicle_vertices;

#endif /* _EXPLORE_H */
