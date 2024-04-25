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

/* Ideal frames/s - adjust as required to prevent jerking */
#define FRAMES_PER_SEC	10

/* Keyboard defines used in GetInput */
#define PAUSE_PROGRAM	27	/* escape: pause/unpause program */
#define REQ_CLIMB	145	/* cursor up: climb */
#define REQ_DIVE	17	/* cursor down: dive */
#define TURN_RIGHT	29	/* cursor right: turn right */
#define TURN_LEFT	157	/* cursor left: turn left */
#define FIRE_MISSILE	20	/* space: fire missile */
#define CYCLE_PLAYER	49	/* 1: cycle player */
#define QUIT_PROGRAM	81	/* Q: quit program */

/* Line drawing modes */
enum {DRAW_JUST_TO = 0, DRAW_FROM_TO = 1};

/* Hex digit defines */
#define HEX_DGT_HI	0xF0
#define HEX_DGT_LO	0x0F
#define SHIFT_HEX_DGT	4

static void DrawLine16(void);
static void DefaultCallback(int waiting);
static int16_t Minimum(int16_t a, int16_t b);
static int16_t Maximum(int16_t a, int16_t b);

static uint8_t
	current_mode,
	current_color;

static int16_t
	x_from_point,
	y_from_point,
	x_to_point,
	y_to_point;

static PLAYER_STATUS player_status;

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
	UpdateDisplay(NULL);
}

