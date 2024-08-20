/*
    Contents: "render objects" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include "explore.h"

#define SCALE_SHIFT	4

/* Render all the objects */
void RenderObjects(PLAYER_STATUS *p)
{
	static int16_t
		X[4] = {-100, -100, +100, +100},
		Y[4] = {-100, +100, +50, -50},
		Z[4] = {+12000, +12000, +12000, +12000};
	int16_t xr, yr, x, y, w, h, scale;

	h = 3 * display_height >> 2;
	w = display_width >> 1;

	scale = SCALE_1_0 + (p->z >> SCALE_SHIFT);

	xr = w + MultiplyThenDivide(X[0] - p->x, p->cos, scale) - MultiplyThenDivide(Y[0] - p->y, p->sin, scale);
	yr = h - MultiplyThenDivide(Y[0] - p->y, p->cos, scale) - MultiplyThenDivide(X[0] - p->x, p->sin, scale);
	PlotPoint16(xr, yr, CLR16_GREEN);

	x = w + MultiplyThenDivide(X[1] - p->x, p->cos, scale) - MultiplyThenDivide(Y[1] - p->y, p->sin, scale);
	y = h - MultiplyThenDivide(Y[1] - p->y, p->cos, scale) - MultiplyThenDivide(X[1] - p->x, p->sin, scale);
	DrawLineJustTo16(x, y, CLR16_GREEN);

	x = w + MultiplyThenDivide(X[2] - p->x, p->cos, scale) - MultiplyThenDivide(Y[2] - p->y, p->sin, scale);
	y = h - MultiplyThenDivide(Y[2] - p->y, p->cos, scale) - MultiplyThenDivide(X[2] - p->x, p->sin, scale);
	DrawLineJustTo16(x, y, CLR16_GREEN);

	x = w + MultiplyThenDivide(X[3] - p->x, p->cos, scale) - MultiplyThenDivide(Y[3] - p->y, p->sin, scale);
	y = h - MultiplyThenDivide(Y[3] - p->y, p->cos, scale) - MultiplyThenDivide(X[3] - p->x, p->sin, scale);
	DrawLineJustTo16(x, y, CLR16_GREEN);

	DrawLineJustTo16(xr, yr, CLR16_GREEN);
/*
	scale = SCALE_1_0 + ((p->z - Z[0]) >> SCALE_SHIFT);

	xr = w + MultiplyThenDivide(X[0] - p->x, p->cos, scale) - MultiplyThenDivide(Y[0] - p->y, p->sin, scale);
	yr = h - MultiplyThenDivide(Y[0] - p->y, p->cos, scale) - MultiplyThenDivide(X[0] - p->x, p->sin, scale);
	PlotPoint16(xr, yr, CLR16_GREEN);

	scale = SCALE_1_0 + ((p->z - Z[1]) >> SCALE_SHIFT);

	x = w + MultiplyThenDivide(X[1] - p->x, p->cos, scale) - MultiplyThenDivide(Y[1] - p->y, p->sin, scale);
	y = h - MultiplyThenDivide(Y[1] - p->y, p->cos, scale) - MultiplyThenDivide(X[1] - p->x, p->sin, scale);
	DrawLineJustTo16(x, y, CLR16_GREEN);

	scale = SCALE_1_0 + ((p->z - Z[2]) >> SCALE_SHIFT);

	x = w + MultiplyThenDivide(X[2] - p->x, p->cos, scale) - MultiplyThenDivide(Y[2] - p->y, p->sin, scale);
	y = h - MultiplyThenDivide(Y[2] - p->y, p->cos, scale) - MultiplyThenDivide(X[2] - p->x, p->sin, scale);
	DrawLineJustTo16(x, y, CLR16_GREEN);

	scale = SCALE_1_0 + ((p->z - Z[3]) >> SCALE_SHIFT);

	x = w + MultiplyThenDivide(X[3] - p->x, p->cos, scale) - MultiplyThenDivide(Y[3] - p->y, p->sin, scale);
	y = h - MultiplyThenDivide(Y[3] - p->y, p->cos, scale) - MultiplyThenDivide(X[3] - p->x, p->sin, scale);
	DrawLineJustTo16(x, y, CLR16_GREEN);

	DrawLineJustTo16(xr, yr, CLR16_GREEN);
*/
	DrawLineFromTo16(w - 12, h - 12, w + 12, h + 12, CLR16_WHITE);
	DrawLineFromTo16(w - 12, h + 12, w + 12, h - 12, CLR16_WHITE);
}
