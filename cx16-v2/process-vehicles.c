/*
    Contents: "process vehicles" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include <string.h>
#include "explore.h"

#define ANGLE_DELTA_SHIFT	2	/* shift left */
#define XY_MISSILE_DELTA_SHIFT	3	/* shift right */
#define XY_AIR_DELTA_SHIFT	6	/* shift right */
#define XY_GND_DELTA_SHIFT	7	/* shift right */
#define Z_DELTA_SHIFT		5	/* shift left */
#define XY_COLLISION_SHIFT	2	/* shift right */

/* For ground vehicles: < 0 reverse, 0 neutral, > 0 forward */
#define MAX_GEAR		2
#define MIN_GEAR		-1

#define ARENA_INDEX(x, y)	((((y) >> 6) & 0xF0) + ((x) >> 10))

/* Process every vehicle */
void ProcessVehicles(void)
{
	VEHICLE *player, *missile;
	int8_t shift;
	int16_t *arena, i, j, x, y, z, dx, dy;

	/* Will need this */
	arena = GetXMAddress(g_arena_data, g_arena_index);

	/* Process each player */
	for (i = 0, player = g_vehicles; i < PLAYER_COUNT; ++i, ++player) {
		/* Ignore if not active */
		if (!player->active)
			continue;

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
			shift = XY_AIR_DELTA_SHIFT - player->gear;
			x += player->sin >> shift;
			y += player->cos >> shift;
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

		/* Add any missiles */
		if (player->fire) {
			for (j = PLAYER_COUNT; j < VEHICLE_COUNT; ++j) {
				if (!g_vehicles[j].active) {
					memcpy(g_vehicles + j, player, sizeof(VEHICLE));
					g_vehicles[j].appearance[APP_PRM] = player->appearance[APP_MSS];
					break;
				}
			}
			player->fire = false;
			player->loading = MSS_LOADING_COUNTER;
		} else if (player->loading)
			--player->loading;
	}
	/* Process each missile */
	for (missile = g_vehicles + i; i < VEHICLE_COUNT; ++i, ++missile) {
		/* Deactivate if exploded */
		if (missile->exploding)
			missile->active = false;

		/* Ignore if not active */
		if (!missile->active)
			continue;

		/* Explode if a wall was created underneath */
		z = arena[ARENA_INDEX(missile->x, missile->y)];
		if (missile->z < z)
			missile->exploding = true;

		/* Countdown? */
		if (missile->countdown && !--missile->countdown)
			missile->exploding = true;

		/* Will need these */
		x = missile->x;
		y = missile->y;
		z = missile->z;

		/* Process angle */
		if (missile->angle_delta) {
			missile->angle += missile->angle_delta << ANGLE_DELTA_SHIFT;
			missile->sin = Sin(missile->angle);
			missile->cos = Cos(missile->angle);
			missile->angle_delta = 0;
		}

		/* Process XY movement */
		x += missile->sin >> XY_MISSILE_DELTA_SHIFT;
		y += missile->cos >> XY_MISSILE_DELTA_SHIFT;

		/* Keep in bounds */
		x = Minimum(Maximum(x, MIN_XYZ), MAX_XYZ);
		y = Minimum(Maximum(y, MIN_XYZ), MAX_XYZ);

		/* Process Z movement */
		if (missile->z_delta) {
			z += missile->z_delta << Z_DELTA_SHIFT;
			z = Minimum(Maximum(z, MIN_XYZ), MAX_XYZ);
			missile->z_delta = 0;
		}

		/* Try moving to new XY */
		if (z >= arena[ARENA_INDEX(x, y)]) {
			missile->x = x;
			missile->y = y;
		}
		/* Can't move? Try sliding along the wall */
		else if (z >= arena[ARENA_INDEX(missile->x, y)])
				missile->y = y;
		else if (z >= arena[ARENA_INDEX(x, missile->y)])
				missile->x = x;
		/* Still can't move? Explode! */
		else
			missile->exploding = true;
		/* Finalize Z */
		if (z >= arena[ARENA_INDEX(missile->x, missile->y)])
			missile->z = z;

		/* Check for player collisions */
		for (j = 0; j < PLAYER_COUNT; ++j) {
			player = g_vehicles + j;
			if (missile->identifier != player->identifier && player->active && abs(missile->z - player->z) <= VEHICLE_Z_TOL) {
				dx = abs(missile->x - player->x) >> XY_COLLISION_SHIFT;
				dy = abs(missile->y - player->y) >> XY_COLLISION_SHIFT;
				if (dx <= MSS_XY_TOL && dy <= MSS_XY_TOL && g_squares[dx] + g_squares[dy] <= MSS_DST_TOL_SQR) {
					missile->exploding = true;
					player->health -= missile->damage;
					if (player->health <= 0) {
						AdvancePlayer(g_vehicles + missile->identifier);
						if (!AdvancePlayer(player))
							player->active = false;
					}
				}
			}
		}

		/* Animate explosions */
		if (missile->exploding) {
			missile->appearance[APP_PRM] = g_exploding_prm;
			missile->appearance[APP_AUX] = g_exploding_aux;
		}
	}
}
