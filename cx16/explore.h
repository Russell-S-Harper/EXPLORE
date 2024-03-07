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

/******** Function declarations ********/

/* Routines called by main */

void InitProgram(void);		/* Defined in initialize.c */
void ProcessVehicles(void);	/* Defined in process-vehicles.c */
void ScanField(void);		/* Defined in scan-field.c */
void RenderObjects(void);	/* Defined in render-objects.c */
void UpdateDisplay(void);	/* Defined in cx16-specific.c */

/* Routines defined in cx16-specific.c and called by InitProgram */
void InitSpecific(void);

/* Math routines defined in cx16-specific.c and called throughout the program */
int16_t MultiplyThenDivide(int16_t num1, int16_t num2, int16_t denom);
int16_t SpecialMultiply(int16_t num1, int16_t num2);
int16_t SpecialDivide(int16_t num, int16_t denom);

/* Drawing routines */
void DrawLineWithCallback(int16_t x1, int16_t y1, int16_t x2, int16_t y2, unsigned char color, bool (*callback)(int16_t x, int16_t y));
void Color16Plot(uint16_t x, uint16_t y, unsigned char color);
void Color16Erase(uint16_t x, uint16_t y);

/* Keyboard routine defined in cx16-specific.c and called by ProcessVehicle */
int16_t GetInput(int16_t *);

/* Other routines called throughout the program */
int16_t Sin(int16_t angle);
int16_t Cos(int16_t angle);

/******** Variable declarations ********/
extern XM_HANDLE
	string_data,
	mask_data,
	trig_data,
	tan_data,
	arctan_data,
	left_bounds,
	right_bounds;

extern bool
	exit_program;

extern unsigned char
	mask_index;

extern uint16_t
	display_width,
	display_height;

#endif /* _EXPLORE_H */