/* Switch to the other screen, clear the previous screen, and perform other functions */
void UpdateDisplay(void (*callback)(int waiting))
{
	static clock_t frame_clock;
	uint8_t base, address, i, j;
	clock_t current_clock;
	register volatile uint8_t *p;

	/* Provide a default if none given */
	if (!callback)
		callback = DefaultCallback;

	/* Wait for the end of the frame based on clock cycles */
	if (!frame_clock)
		frame_clock = clock();
	else {
		do {
			/* While waiting for the end of the frame, do some work */
			callback(FRAME_TO_FINISH);
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
		callback(SCREEN_TO_FINISH);
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
	current_mode = DRAW_FROM_TO;
	x_from_point = x1;
	y_from_point = y1;
	x_to_point = x2;
	y_to_point = y2;
	current_color = color;
	DrawLine16();
}

void DrawLineJustTo16(int16_t x, int16_t y, uint8_t color)
{
	current_mode = DRAW_JUST_TO;
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
		/* Because clipping was required, change the mode to DRAW_FROM_TO
			to ensure the initial point is rendered */
		current_mode = DRAW_FROM_TO;
	}

	switch (current_mode) {
		/* Render "right to left" for from-to
			- accounts for 0.5 pixel bias */
		case DRAW_FROM_TO:
			if (x1 < x2) {
				xt = x1;
				x1 = x2;
				x2 = xt;
				yt = y1;
				y1 = y2;
				y2 = yt;
			}
			break;
		/* Render "left to right" for just-to
			- 0.5 pixel bias will automatically skip the first one */
		case DRAW_JUST_TO:
			if (x2 > x1) {
				xt = x1;
				x1 = x2;
				x2 = xt;
				yt = y1;
				y1 = y2;
				y2 = yt;
			}
			break;
	}

	/* Will need these */
	dx = x2 - x1;
	dy = y2 - y1;

	/* DRAW_FROM_TO will add 1 to i because we want to render all points,
		up to and including the endpoints, while DRAW_JUST_TO will
		not draw the first point, but will draw the last point */
	if (abs(dy) > abs(dx)) {
		slope = abs(SpecialDivide(dx, dy)) >> 3;
		d0 = VERA_ADV_BY_0_5 | (dx < 0? VERA_DECR: VERA_INCR);
		d1 = VERA_ADV_BY_160 | (dy < 0? VERA_DECR: VERA_INCR);
		i = abs(dy) + current_mode;
	} else {
		slope = abs(SpecialDivide(dy, dx)) >> 3;
		d0 = VERA_ADV_BY_160 | (dy < 0? VERA_DECR: VERA_INCR);
		d1 = VERA_ADV_BY_0_5 | (dx < 0? VERA_DECR: VERA_INCR);
		i = abs(dx) + current_mode;
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

/* Convenience functions usually done as macros, but using functions to avoid side effects! */

static int16_t Minimum(int16_t a, int16_t b) { return a < b? a: b; }

static int16_t Maximum(int16_t a, int16_t b) { return a > b? a: b; }

/* Processes keyboard/joystick (pending) input */
PLAYER_STATUS *GetInput(uint8_t player)
{
	if (player)
		ExitProgram(ERR_OB);

	/* Process keyboard input */
	while (kbhit()) {

		switch ((uint8_t)cgetc()) {

			case PAUSE_PROGRAM:
				/* Wait until it's pressed again */
				while (cgetc() != PAUSE_PROGRAM);
				break;

			case REQ_CLIMB:
				++player_status.z_delta;
				break;

			case REQ_DIVE:
				--player_status.z_delta;
				break;

			case TURN_RIGHT:
				player_status.angle_delta -= 2;
				break;

			case TURN_LEFT:
				player_status.angle_delta += 2;
				break;

			case FIRE_MISSILE:
				player_status.fire_missile = true;
				break;

			case CYCLE_PLAYER:
				++player_status.player;
				break;

			case QUIT_PROGRAM:
				tgi_done();
				exit_program = true;
				break;
		}
	}
	return &player_status;
}

/* The program insures that the values passed to and returned
	from MultiplyThenDivide, SpecialMultiply and SpecialDivide
	never exceed |32767|. Since these three routines are used
	frequently, it is well worth the effort to optimize them. */

/* Multiplication and division constants */
#define BITS_PER_WORD	16
#define SHIFT_MASK	0x8000

/* Equivalent to (int16_t)((int32_t)num1 * num2 / denom) */
int16_t MultiplyThenDivide(int16_t num1, int16_t num2, int16_t denom)
{
	uint32_t divisor, product;
	uint16_t i, working;
	int16_t result = 0, sign = 0, shifts = BITS_PER_WORD - 1;

	if (num1 && num2 && denom) {
		/* Normalize to positive */
		if (num1 < 0) {
			++sign;
			num1 = -num1;
		}
		if (num2 < 0) {
			++sign;
			num2 = -num2;
		}
		if (denom < 0) {
			++sign;
			denom = -denom;
		}
		/* num1 * num2 to 32-bit product */
		if (num1 > 1 && num2 > 1) {
			/* Check if num2 or num1 are powers of two, e.g. for SpecialMultiply */
			if (!(num2 & (num2 - 1))) {
				product = num1;
				while (num2 > 1) {
					product <<= 1;
					num2 >>= 1;
				}
			} else if (!(num1 & (num1 - 1))) {
				product = num2;
				while (num1 > 1) {
					product <<= 1;
					num1 >>= 1;
				}
			} else {
				/* Regular multiply */
				i = SHIFT_MASK;
				product = 0;
				while (i) {
					product <<= 1;
					if (i & num2)
						product += num1;
					i >>= 1;
				}
			}
		} else
			product = num1 > num2? num1: num2;
		/* product / denom to 16-bit result */
		if (denom > 1) {
			/* Check for powers of two, e.g. for SpecialDivide */
			if (!(denom & (denom - 1))) {
				while (denom > 1) {
					product >>= 1;
					denom >>= 1;
				}
				result = product;
			} else {
				divisor = (uint32_t)denom;
				/* Align */
				while (divisor < product) {
					divisor <<= 1;
					--shifts;
				}
				i = SHIFT_MASK;
				working = 0;
				while (i) {
					if (product >= divisor) {
						product -= divisor;
						working |= i;
					}
					product <<= 1;
					i >>= 1;
				}
				working >>= shifts;
				result = (int16_t)working;
			}
		} else
			result = product;
		if (sign & 1) result = -result;
	}
	return result;
}

/* Equivalent to (int16_t)(((int32_t)num1 * num2) >> SHIFT_1_0) */
int16_t SpecialMultiply(int16_t num1, int16_t num2)
{
	return MultiplyThenDivide(num1, num2, SCALE_1_0);
}

/* Equivalent to (int16_t)(((int32_t)num << SHIFT_1_0) / denom) */
int16_t SpecialDivide(int16_t num, int16_t denom)
{
	return MultiplyThenDivide(num, SCALE_1_0, denom);
}
