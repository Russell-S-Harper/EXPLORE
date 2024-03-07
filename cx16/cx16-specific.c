/*
    Contents: "cx16 specific" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

/* cx16-specific.c */

#include <stddef.h>
#include <string.h>
#include <conio.h>
#include <tgi.h>
#include <cx16.h>
#include <unistd.h>

#include "explore.h"

/******** CX16 specific ********/

#define HORZ_RES	320
#define VERT_RES	240

#define SCR_1_BASE	0x00
#define SCR_1_ADDR	0x0000
#define SCR_2_BASE	0x4C
#define SCR_2_ADDR	0x9800

#define PIXELS_PER_BYTE	2
#define BITS_PER_WORD	16
#define SHIFT_MASK	0x8000

				/* Equivalent to y * HORZ_RES / PIXELS_PER_BYTE + x / PIXELS_PER_BYTE */
#define GetScreenAddress(x, y)	((uint32_t)(((y) << 7) + ((y) << 5) + ((x) >> 1)) \
					+ (VERA.layer0.tilebase? (uint32_t)SCR_1_ADDR: (uint32_t)SCR_2_ADDR))

#define ZISHFT		2	/* improves the climbing response (* 4) */
#define MAXCNTR		20	/* limit for spinning and climbing rate */
#define MSMOVE		8	/* for keyboard emulation of mouse */

/* Keyboard defines used in GetInput */
#define PAUSE_PROGRAM	27	/* escape key: pause/unpause program */
#define CLIMB_FASTER	145	/* cursor up: climb faster */
#define TURN_LEFT	157	/* cursor left: turn left */
#define TURN_RIGHT	29	/* cursor right: turn right */
#define DIVE_FASTER	17	/* cursor down: dive faster */
#define ACCELERATE	130	/* page up: accelerate */
#define DEACCELERATE	2	/* page down: deaccelerate */
#define QUIT_PROGRAM	81	/* Q: to quit */

/* Mouse emulation using keyboard.
	The original design used the delta of the mouse position
	to slide the player's vehicle around, the left mouse button
	to toggle the targets, and the right mouse button to cycle
	through the drones */
#define SLIDE_FORWARD	87	/* W: slide forward */
#define SLIDE_LEFT	65	/* A: slide left */
#define SLIDE_RIGHT	68	/* D: slide right */
#define SLIDE_BACKWARD	83	/* S: slide backward */
#define TOGGLE_TARGETS	50	/* 2: toggle targets */
#define	CYCLE_DRONES	49	/* 1: cycle drones */

#define LGEAR 		9	/* Divide SCALE_1_0 by 2^LGEAR to give low gear */
#define HGEAR 		6	/* and by 2^HGEAR to give high gear */
	
static int16_t dxmouse, dymouse, altcntr, dircntr, vgear, lbutton, rbutton;

/* Initialize the hardware, etc. */
void InitSpecific(void)
{
	int16_t i;
	XM_HANDLE *p;

	/* Set up */
	tgi_install(tgi_static_stddrv);
	if (tgi_geterror() != TGI_ERR_OK)
		ExitProgram(GIERR);

	/* Initialize */
	tgi_init();
	tgi_clear();

	/* Reserve and clear memory for the mask */
	mask_data = AllocXM(VERT_RES, sizeof(XM_HANDLE));
	for (i = 0; i < VERT_RES; ++i) {
		p = GetXMAddress(mask_data, i);
		*p = AllocXM(HORZ_RES, sizeof(unsigned char));
		memset(GetXMAddressInitial(*p), 0, HORZ_RES);
	}

	/* Save the resolution */
	display_width = HORZ_RES;
	display_height = VERT_RES;

	/* Set the bitmap to 16-color mode */
	VERA.layer0.config = 6;

	/* Set the offset */
	VERA.layer0.tilebase = SCR_2_BASE;

	/* Clear the end of the second screen */
	UpdateDisplay();
}

