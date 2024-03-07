/*
    Contents: "render objects" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include <string.h>
#include "explore.h"

static void BeginPolygon(void);
static void EndPolygon(void);
static bool PolygonPointWithMask(int16_t x, int16_t y);

static bool HorizonPointWithMask(int16_t x, int16_t y);

static int16_t Minimum(int16_t a, int16_t b);
static int16_t Maximum(int16_t a, int16_t b);

/* Render all the objects */
void RenderObjects(void)
{
	static unsigned char color;
	static int16_t angle;
	int16_t s, c;
	int16_t x, y, x1, y1, x2, y2, x3, y3;

	if (!color)
		++color;

	x = 20;
	y = 59;

	s = Sin(angle);
	c = Cos(angle);
	x1 = SpecialMultiply(x, c) - SpecialMultiply(y, s) + 160;
	y1 = SpecialMultiply(y, c) + SpecialMultiply(x, s) + 120;

	s = Sin(angle + FULL_CIRC / 3);
	c = Cos(angle + FULL_CIRC / 3);
	x2 = SpecialMultiply(x, c) - SpecialMultiply(y, s) + 160;
	y2 = SpecialMultiply(y, c) + SpecialMultiply(x, s) + 120;

	s = Sin(angle + 2 * FULL_CIRC / 3);
	c = Cos(angle + 2 * FULL_CIRC / 3);
	x3 = SpecialMultiply(x, c) - SpecialMultiply(y, s) + 160;
	y3 = SpecialMultiply(y, c) + SpecialMultiply(x, s) + 120;

	BeginPolygon();
	DrawLineWithCallback(x1, y1, x2, y2, color, PolygonPointWithMask);
	DrawLineWithCallback(x2, y2, x3, y3, color, PolygonPointWithMask);
	DrawLineWithCallback(x3, y3, x1, y1, color, PolygonPointWithMask);
	EndPolygon();

	s = Sin(-angle);
	c = Cos(-angle);
	x1 = SpecialMultiply(x, c) - SpecialMultiply(y, s) + 160;
	y1 = SpecialMultiply(y, c) + SpecialMultiply(x, s) + 120;

	s = Sin(-angle + FULL_CIRC / 3);
	c = Cos(-angle + FULL_CIRC / 3);
	x2 = SpecialMultiply(x, c) - SpecialMultiply(y, s) + 160;
	y2 = SpecialMultiply(y, c) + SpecialMultiply(x, s) + 120;

	s = Sin(-angle + 2 * FULL_CIRC / 3);
	c = Cos(-angle + 2 * FULL_CIRC / 3);
	x3 = SpecialMultiply(x, c) - SpecialMultiply(y, s) + 160;
	y3 = SpecialMultiply(y, c) + SpecialMultiply(x, s) + 120;

	BeginPolygon();
	DrawLineWithCallback(x1, y1, x2, y2, 16 - color, PolygonPointWithMask);
	DrawLineWithCallback(x2, y2, x3, y3, 16 - color, PolygonPointWithMask);
	DrawLineWithCallback(x3, y3, x1, y1, 16 - color, PolygonPointWithMask);
	EndPolygon();

	DrawLineWithCallback(0, display_height / 2, display_width - 1, display_height / 2, MEDIUMGRAY, HorizonPointWithMask);

	angle = (angle + 8) & (FULL_CIRC - 1);
	color = (angle >> 5) & (COLORS - 1);
}

void DrawLineWithCallback(int16_t x1, int16_t y1, int16_t x2, int16_t y2, unsigned char color, bool (*callback)(int16_t x, int16_t y))
{
	int16_t dx, dy, slope;
	int32_t l;
	register int16_t i, t;

	if (x1 != x2 && y1 != y2) {
		/* Regular line */
		dx = x2 - x1;
		dy = y2 - y1;
		if (abs(dx) > abs(dy)) {
			/* |slope| < 1 */
			slope = SpecialDivide(dy, dx);
			if (x1 > x2) {
				i = x1;
				x1 = x2;
				x2 = i;
				i = y1;
				y1 = y2;
				y2 = i;
			}			
			for (i = x1, l = ((int32_t)y1 << SHIFT_1_0) + SCALE_0_5; i <= x2; ++i, l += slope) {
				t = l >> SHIFT_1_0;
				if ((*callback)(i, t))
					Color16Plot((uint16_t)i, (uint16_t)t, color);
			}		
		} else {
			/* |slope| >= 1 */
			slope = SpecialDivide(dx, dy);
			if (y1 > y2) {
				i = x1;
				x1 = x2;
				x2 = i;
				i = y1;
				y1 = y2;
				y2 = i;
			}			
			for (i = y1, l = ((int32_t)x1 << SHIFT_1_0) + SCALE_0_5; i <= y2; ++i, l += slope) {
				t = l >> SHIFT_1_0;
				if ((*callback)(t, i))
					Color16Plot((uint16_t)t, (uint16_t)i, color);
			}
		}
	} else if (y1 == y2) {
		/* Horizontal line */
		if (x1 > x2) {
			i = x1;
			x1 = x2;
			x2 = i;
		}
		for (i = x1; i <= x2; ++i) {
			if ((*callback)(i, y1))
				Color16Plot((uint16_t)i, (uint16_t)y1, color);
		}		
	} else {
		/* Vertical line */
		if (y1 > y2) {
			i = y1;
			y1 = y2;
			y2 = i;
		}
		for (i = y1; i <= y2; ++i) {
			if ((*callback)(x1, i))
				Color16Plot((uint16_t)x1, (uint16_t)i, color);
		}
	}
}

static void BeginPolygon(void)
{
	/* Set the left to a large value 0x7F7F (i.e. 32639), and the right to a small value 0x8080 (i.e. -32640)*/
	memset(GetXMAddressInitial(left_bounds), 0x7F, display_height * sizeof(int16_t));
	memset(GetXMAddressInitial(right_bounds), 0x80, display_height * sizeof(int16_t));
}

static void EndPolygon(void)
{
	int16_t x1, x2;
	register int16_t y;
	for (y = 0; y < display_height; ++y) {
		GetXM(left_bounds, y, &x1);
		GetXM(right_bounds, y, &x2);
		if (x1 <= x2)
			memset(GetXMAddress(GetXMDirectSigned(mask_data, y), x1), mask_index, x2 - x1 + 1);
	}	
}

static bool PolygonPointWithMask(int16_t x, int16_t y)
{
	unsigned char m;
	int16_t *p;
	if (y >= 0 && y < display_height) {
		p = GetXMAddress(left_bounds, y);
		if (x < *p)
			*p = Maximum(x, 0);
		p = GetXMAddress(right_bounds, y);
		if (x > *p)
			*p = Minimum(x, display_width - 1);
		if (x >= 0 && x < display_width) {
			GetXM(GetXMDirectSigned(mask_data, y), x, &m);
			return (m != mask_index);
		}	
	}
	return false;	
}

static bool HorizonPointWithMask(int16_t x, int16_t y)
{
	unsigned char m;
	if (y >= 0 && y < display_height) {
		if (x >= 0 && x < display_width) {
			GetXM(GetXMDirectSigned(mask_data, y), x, &m);
			return (m != mask_index);
		}	
	}
	return false;	
}

/* Convenience functions usually done as macros, but using functions to avoid side effects! */

static int16_t Minimum(int16_t a, int16_t b) { return a < b? a: b; }

static int16_t Maximum(int16_t a, int16_t b) { return a > b? a: b; }



