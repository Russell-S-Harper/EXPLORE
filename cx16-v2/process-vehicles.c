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
	uint8_t i, j;
	int16_t *arena, x, y, z, dx, dy;

	/* Will need this */
	arena = GetXMAddress(g_arena_data, g_arena_index);

	/* Process each player */
	for (i = PLAYER_INDEX, player = g_vehicles + PLAYER_INDEX; i < PLAYER_LIMIT; ++i, ++player) {

		/* Get player input regardless, NPCs will get inputs during idle times */
		if (i == HUMAN_ID)
			GetPlayerInput(player);

		/* Ignore if not active */
		if (!player->active)
			continue;

		/* Important: "bump" if a wall was created or removed underneath */
		z = arena[ARENA_INDEX(player->x, player->y)];
		if (player->z < z || !player->airborne)
			player->z = z;

		/* Will need these */
		x = player->x;
		y = player->y;
		z = player->z;

		/* Process angle */
		if (player->a_delta) {
			player->angle += player->a_delta << ANGLE_DELTA_SHIFT;
			player->sin = Sin(player->angle);
			player->cos = Cos(player->angle);
			player->a_delta = 0;
		}

		/* Process XY movement */
		if (player->airborne) {
			shift = XY_AIR_DELTA_SHIFT - player->gear;
			x += player->sin >> shift;
			y += player->cos >> shift;
		} else {
			player->gear = Min16(Max16(player->gear, MIN_GEAR), MAX_GEAR);
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
		x = Min16(Max16(x, MIN_XYZ), MAX_XYZ);
		y = Min16(Max16(y, MIN_XYZ), MAX_XYZ);

		/* Process Z movement */
		if (player->z_delta) {
			z += player->z_delta << Z_DELTA_SHIFT;
			if (z < MIN_XYZ) {
				z = MIN_XYZ;
				ReportToAI(player, AIE_REACHED_TOP, 0);
			} else if (z > MAX_XYZ) {
				z = MAX_XYZ;
				ReportToAI(player, AIE_REACHED_BOTTOM, 0);
			}
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
		else
			/* Stuck! */
			ReportToAI(player, AIE_STUCK_PLAYER, arena[ARENA_INDEX(x, y)]);

		/* Finalize Z */
		if (z >= arena[ARENA_INDEX(player->x, player->y)])
			player->z = z;

		/* Add any missiles */
		if (player->firing) {
			for (j = MISSILE_INDEX, missile = g_vehicles + MISSILE_INDEX; j < MISSILE_LIMIT; ++j, ++missile) {
				if (!missile->active) {
					/* Set up the missile as a clone of the player */
					memcpy(missile, player, sizeof(VEHICLE));
					missile->appearance[APP_PRM] = player->appearance[APP_MSS];
					missile->appearance[APP_AUX] = 0;
					missile->hit_cd = 0;
					AddSound(MSS_FIRING);
					break;
				}
			}
			player->firing = false;
			player->loading_cd = MSS_LOADING;
		} else if (player->loading_cd)
			--player->loading_cd;

		/* Hit counter */
		if (player->hit_cd)
			--player->hit_cd;
	}
	/* Process each missile */
	for (i = MISSILE_INDEX, missile = g_vehicles + MISSILE_INDEX; i < MISSILE_LIMIT; ++i, ++missile) {
		/* Deactivate if exploded */
		if (missile->exploding)
			missile->active = false;

		/* Ignore if not active */
		if (!missile->active)
			continue;

		/* Explode if a wall was created above */
		z = arena[ARENA_INDEX(missile->x, missile->y)];
		if (missile->z < z)
			missile->exploding = true;

		/* Countdown? */
		if (missile->live_cd && !--missile->live_cd)
			missile->exploding = true;

		/* Will need these */
		x = missile->x;
		y = missile->y;
		z = missile->z;

		/* Process angle */
		if (missile->a_delta) {
			missile->angle += missile->a_delta << ANGLE_DELTA_SHIFT;
			missile->sin = Sin(missile->angle);
			missile->cos = Cos(missile->angle);
			missile->a_delta = 0;
		}

		/* Process XY movement */
		x += missile->sin >> XY_MISSILE_DELTA_SHIFT;
		y += missile->cos >> XY_MISSILE_DELTA_SHIFT;

		/* Keep in bounds */
		x = Min16(Max16(x, MIN_XYZ), MAX_XYZ);
		y = Min16(Max16(y, MIN_XYZ), MAX_XYZ);

		/* Process Z movement */
		if (missile->z_delta) {
			z += missile->z_delta << Z_DELTA_SHIFT;
			z = Min16(Max16(z, MIN_XYZ), MAX_XYZ);
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
		/* Still can't move? Explode and report stuck! */
		else {
			missile->exploding = true;
			ReportToAI(g_vehicles + missile->identifier, AIE_STUCK_MISSILE, arena[ARENA_INDEX(x, y)]);
		}

		/* Finalize Z */
		if (z >= arena[ARENA_INDEX(missile->x, missile->y)])
			missile->z = z;

		/* Check for player collisions */
		for (j = PLAYER_INDEX, player = g_vehicles + PLAYER_INDEX; j < PLAYER_LIMIT; ++j, ++player) {
			if (missile->identifier != player->identifier && player->active && abs(missile->z - player->z) <= VEHICLE_Z_TOL) {
				dx = abs(missile->x - player->x) >> XY_COLLISION_SHIFT;
				dy = abs(missile->y - player->y) >> XY_COLLISION_SHIFT;
				if (dx <= MSS_XY_TOL && dy <= MSS_XY_TOL && g_squares[dx] + g_squares[dy] <= MSS_DST_TOL_SQR) {
					missile->exploding = true;
					player->health -= missile->damage;
					player->hit_cd = MSS_HIT;
					ReportToAI(player, AIE_DAMAGED_PLAYER, missile->identifier);
					if (player->health <= 0) {
						if (!AdvancePlayer(g_vehicles + missile->identifier))
							g_vehicles[missile->identifier].health = PLAYER_HEALTH;
						if (!AdvancePlayer(player)) {
							player->active = false;
							ReportToAI(player, AIE_ELIMINATED_PLAYER, missile->identifier);
						} else
							ReportToAI(player, AIE_ADVANCED_PLAYER, missile->identifier);
						AddSound(BELL_RINGING);
					}
				}
			}
		}

		/* Animate explosions */
		if (missile->exploding) {
			missile->appearance[APP_PRM] = g_exploding_prm;
			missile->appearance[APP_AUX] = g_exploding_aux;
			AddSound(MSS_EXPLODING);
		}
	}
	/* Check if there's a winner */
	for (i = j = PLAYER_INDEX, player = NULL; i < PLAYER_LIMIT && j <= 1; ++i) {
		if (g_vehicles[i].active) {
			player = g_vehicles + i;
			++j;
		}
	}
	if (j == 1)
		ReportToAI(player, AIE_WINNING_PLAYER, 0);
}
