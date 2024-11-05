/*
    Contents: "process vehicles" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include "explore.h"

#define ANGLE_DELTA_SHIFT	1
#define XY_AIR_DELTA_SHIFT	4
#define XY_GND_DELTA_SHIFT	7
#define Z_DELTA_SHIFT		6

#define MAX_GEAR		2
#define MIN_GEAR		-1

#define ARENA_INDEX(x, y)	((((y) >> 6) & 0xF0) + ((x) >> 10))

/* Process every vehicle */
void ProcessVehicles(void)
{
	VEHICLE *player;
	int8_t shift;
	int16_t *arena, i, x, y, z;

	/* Will need this */
	arena = GetXMAddress(arena_data, arena_index);

	/* Process each player */
	for (i = 0, player = vehicles; i < VEHICLE_COUNT; ++i, ++player) {
		/* Get player input, NPCs will get inputs during idle times */
		if (i == PLAYER_INDEX)
			GetPlayerInput(player);

		/* Important: "bump" if a wall was created or removed underneath */
		z = arena[ARENA_INDEX(player->x, player->y)];
		if (player->z < z || !player->airborne)
			player->z = z;

		/* Will need these */
		x = player->x;
		y = player->y;
		z = player->z;

		/* Process angle */
		if (player->angle_delta) {
			player->angle += player->angle_delta << ANGLE_DELTA_SHIFT;
			player->sin = Sin(player->angle);
			player->cos = Cos(player->angle);
			player->angle_delta = 0;
		}

		/* Process XY movement */
		if (player->airborne) {
			x += player->sin >> XY_AIR_DELTA_SHIFT;
			y += player->cos >> XY_AIR_DELTA_SHIFT;
		} else {
			player->gear = Minimum(Maximum(player->gear, MIN_GEAR), MAX_GEAR);
			switch (player->gear) {
				case MIN_GEAR:
					x -= player->sin >> XY_GND_DELTA_SHIFT;
					y -= player->cos >> XY_GND_DELTA_SHIFT;
					break;
				case 0:
					/* 0: neutral */
					break;
				default:
					shift = XY_GND_DELTA_SHIFT - player->gear;
					x += player->sin >> shift;
					y += player->cos >> shift;
					break;
			}
		}
		/* Keep in bounds */
		x = Minimum(Maximum(x, MIN_XYZ), MAX_XYZ);
		y = Minimum(Maximum(y, MIN_XYZ), MAX_XYZ);

		/* Process Z movement */
		if (player->z_delta) {
			z += player->z_delta << Z_DELTA_SHIFT;
			z = Minimum(Maximum(z, MIN_XYZ), MAX_XYZ);
			player->z_delta = 0;
		}

		/* Try moving to new XY */
		if (z >= arena[ARENA_INDEX(x, y)]) {
			player->x = x;
			player->y = y;
		}
		/* Can't move? Try sliding along the wall */
		else if (z >= arena[ARENA_INDEX(player->x, y)])
			player->y = y;
		else if (z >= arena[ARENA_INDEX(x, player->y)])
			player->x = x;

		/* Finalize Z */
		if (z >= arena[ARENA_INDEX(player->x, player->y)])
			player->z = z;
	}
	/* TODO: process every missile */
}
