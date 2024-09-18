/*
    Contents: "render objects" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include "explore.h"

#define SCALE_SHIFT	4
#define SCREEN_SHIFT	5

/* Render all the objects */
void RenderObjects(PLAYER_STATUS *p)
{
	static POINT *points;
	char *v = GetXMAddress(arena_data, arena_index);
	int16_t h, w, scale;
	POINT *P, *p1, *p2;
	VERTEX *V = (VERTEX *)(v + X_LIMIT * Y_LIMIT * sizeof(int16_t));
	SEGMENT *S = (SEGMENT *)(v + X_LIMIT * Y_LIMIT * sizeof(int16_t) + sizeof(VERTEX) * (max_vertices + 1));

	if (!points)
		points = malloc(sizeof(POINT) * (max_vertices + 1));

	h = 3 * display_height >> 2;
	w = display_width >> 1;

	for (P = points; V->z >= 0; ++V, ++P) {
		scale = SCALE_1_0 - ((V->z - p->z) >> SCALE_SHIFT);
		P->x = w + ((MultiplyThenDivide(V->x - p->x, p->cos, scale) - MultiplyThenDivide(V->y - p->y, p->sin, scale)) >> SCREEN_SHIFT);
		P->y = h + ((MultiplyThenDivide(V->y - p->y, p->cos, -scale) + MultiplyThenDivide(V->x - p->x, p->sin, -scale)) >> SCREEN_SHIFT);
	}

	for (; S->index_from != S->index_to; ++S) {
		p1 = points + S->index_from;
		p2 = points + S->index_to;
		DrawLineFromTo16(p1->x, p1->y, p2->x, p2->y, CLR16_GREEN);
	}

	DrawLineFromTo16(w - 12, h - 12, w + 12, h + 12, CLR16_WHITE);
	DrawLineFromTo16(w - 12, h + 12, w + 12, h - 12, CLR16_WHITE);
}
