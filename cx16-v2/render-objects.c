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
#define VEHICLE_POINTS(dz, da)	((((((dz) + 16384) >> 7) & 0xE0) + (((da) >> 4) & 0x1F)) * sizeof(OFFSET) * g_max_vehicle_vertices)

/* Optimized qsort */
static void SortIndicesByVehicleHeight(unsigned char *indices, char lo, char hi);

/* Render all the objects */
void RenderObjects(void)
{
	static POINT *s_points = NULL;
	static int16_t s_w, s_h;
	static unsigned char *s_indices;
	char *xm;
	uint8_t color;
	int16_t i, j, count, dx, dy, dz, da, screen_x, screen_y, last_dz, scale;
	POINT *P, *p1, *p2;
	OFFSET *O, *o1, *o2;
	VERTEX *V;
	SEGMENT *S;
	VEHICLE *focus, *vehicle;

	if (!s_points) {
		s_points = malloc(sizeof(POINT) * (g_max_arena_vertices + 1));
		s_indices = malloc(sizeof(unsigned char) * VEHICLE_COUNT);
		s_w = g_display_width >> 1;
		s_h = 3 * g_display_height >> 2;
	}

	/* Get the focus vehicle */
	focus = g_vehicles + g_vehicle_index;

	/* Render the arena */
	xm = GetXMAddress(g_arena_data, g_arena_index);
	V = (VERTEX *)(xm + ARENA_X_LIMIT * ARENA_Y_LIMIT * sizeof(int16_t));
	S = (SEGMENT *)(xm + ARENA_X_LIMIT * ARENA_Y_LIMIT * sizeof(int16_t) + sizeof(VERTEX) * (g_max_arena_vertices + 1));
	for (P = s_points, last_dz = INT16_MIN; V->z >= 0; ++V, ++P) {
		dx = V->x - focus->x;
		dy = V->y - focus->y;
		dz = V->z - focus->z;
		if (last_dz != dz) {
			last_dz = dz;
			scale = MultiplyThenDivide(dz, ARENA_SCALE_M1, ARENA_SCALE_M2) + ARENA_SCALE_B;
		}
		P->x = s_w + ((MultiplyThenDivide(dx, focus->cos, scale) - MultiplyThenDivide(dy, focus->sin, scale)) >> ARENA_XY_SHIFT);
		P->y = s_h + ((MultiplyThenDivide(dx, focus->sin, -scale) + MultiplyThenDivide(dy, focus->cos, -scale)) >> ARENA_XY_SHIFT);
	}
	for (; S->index_from != S->index_to; ++S) {
		p1 = s_points + S->index_from;
		p2 = s_points + S->index_to;
		DrawLineFromTo16(p1->x, p1->y, p2->x, p2->y, CLR16_DARKGRAY);
	}

	/* Pull out the active vehicles and sort to render from the bottom up */
	for (i = count = 0; i < VEHICLE_COUNT; ++i) {
		if (g_vehicles[i].active) {
			s_indices[count] = i;
			++count;
		}
	}
	SortIndicesByVehicleHeight(s_indices, 0, count - 1);

	/* Render the vehicles */
	for (i = 0, last_dz = INT16_MIN; i < count; ++i) {
		/* Get the vehicle */
		vehicle = g_vehicles + s_indices[i];
		/* Special handling for focus */
		if (vehicle == focus) {
			last_dz = dz = da = 0;
			scale = ARENA_SCALE_B;
			color = CLR16_WHITE;
			screen_x = s_w;
			screen_y = s_h;
		/* Else regular vehicle */
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
			screen_x = s_w + ((MultiplyThenDivide(dx, focus->cos, scale) - MultiplyThenDivide(dy, focus->sin, scale)) >> ARENA_XY_SHIFT);
			screen_y = s_h + ((MultiplyThenDivide(dx, focus->sin, -scale) + MultiplyThenDivide(dy, focus->cos, -scale)) >> ARENA_XY_SHIFT);
		}
		/* Render the vehicle */
		for (j = 0; j < sizeof(vehicle->appearance) / sizeof(XM_HANDLE); ++j) {
			if (vehicle->appearance[j]) {
				xm = GetXMAddressInitial(vehicle->appearance[j]);
				S = (SEGMENT *)xm;
				O = (OFFSET *)(xm + sizeof(SEGMENT) * (g_max_vehicle_segments + 1) + VEHICLE_POINTS(dz, da));
				for (; S->index_from != S->index_to; ++S) {
					o1 = O + S->index_from;
					o2 = O + S->index_to;
					DrawLineFromTo16(screen_x + o1->x, screen_y + o1->y, screen_x + o2->x, screen_y + o2->y, color);
				}
			}
		}
	}
}

/* Optimized code from qsort.c for this specific case, otherwise qsort becomes a bottleneck! */
static void SortIndicesByVehicleHeight(unsigned char *indices, char lo, char hi)
{
	unsigned char t;
	char i, j;
	int16_t z;

	/* Sort indices by vehicle height */
	while (hi > lo) {
		i = lo + 1;
		j = hi;
		while (i <= j) {
			z = g_vehicles[indices[lo]].z;
			while (i <= j && z >= g_vehicles[indices[i]].z)
				++i;
			while (i <= j && z < g_vehicles[indices[j]].z)
				--j;
			if (i <= j) {
				t = indices[i];
				indices[i] = indices[j];
				indices[j] = t;
				++i;
				--j;
			}
		}
		if (j != lo) {
			t = indices[j];
			indices[j] = indices[lo];
			indices[lo] = t;
		}
		/* More commonly expressed as j > (hi + lo) / 2, meaning is j past the midpoint? */
		if (j - lo > hi - j) {
			SortIndicesByVehicleHeight(indices, j + 1, hi);
			hi = j - 1;
		} else {
			SortIndicesByVehicleHeight(indices, lo, j - 1);
			lo = j + 1;
		}
	}
}
