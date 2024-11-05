/*
    Contents: "cx16 specific" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

/* cx16-specific.c */

#include <limits.h>
#include <time.h>
#include <conio.h>
#include <tgi.h>
#include "explore.h"
#include "vera.h"

/* Ideal frames/s - adjust as required to prevent "jerking".
	Limited by the resolution of CLOCKS_PER_SEC.

	FRAMES_PER_SEC | Actual frames/s
	------------- -+----------------
	1 - 6          | 1 - 6
	7              | 7.500
	8              | 8.571
	9, 10          | 10
	11, 12         | 12
	13 - 15        | 15
	16 - 20        | 20
	21 - 30        | 30
	31 - 60        | 60
*/
#define FRAMES_PER_SEC	6

/* Keyboard defines used in GetPlayerInput */
#define PAUSE_PROGRAM		27	/* escape: pause/unpause program */
#define REQ_CLIMB_OR_GEAR_FWD	145	/* cursor up: climb or gear forward */
#define REQ_DIVE_OR_GEAR_REV	17	/* cursor down: dive or gear reverse */
#define TURN_RIGHT		29	/* cursor right: turn right */
#define TURN_LEFT		157	/* cursor left: turn left */
#define FIRE_MISSILE		20	/* space: fire missile */
#define CYCLE_PLAYER		49	/* 1: cycle player */
#define QUIT_PROGRAM		81	/* Q: quit program */

/* Hex digit defines */
#define HEX_DGT_HI	0xF0
#define HEX_DGT_LO	0x0F
#define SHIFT_HEX_DGT	4

static void DrawLine16(void);
static void DefaultCallback(int waiting);

static uint8_t
	current_color;

static int16_t
	x_from_point,
	y_from_point,
	x_to_point,
	y_to_point;

/* Initialize the hardware, etc. */
void InitSpecific(void)
{
	/* Set up */
	tgi_install(tgi_static_stddrv);
	if (tgi_geterror() != TGI_ERR_OK)
		ExitProgram(ERR_GI);

	/* Initialize */
	tgi_init();
	tgi_clear();

	/* Save the resolution */
	display_width = VERA_HORZ_RES;
	display_height = VERA_VERT_RES;

	/* Set the bitmap to 16-color mode */
	VERA_L0_CONFIG = VERA_16_COLOR_MODE;

	/* Set the offset */
	VERA_L0_TILEBASE = VERA_SCR_2_BASE;

	/* Clear the remainder of screen 2 */
	UpdateDisplay();
}

/* Switch to the other screen, clear the previous screen, and perform other functions */
void UpdateDisplay(void)
{
	static clock_t frame_clock;
	uint8_t base, address, i, j;
	clock_t current_clock;
	register volatile uint8_t *p;

	/* Wait for the end of the frame based on clock cycles */
	if (!frame_clock)
		frame_clock = clock();
	else {
		do {
			/* While waiting for the end of the frame, do some work */
			DefaultCallback(FRAME_TO_FINISH);
			/* Check if the frame is done */
			current_clock = clock();
			/* Check for rollover - not likely to happen but JIC! */
			if (current_clock < frame_clock)
				break;
		}
		while (current_clock - frame_clock < (CLOCKS_PER_SEC / FRAMES_PER_SEC));
		frame_clock = current_clock;
	}

	/* Wait for the end of the current screen based on scan lines */
	do
		/* While waiting for the end of the current screen, do some work */
		DefaultCallback(SCREEN_TO_FINISH);
	while (((VERA_IEN & VERA_SCANLINE_H)? 256: 0) + VERA_IRQLINE_L < (VERA_VERT_RES * 2 - 1));

	/* Set up the next screen */
	switch (VERA_L0_TILEBASE) {
		case VERA_SCR_1_BASE:
			base = VERA_SCR_2_BASE;
			address = VERA_SCR_1_ADDR;
			break;
		case VERA_SCR_2_BASE:
			base = VERA_SCR_1_BASE;
			address = VERA_SCR_2_ADDR;
			break;
		default:
			ExitProgram(ERR_DC);
	}

	/* Switch to the other screen */
	VERA_L0_TILEBASE = base;

	/* Set the address to start clearing */
	VERA_ADDRx_L = 0x00;
	VERA_ADDRx_M = address;
	VERA_ADDRx_H = VERA_INC_1;

	/* Clear the previous screen */
	for (p = &VERA_DATA0, i = 0; i < VERA_VERT_RES; ++i) {
		for (j = 0; j < VERA_HORZ_RES / VERA_PX_PER_BYTE / 8; ++j) {
			*p = ((CLR16_BLACK << SHIFT_HEX_DGT) | CLR16_BLACK);
			*p = ((CLR16_BLACK << SHIFT_HEX_DGT) | CLR16_BLACK);
			*p = ((CLR16_BLACK << SHIFT_HEX_DGT) | CLR16_BLACK);
			*p = ((CLR16_BLACK << SHIFT_HEX_DGT) | CLR16_BLACK);
			*p = ((CLR16_BLACK << SHIFT_HEX_DGT) | CLR16_BLACK);
			*p = ((CLR16_BLACK << SHIFT_HEX_DGT) | CLR16_BLACK);
			*p = ((CLR16_BLACK << SHIFT_HEX_DGT) | CLR16_BLACK);
			*p = ((CLR16_BLACK << SHIFT_HEX_DGT) | CLR16_BLACK);
		}
	}
}

