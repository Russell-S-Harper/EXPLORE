/*
    Contents: "render objects" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include "explore.h"

/* Render all the objects */
void RenderObjects(void)
{
	static unsigned char color;
	static int16_t angle;
	int16_t s, c;
	int16_t x, y, x1, y1, x2, y2, x3, y3;

	if (!color)
		++color;

	DrawLineFromTo16(0, display_height / 2, display_width - 1, display_height / 2, WHITE);

	x = 53;
	y = 159;

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

	PlotPoint16(x1, y1, color);
	DrawLineJustTo16(x2, y2, color);
	DrawLineJustTo16(x3, y3, color);
	DrawLineJustTo16(x1, y1, color);

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

	PlotPoint16(x1, y1, 16 - color);
	DrawLineJustTo16(x2, y2, 16 - color);
	DrawLineJustTo16(x3, y3, 16 - color);
	DrawLineJustTo16(x1, y1, 16 - color);

	angle = (angle + 8) & (FULL_CIRC - 1);
	color = (angle >> 5) & (COLORS - 1);
}