/* Switch to the other screen, clear the previous screen, and perform other functions */
void UpdateDisplay(void)
{
	static int16_t index;
	unsigned char base, i, j;
	uint16_t address;
	register volatile unsigned char *p;

	/* Advance the mask index */
	if (!++mask_index)
		mask_index = 1;

	/* While waiting for the end of the screen display, clear some of the mask! */
	do {
		memset(GetXMAddressInitial(GetXMDirectSigned(mask_data, index)), 0, HORZ_RES);
		if (++index >= VERT_RES)
			index = 0;
	} while (((VERA.irq_enable & 0x40)? 256: 0) + VERA.irq_raster < (VERT_RES * 2 - 1));

	/* Set up the next screen */
	switch (VERA.layer0.tilebase) {
		case SCR_1_BASE:
			base = SCR_2_BASE;
			address = SCR_1_ADDR;
			break;
		case SCR_2_BASE:
			base = SCR_1_BASE;
			address = SCR_2_ADDR;
			break;
		default:
			ExitProgram(DCERR);
	}

	/* Switch to the other screen */
	VERA.layer0.tilebase = base;

	/* Set the address to start clearing */
	VERA.address = address;
	VERA.address_hi = VERA_INC_1;

	/* Clear the previous screen */
	for (p = &VERA.data0, i = 0; i < VERT_RES; ++i) {
		for (j = 0; j < HORZ_RES / PIXELS_PER_BYTE / 8; ++j) {
			*p = BLACK;
			*p = BLACK;
			*p = BLACK;
			*p = BLACK;
			*p = BLACK;
			*p = BLACK;
			*p = BLACK;
			*p = BLACK;
		}
	}
}

/* Pixel routines */
void Color16Plot(uint16_t x, uint16_t y, unsigned char color)
{
	uint32_t address = GetScreenAddress(x, y);

	VERA.address = (uint16_t)(address & UINT16_MAX);
	VERA.address_hi = (uint16_t)(address >> BITS_PER_WORD);

	/* Note: the pixel is assumed to be all zeroes (i.e. BLACK)! */
	if (x & 1)
		VERA.data0 |= (color & 0x0F);
	else
		VERA.data0 |= (color << 4);
}

void Color16Erase(uint16_t x, uint16_t y)
{
	uint32_t address = GetScreenAddress(x, y);

	VERA.address = (uint16_t)(address & UINT16_MAX);
	VERA.address_hi = (uint16_t)(address >> BITS_PER_WORD);
	
	if (x & 1)
		VERA.data0 &= 0xF0;
	else
		VERA.data0 &= 0x0F;
}

/* GetInput processes keyboard and mouse input.
	When translating this routine, note that kbhit returns nonzero
	if a character is ready on the keyboard queue and that cgetc
	returns the next character on the queue, or waits until one is
	available. At the time of writing, the CX16 emulator didn't have
	satisfactory mouse support within cc65, so for now, emulating
	with the keyboard. */
int16_t GetInput(int16_t *vp)
{
	int16_t altstat = 0, dirstat = 0;

	/* Update the "mouse" position */
	dxmouse = 0;
	dymouse = 0;

	/* Process keyboard input */
	while (kbhit()) {

		switch ((unsigned char)cgetc()) {

			case PAUSE_PROGRAM:
				/* Wait until it's pressed again */
				while (cgetc() != PAUSE_PROGRAM);
				break;

			case CLIMB_FASTER:
				if (altcntr < 0) altcntr = 0;
				else if (altcntr < MAXCNTR) ++altcntr;
				altstat = 1;
				break;

			case TURN_LEFT:
				if (dircntr > 0) dircntr = 0;
				else if (dircntr > -MAXCNTR) --dircntr;
				dirstat = 1;
				break;

			case TURN_RIGHT:
				if (dircntr < 0) dircntr = 0;
				else if (dircntr < MAXCNTR) ++dircntr;
				dirstat = 1;
				break;

			case DIVE_FASTER:
				if (altcntr > 0) altcntr = 0;
				else if (altcntr > -MAXCNTR) --altcntr;
				altstat = 1;
				break;

			case ACCELERATE:
				if (vgear > HGEAR) --vgear;
				break;

			case DEACCELERATE:
				if (vgear < LGEAR) ++vgear;
				break;

			case SLIDE_FORWARD:
				dymouse = +MSMOVE;
				break;

			case SLIDE_LEFT:
				dxmouse = -MSMOVE;
				break;

			case SLIDE_RIGHT:
				dxmouse = +MSMOVE;
				break;

			case SLIDE_BACKWARD:
				dymouse = -MSMOVE;
				break;

			case TOGGLE_TARGETS:
				lbutton = 1;
				break;

			case CYCLE_DRONES:
				rbutton = 1;
				break;

			case QUIT_PROGRAM:
				tgi_done();
				exit_program = true;
				break;
		}
	}
/*
	vp[VEHICD] += dircntr;
	zt = vp[VEHICZ] + (altcntr << ZISHFT);

	if (!altstat) {
		if (q = ((d = altcntr) < 0)) d = -d;
		d >>= 1;
		altcntr = (q)? -d: d;
	}

	if (!dirstat) {
		if (q = ((d = dircntr) < 0)) d = -d;
		d >>= 1;
		dircntr = (q)? -d: d;
	}
*/
	return *vp;
}


/* The program insures that the values passed to and returned
	from MultiplyThenDivide, SpecialMultiply and SpecialDivide
	never exceed |32767|. Since these three routines are used
	frequently, it is well worth the effort to optimize them. */

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
