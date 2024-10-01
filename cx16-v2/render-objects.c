/*
    Contents: "render objects" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include "explore.h"

#define SCALE_M1	-3
#define SCALE_M2	32
#define SCALE_B		4608
#define XY_SHIFT	5

/* Render all the objects */
void RenderObjects(void)
{
	static POINT *working;
	char *xm;
	int16_t h, w, scale;
	POINT *point, *p1, *p2;
	VERTEX *vertex;
	SEGMENT *segment;
	VEHICLE *vehicle = vehicles + vehicle_index;

	if (!working)
		working = malloc(sizeof(POINT) * (max_vertices + 1));

	xm = GetXMAddress(arena_data, arena_index);
	vertex = (VERTEX *)(xm + X_LIMIT * Y_LIMIT * sizeof(int16_t));
	segment = (SEGMENT *)(xm + X_LIMIT * Y_LIMIT * sizeof(int16_t) + sizeof(VERTEX) * (max_vertices + 1));

	h = 3 * display_height >> 2;
	w = display_width >> 1;

	for (point = working; vertex->z >= 0; ++vertex, ++point) {
		scale = MultiplyThenDivide(vertex->z - vehicle->z, SCALE_M1, SCALE_M2) + SCALE_B;
		point->x = w + ((MultiplyThenDivide(vertex->x - vehicle->x, vehicle->cos, scale) - MultiplyThenDivide(vertex->y - vehicle->y, vehicle->sin, scale)) >> XY_SHIFT);
		point->y = h + ((MultiplyThenDivide(vertex->y - vehicle->y, vehicle->cos, -scale) + MultiplyThenDivide(vertex->x - vehicle->x, vehicle->sin, -scale)) >> XY_SHIFT);
	}

	for (; segment->index_from != segment->index_to; ++segment) {
		p1 = working + segment->index_from;
		p2 = working + segment->index_to;
		DrawLineFromTo16(p1->x, p1->y, p2->x, p2->y, CLR16_GREEN);
	}

	DrawLineFromTo16(w - 12, h - 12, w + 12, h + 12, CLR16_WHITE);
	DrawLineFromTo16(w - 12, h + 12, w + 12, h - 12, CLR16_WHITE);
}