/* Default callback to do useful work while waiting */
static void DefaultCallback(int waiting)
{
	/* Perform tasks dependent on what we're waiting for */
	switch (waiting) {
		case FRAME_TO_FINISH:
			break;
		case SCREEN_TO_FINISH:
			break;
	}
}

/* Drawing routines */
void DrawLineFromTo16(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color)
{
	x_from_point = x1;
	y_from_point = y1;
	x_to_point = x2;
	y_to_point = y2;
	current_color = color;
	DrawLine16();
}

void DrawLineJustTo16(int16_t x, int16_t y, uint8_t color)
{
	x_from_point = x_to_point;
	y_from_point = y_to_point;
	x_to_point = x;
	y_to_point = y;
	current_color = color;
	DrawLine16();
}

static void DrawLine16(void)
{
	uint8_t d0, d1;
	int16_t x1, y1, x2, y2, xmin, xmax, ymin, ymax, xt, yt, dx, dy, slope;
	uint32_t address;
	register volatile uint8_t *p;
	register uint8_t c;
	register int16_t i;

	/* Copy because points may be changed and/or swapped after clipping */
	x1 = x_from_point;
	y1 = y_from_point;
	x2 = x_to_point;
	y2 = y_to_point;

	/* Will need these */
	ymax = Maximum(y1, y2);
	ymin = Minimum(y1, y2);
	xmax = Maximum(x1, x2);
	xmin = Minimum(x1, x2);

	/* Check if clipping is required */
	if (ymax > (VERA_VERT_RES - 1) || ymin < 0 || xmax > (VERA_HORZ_RES - 1) || xmin < 0) {
	    	/* If there's no chance of intersection, just return */
		if (ymax < 0 || ymin > (VERA_VERT_RES - 1) || xmax < 0 || xmin > (VERA_HORZ_RES - 1))
			return;
		/* Coordinates can change, so discarding xmin, xmax, ymin, and ymax */
		if (Maximum(y1, y2) > (VERA_VERT_RES - 1) && y2 != y1) {
			xt = MultiplyThenDivide((VERA_VERT_RES - 1) - y1, x2 - x1, y2 - y1) + x1;
			if (y2 > y1) {
				y2 = VERA_VERT_RES - 1;
				x2 = xt;
			} else {
				y1 = VERA_VERT_RES - 1;
				x1 = xt;
			}
		}
		if (Minimum(y1, y2) < 0 && y2 != y1) {
			xt = MultiplyThenDivide(-y1, x2 - x1, y2 - y1) + x1;
			if (y2 < y1) {
				y2 = 0;
				x2 = xt;
			} else {
				y1 = 0;
				x1 = xt;
			}
		}
		if (Maximum(x1, x2) > (VERA_HORZ_RES - 1) && x2 != x1) {
			yt = MultiplyThenDivide((VERA_HORZ_RES - 1) - x1, y2 - y1, x2 - x1) + y1;
			if (x2 > x1) {
				x2 = VERA_HORZ_RES - 1;
				y2 = yt;
			} else {
				x1 = VERA_HORZ_RES - 1;
				y1 = yt;
			}
		}
		if (Minimum(x1, x2) < 0 && x2 != x1) {
			yt = MultiplyThenDivide(-x1, y2 - y1, x2 - x1) + y1;
			if (x2 < x1) {
				x2 = 0;
				y2 = yt;
			} else {
				x1 = 0;
				y1 = yt;
			}
		}
		/* Final check! */
		if (y1 < 0 || y2 < 0 || y1 > (VERA_VERT_RES - 1) || y2 > (VERA_VERT_RES - 1)
		    || x1 < 0 || x2 < 0 || x1 > (VERA_HORZ_RES - 1) || x2 > (VERA_HORZ_RES - 1))
			return;
	}

	/* Render "right to left" to account for 0.5 pixel bias */
	if (x1 < x2) {
		xt = x1;
		x1 = x2;
		x2 = xt;
		yt = y1;
		y1 = y2;
		y2 = yt;
	}

	/* Will need these */
	dx = x2 - x1;
	dy = y2 - y1;

	/* Add 1 to i because we want to render all points up to and including the endpoints */
	if (abs(dy) > abs(dx)) {
		slope = abs(SpecialDivide(dx, dy)) >> 3;
		d0 = VERA_ADV_BY_0_5 | (dx < 0? VERA_DECR: VERA_INCR);
		d1 = VERA_ADV_BY_160 | (dy < 0? VERA_DECR: VERA_INCR);
		i = abs(dy) + 1;
	} else {
		slope = abs(SpecialDivide(dy, dx)) >> 3;
		d0 = VERA_ADV_BY_160 | (dy < 0? VERA_DECR: VERA_INCR);
		d1 = VERA_ADV_BY_0_5 | (dx < 0? VERA_DECR: VERA_INCR);
		i = abs(dx) + 1;
	}

	/* Get the address */
	address = VERA_ADDR_FX(x1, y1);

	/* DCSEL = 2 */
	VERA_CTRL = VERA_DCSEL_2;

	/* Turn on FX 4-bit mode and line helper */
	VERA_FX_CTRL = VERA_FX_4_BIT_MODE | VERA_FX_LINE_HELPER;

	/* Select ADDR1 */
	VERA_CTRL = VERA_DCSEL_2 | VERA_ADDR_1;

	/* Always increment setting */
	VERA_ADDRx_H = d1;

	/* Set ADDR1 - address format is a bit wonky, just deal with it! */
	VERA_ADDRx_H |= (address & 0x01) << 1;
	address >>= 1;
	VERA_ADDRx_L = address & UINT8_MAX;
	address >>= CHAR_BIT;
	VERA_ADDRx_M = address & UINT8_MAX;
	address >>= CHAR_BIT;
	VERA_ADDRx_H |= address & 0x01;

	/* Select ADDR0 */
	VERA_CTRL = VERA_DCSEL_2 | VERA_ADDR_0;

	/* Occasionally increment setting */
	VERA_ADDRx_H = d0;

	/* DCSEL = 3 */
	VERA_CTRL = VERA_DCSEL_3;

	/* Set slope */
	VERA_FX_X_INCR_L = slope & UINT8_MAX;
	slope >>= CHAR_BIT;
	VERA_FX_X_INCR_H = slope & 0x03;

	/* Draw the line */
	p = &VERA_DATA1;
	c = (current_color << SHIFT_HEX_DGT) | current_color;
	while (i >= 8) {
		*p = c;
		*p = c;
		*p = c;
		*p = c;
		*p = c;
		*p = c;
		*p = c;
		*p = c;
		i -= 8;
	}
	for (; i > 0; --i)
		*p = c;

	/* DCSEL = 2 */
	VERA_CTRL = VERA_DCSEL_2;

	/* Turn off 4-bit and line helper mode */
	VERA_FX_CTRL = VERA_TRADITIONAL;
}

