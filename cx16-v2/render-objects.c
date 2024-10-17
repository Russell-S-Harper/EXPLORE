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
	int16_t h, w, scale, dx, dy, dz, last_dz;
	POINT *point, *p1, *p2;
	VERTEX *vertex;
	SEGMENT *segment;
	VEHICLE *vehicle;

	if (!working)
		working = malloc(sizeof(POINT) * (max_vertices + 1));

	xm = GetXMAddress(arena_data, arena_index);
	vertex = (VERTEX *)(xm + X_LIMIT * Y_LIMIT * sizeof(int16_t));
	segment = (SEGMENT *)(xm + X_LIMIT * Y_LIMIT * sizeof(int16_t) + sizeof(VERTEX) * (max_vertices + 1));
	vehicle = vehicles + vehicle_index;

	h = 3 * display_height >> 2;
	w = display_width >> 1;

	for (point = working, last_dz = INT16_MIN; vertex->z >= 0; ++vertex, ++point) {
		dx = vertex->x - vehicle->x;
		dy = vertex->y - vehicle->y;
		dz = vertex->z - vehicle->z;
		if (last_dz != dz) {
			last_dz = dz;
			scale = MultiplyThenDivide(dz, SCALE_M1, SCALE_M2) + SCALE_B;
		}
		point->x = w + ((MultiplyThenDivide(dx, vehicle->cos, scale) - MultiplyThenDivide(dy, vehicle->sin, scale)) >> XY_SHIFT);
		point->y = h + ((MultiplyThenDivide(dy, vehicle->cos, -scale) + MultiplyThenDivide(dx, vehicle->sin, -scale)) >> XY_SHIFT);
	}

	for (; segment->index_from != segment->index_to; ++segment) {
		p1 = working + segment->index_from;
		p2 = working + segment->index_to;
		DrawLineFromTo16(p1->x, p1->y, p2->x, p2->y, CLR16_ORANGE);
	}

	DrawLineFromTo16(w - 12, h - 12, w + 12, h + 12, CLR16_WHITE);
	DrawLineFromTo16(w - 12, h + 12, w + 12, h - 12, CLR16_WHITE);
}
