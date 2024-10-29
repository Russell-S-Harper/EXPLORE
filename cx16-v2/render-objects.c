/*
    Contents: "render objects" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include "explore.h"

#define ARENA_SCALE_M1		-3
#define ARENA_SCALE_M2		32
#define ARENA_SCALE_B		4608
#define ARENA_XY_SHIFT		5

/* Revise if VEHICLE_HGTS or VEHICLE_DIRS is changed */
#define VEHICLE_POINTS(dz, da)	((((((dz) + 16384) >> 7) & 0xE0) + (((da) >> 4) & 0x1F)) * sizeof(OFFSET) * max_vehicle_vertices)

static int CompareIndexedVehiclesByHeight(void *p, void *q);

/* Render all the objects */
void RenderObjects(void)
{
	static POINT *working = NULL;
	static int16_t h, w;
	static unsigned char *indices;
	char *xm;
	uint8_t color;
	int16_t i, dx, dy, dz, da, screen_x, screen_y, last_dz, scale;
	POINT *P, *p1, *p2;
	OFFSET *O, *o1, *o2;
	VERTEX *V;
	SEGMENT *S;
	VEHICLE *focus, *vehicle;

	if (!working) {
		working = malloc(sizeof(POINT) * (max_arena_vertices + 1));
		h = 3 * display_height >> 2;
		w = display_width >> 1;
		indices = malloc(sizeof(unsigned char) * VEHICLE_COUNT);
		for (i = 0; i < VEHICLE_COUNT; ++i)
			indices[i] = i;
	}

	focus = vehicles + vehicle_index;
	xm = GetXMAddress(arena_data, arena_index);
	V = (VERTEX *)(xm + ARENA_X_LIMIT * ARENA_Y_LIMIT * sizeof(int16_t));
	S = (SEGMENT *)(xm + ARENA_X_LIMIT * ARENA_Y_LIMIT * sizeof(int16_t) + sizeof(VERTEX) * (max_arena_vertices + 1));

	for (P = working, last_dz = INT16_MIN; V->z >= 0; ++V, ++P) {
		dx = V->x - focus->x;
		dy = V->y - focus->y;
		dz = V->z - focus->z;
		if (last_dz != dz) {
			last_dz = dz;
			scale = MultiplyThenDivide(dz, ARENA_SCALE_M1, ARENA_SCALE_M2) + ARENA_SCALE_B;
		}
		P->x = w + ((MultiplyThenDivide(dx, focus->cos, scale) - MultiplyThenDivide(dy, focus->sin, scale)) >> ARENA_XY_SHIFT);
		P->y = h + ((MultiplyThenDivide(dx, focus->sin, -scale) + MultiplyThenDivide(dy, focus->cos, -scale)) >> ARENA_XY_SHIFT);
	}

	for (; S->index_from != S->index_to; ++S) {
		p1 = working + S->index_from;
		p2 = working + S->index_to;
		DrawLineFromTo16(p1->x, p1->y, p2->x, p2->y, CLR16_DARKGRAY);
	}

	/* Sort to draw from the bottom up */
	qsort(indices, VEHICLE_COUNT, sizeof(unsigned char), CompareIndexedVehiclesByHeight);

	for (i = 0, last_dz = INT16_MIN; i < VEHICLE_COUNT; ++i) {
		vehicle = vehicles + indices[i];
		if (vehicle == focus) {
			last_dz = dz = da = 0;
			scale = ARENA_SCALE_B;
			color = CLR16_WHITE;
			screen_x = w;
			screen_y = h;
		} else {
			dx = vehicle->x - focus->x;
			dy = vehicle->y - focus->y;
			dz = vehicle->z - focus->z;
			da = vehicle->angle - focus->angle;
			if (last_dz != dz) {
				last_dz = dz;
				scale = MultiplyThenDivide(dz, ARENA_SCALE_M1, ARENA_SCALE_M2) + ARENA_SCALE_B;
				if (dz < -VEHICLE_Z_TOLERANCE)
					color = CLR16_ORANGE;
				else if (dz > VEHICLE_Z_TOLERANCE)
					color = CLR16_LIGHTBLUE;
				else
					color = CLR16_WHITE;
			}
			screen_x = w + ((MultiplyThenDivide(dx, focus->cos, scale) - MultiplyThenDivide(dy, focus->sin, scale)) >> ARENA_XY_SHIFT);
			screen_y = h + ((MultiplyThenDivide(dx, focus->sin, -scale) + MultiplyThenDivide(dy, focus->cos, -scale)) >> ARENA_XY_SHIFT);
		}

		xm = GetXMAddressInitial(vehicle->appearance);
		S = (SEGMENT *)xm;
		O = (OFFSET *)(xm + sizeof(SEGMENT) * (max_vehicle_segments + 1) + VEHICLE_POINTS(dz, da));

		for (; S->index_from != S->index_to; ++S) {
			o1 = O + S->index_from;
			o2 = O + S->index_to;
			DrawLineFromTo16(screen_x + o1->x, screen_y + o1->y, screen_x + o2->x, screen_y + o2->y, color);
		}
	}
}

static int CompareIndexedVehiclesByHeight(void *p, void *q)
{
	int16_t result;
	const VEHICLE *a = vehicles + *(unsigned char *)p, *b = vehicles + *(unsigned char *)q;

	result = a->z - b->z;
	if (!result)
		result = a->x - b->x;
	if (!result)
		result = a->y - b->y;

	return (int)result;
}