/* Pixel routines */
void PlotPoint16(int16_t x, int16_t y, uint8_t color)
{
	uint32_t address;

	if (y >= 0 && y < VERA_VERT_RES && x >= 0 && x < VERA_HORZ_RES) {
		address = VERA_ADDR_PT(x, y);
		VERA_ADDRx_L = (uint8_t)(address & UINT8_MAX);
		address >>= CHAR_BIT;
		VERA_ADDRx_M = (uint8_t)(address & UINT8_MAX);
		address >>= CHAR_BIT;
		VERA_ADDRx_H = (uint8_t)(address & UINT8_MAX);

		if (x & 1) {
			VERA_DATA0 &= HEX_DGT_HI;
			VERA_DATA0 |= (color & HEX_DGT_LO);
		} else {
			VERA_DATA0 &= HEX_DGT_LO;
			VERA_DATA0 |= (color << SHIFT_HEX_DGT);
		}
	}

	/* Set as the current "to-point" */
	x_to_point = x;
	y_to_point = y;
}

void ErasePoint16(int16_t x, int16_t y)
{
	uint32_t address;

	if (y >= 0 && y < VERA_VERT_RES && x >= 0 && x < VERA_HORZ_RES) {
		address = VERA_ADDR_PT(x, y);
		VERA_ADDRx_L = (uint8_t)(address & UINT8_MAX);
		address >>= CHAR_BIT;
		VERA_ADDRx_M = (uint8_t)(address & UINT8_MAX);
		address >>= CHAR_BIT;
		VERA_ADDRx_H = (uint8_t)(address & UINT8_MAX);

		if (x & 1)
			VERA_DATA0 &= HEX_DGT_HI;
		else
			VERA_DATA0 &= HEX_DGT_LO;
	}
}

