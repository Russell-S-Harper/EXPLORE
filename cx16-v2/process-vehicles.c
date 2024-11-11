/*
    Contents: "process vehicles" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include "explore.h"

#define ANGLE_DELTA_SHIFT	1
#define XY_MISSILE_DELTA_SHIFT	3
#define XY_AIR_DELTA_SHIFT	6
#define XY_GND_DELTA_SHIFT	7
#define Z_DELTA_SHIFT		6

#define MAX_GEAR		2
#define MIN_GEAR		-1

#define ARENA_INDEX(x, y)	((((y) >> 6) & 0xF0) + ((x) >> 10))

/* Process every vehicle */
void ProcessVehicles(void)
{
	VEHICLE *vehicle, *working;
	int8_t shift;
	int16_t *arena, i, j, x, y, z;

	/* Will need this */
	arena = GetXMAddress(g_arena_data, g_arena_index);

	/* Process each player */
	for (i = 0, vehicle = g_vehicles; i < PLAYER_COUNT; ++i, ++vehicle) {
		/* Ignore if not active */
		if (!vehicle->active)
			continue;

		/* Get player input, NPCs will get inputs during idle times */
		if (i == PLAYER_INDEX)
			GetPlayerInput(vehicle);

		/* Important: "bump" if a wall was created or removed underneath */
		z = arena[ARENA_INDEX(vehicle->x, vehicle->y)];
		if (vehicle->z < z || !vehicle->airborne)
			vehicle->z = z;

		/* Will need these */
		x = vehicle->x;
		y = vehicle->y;
		z = vehicle->z;

		/* Process angle */
		if (vehicle->angle_delta) {
			vehicle->angle += vehicle->angle_delta << ANGLE_DELTA_SHIFT;
			vehicle->sin = Sin(vehicle->angle);
			vehicle->cos = Cos(vehicle->angle);
			vehicle->angle_delta = 0;
		}

		/* Process XY movement */
		if (vehicle->airborne) {
			shift = XY_AIR_DELTA_SHIFT - vehicle->gear;
			x += vehicle->sin >> shift;
			y += vehicle->cos >> shift;
		} else {
			vehicle->gear = Minimum(Maximum(vehicle->gear, MIN_GEAR), MAX_GEAR);
			switch (vehicle->gear) {
				case MIN_GEAR:
					x -= vehicle->sin >> XY_GND_DELTA_SHIFT;
					y -= vehicle->cos >> XY_GND_DELTA_SHIFT;
					break;
				case 0:
					/* 0: neutral */
					break;
				default:
					shift = XY_GND_DELTA_SHIFT - vehicle->gear;
					x += vehicle->sin >> shift;
					y += vehicle->cos >> shift;
					break;
			}
		}
		/* Keep in bounds */
		x = Minimum(Maximum(x, MIN_XYZ), MAX_XYZ);
		y = Minimum(Maximum(y, MIN_XYZ), MAX_XYZ);

		/* Process Z movement */
		if (vehicle->z_delta) {
			z += vehicle->z_delta << Z_DELTA_SHIFT;
			z = Minimum(Maximum(z, MIN_XYZ), MAX_XYZ);
			vehicle->z_delta = 0;
		}

		/* Try moving to new XY */
		if (z >= arena[ARENA_INDEX(x, y)]) {
			vehicle->x = x;
			vehicle->y = y;
		}
		/* Can't move? Try sliding along the wall */
		else if (z >= arena[ARENA_INDEX(vehicle->x, y)])
			vehicle->y = y;
		else if (z >= arena[ARENA_INDEX(x, vehicle->y)])
			vehicle->x = x;

		/* Finalize Z */
		if (z >= arena[ARENA_INDEX(vehicle->x, vehicle->y)])
			vehicle->z = z;

		/* Add any missiles */
		if (vehicle->fire) {
			for (j = PLAYER_COUNT; j < VEHICLE_COUNT; ++j) {
				if (!g_vehicles[j].active) {
					working = g_vehicles + j;
					working->active = true;
					working->exploding = false;
					working->countdown = MSS_COUNTDOWN_COUNTER;
					working->angle = vehicle->angle;
					working->sin = vehicle->sin;
					working->cos = vehicle->cos;
					working->x = vehicle->x;
					working->y = vehicle->y;
					working->z = vehicle->z;
					working->appearance[APP_PRM] = g_vehicle_data[6];
					working->appearance[APP_AUX] = 0;
					break;
				}
			}
			vehicle->fire = false;
			vehicle->loading = MSS_LOADING_COUNTER;
		} else if (vehicle->loading)
			--vehicle->loading;
	}
	/* Process each missile */
	for (; i < VEHICLE_COUNT; ++i, ++vehicle) {
		/* Deactivate if exploded */
		if (vehicle->exploding)
			vehicle->active = false;

		/* Ignore if not active */
		if (!vehicle->active)
			continue;

		/* Explode if a wall was created underneath */
		z = arena[ARENA_INDEX(vehicle->x, vehicle->y)];
		if (vehicle->z < z)
			vehicle->exploding = true;

		/* Countdown? */
		if (vehicle->countdown && !--vehicle->countdown)
			vehicle->exploding = true;

		/* Will need these */
		x = vehicle->x;
		y = vehicle->y;
		z = vehicle->z;

		/* Process angle */
		if (vehicle->angle_delta) {
			vehicle->angle += vehicle->angle_delta << ANGLE_DELTA_SHIFT;
			vehicle->sin = Sin(vehicle->angle);
			vehicle->cos = Cos(vehicle->angle);
			vehicle->angle_delta = 0;
		}

		/* Process XY movement */
		x += vehicle->sin >> XY_MISSILE_DELTA_SHIFT;
		y += vehicle->cos >> XY_MISSILE_DELTA_SHIFT;

		/* Keep in bounds */
		x = Minimum(Maximum(x, MIN_XYZ), MAX_XYZ);
		y = Minimum(Maximum(y, MIN_XYZ), MAX_XYZ);

		/* Process Z movement */
		if (vehicle->z_delta) {
			z += vehicle->z_delta << Z_DELTA_SHIFT;
			z = Minimum(Maximum(z, MIN_XYZ), MAX_XYZ);
			vehicle->z_delta = 0;
		}

		/* Try moving to new XY */
		if (z >= arena[ARENA_INDEX(x, y)]) {
			vehicle->x = x;
			vehicle->y = y;
		}
		/* Can't move? Try sliding along the wall */
		else if (z >= arena[ARENA_INDEX(vehicle->x, y)])
				vehicle->y = y;
		else if (z >= arena[ARENA_INDEX(x, vehicle->y)])
				vehicle->x = x;
		/* Still can't move? Explode! */
		else
			vehicle->exploding = true;
		/* Finalize Z */
		if (z >= arena[ARENA_INDEX(vehicle->x, vehicle->y)])
			vehicle->z = z;

		/* Animate explosions */
		if (vehicle->exploding) {
			vehicle->appearance[APP_PRM] = g_exploding_prm;
			vehicle->appearance[APP_AUX] = g_exploding_aux;
		}
	}
}
