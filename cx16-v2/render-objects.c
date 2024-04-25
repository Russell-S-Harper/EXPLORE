/*
    Contents: "render objects" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include "explore.h"

/* Render all the objects */
void RenderObjects(PLAYER_STATUS *p)
{
	static int16_t
		X[4] = {-100, -100, +100, +100},
		Y[4] = {-100, +100, +50, -50},
		Z[4] = {0, 0, 0, 0};
	int16_t x, y, xr, yr, w, h;

	h = display_height >> 1;
	w = display_width >> 1;

	xr = w + SpecialMultiply(X[0] - p->x, p->cos) - SpecialMultiply(Y[0] - p->y, p->sin);
	yr = h - SpecialMultiply(Y[0] - p->y, p->cos) - SpecialMultiply(X[0] - p->x, p->sin);
	PlotPoint16(xr, yr, CLR16_GREEN);

	x = w + SpecialMultiply(X[1] - p->x, p->cos) - SpecialMultiply(Y[1] - p->y, p->sin);
	y = h - SpecialMultiply(Y[1] - p->y, p->cos) - SpecialMultiply(X[1] - p->x, p->sin);
	DrawLineJustTo16(x, y, CLR16_GREEN);

	x = w + SpecialMultiply(X[2] - p->x, p->cos) - SpecialMultiply(Y[2] - p->y, p->sin);
	y = h - SpecialMultiply(Y[2] - p->y, p->cos) - SpecialMultiply(X[2] - p->x, p->sin);
	DrawLineJustTo16(x, y, CLR16_GREEN);

	x = w + SpecialMultiply(X[3] - p->x, p->cos) - SpecialMultiply(Y[3] - p->y, p->sin);
	y = h - SpecialMultiply(Y[3] - p->y, p->cos) - SpecialMultiply(X[3] - p->x, p->sin);
	DrawLineJustTo16(x, y, CLR16_GREEN);

	DrawLineJustTo16(xr, yr, CLR16_GREEN);

	DrawLineFromTo16(w - 10, h - 10, w + 10, h + 10, CLR16_WHITE);
	DrawLineFromTo16(w - 10, h + 10, w + 10, h - 10, CLR16_WHITE);
}