/* Convenience functions usually implemented as macros, but using functions to avoid side effects! */

int16_t Minimum(int16_t a, int16_t b) { return a < b? a: b; }

int16_t Maximum(int16_t a, int16_t b) { return a > b? a: b; }

/* Processes keyboard/joystick (pending) input */
void GetPlayerInput(VEHICLE *vehicle)
{
	/* Process keyboard input */
	while (kbhit()) {

		switch ((uint8_t)cgetc()) {

			case PAUSE_PROGRAM:
				/* Wait until it's pressed again */
				while (cgetc() != PAUSE_PROGRAM);
				break;

			case REQ_CLIMB_OR_GEAR_FWD:
				if (vehicle->airborne)
					vehicle->z_delta += 1;
				else
					vehicle->gear += 1;
				break;

			case REQ_DIVE_OR_GEAR_REV:
				if (vehicle->airborne)
					vehicle->z_delta -= 1;
				else
					vehicle->gear -= 1;
				break;

			case TURN_RIGHT:
				vehicle->angle_delta += 1;
				break;

			case TURN_LEFT:
				vehicle->angle_delta -= 1;
				break;

			case FIRE_MISSILE:
				vehicle->fire_missile = true;
				break;

			case CYCLE_PLAYER:
				if (++vehicle_index >= VEHICLE_COUNT)
					vehicle_index = 0;
				break;

			case QUIT_PROGRAM:
				tgi_done();
				exit_program = true;
				break;
		}
	}
}

/* The program insures that the values passed to and returned
	from MultiplyThenDivide never exceed |32767|. Since this routine
	is used frequently, it is well worth the effort to optimize it. */

/* Equivalent to (int16_t)((int32_t)num1 * num2 / denom) */
int16_t MultiplyThenDivide(int16_t num1, int16_t num2, int16_t denom)
{
	uint8_t *p, *q, *r;
	int32_t c;

	/* Using the VERA multiplier so representing the parameters as sequences of bytes */
	p = (uint8_t *)&num1;
	q = (uint8_t *)&num2;
	r = (uint8_t *)&c;

	/* Set up the multiplier */
	VERA_CTRL = VERA_DCSEL_2;
	VERA_FX_MULT = VERA_FX_MULTIPLIER;

	/* Set the operands */
	VERA_CTRL = VERA_DCSEL_6;
	VERA_FX_CACHE_L = p[0];
	VERA_FX_CACHE_M = p[1];
	VERA_FX_CACHE_H = q[0];
	VERA_FX_CACHE_U = q[1];

	/* Set to write in unused space between the screens */
	VERA_CTRL = VERA_DCSEL_2 | VERA_ADDR_0;
	VERA_FX_CTRL = VERA_FX_CACHE_WRITE;
	VERA_ADDRx_L = 0x00;
	VERA_ADDRx_M = VERA_UNUSED_ADDR;
	VERA_ADDRx_H = 0x00;

	/* Trigger the multiply */
	VERA_DATA0 = 0x00;

	/* Save the result */
	VERA_ADDRx_H = VERA_INC_1;
	r[0] = VERA_DATA0;
	r[1] = VERA_DATA0;
	r[2] = VERA_DATA0;
	r[3] = VERA_DATA0;

	/* Restore */
	VERA_FX_CTRL = VERA_TRADITIONAL;

	/* Unfortunately, no VERA divider (yet!) */
	return (int16_t)(c / denom);
}
