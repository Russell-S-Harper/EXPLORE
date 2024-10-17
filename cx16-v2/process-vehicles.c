/*
    Contents: "process vehicles" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include "explore.h"

#define ANGLE_DELTA_SHIFT	1
#define XY_DELTA_SHIFT		6
#define Z_DELTA_SHIFT		7

#define ArenaIndex(x, y)	((((y) >> 6) & 0xF0) + ((x) >> 10))

/* Process every vehicle; the player-controlled one is done first */
void ProcessVehicles(void)
{
	VEHICLE *player;
	int16_t *arena, x, y, z;

	player = vehicles + PLAYER_INDEX;
	arena = GetXMAddress(arena_data, arena_index);
	z = arena[ArenaIndex(player->x, player->y)];
	if (player->z < z || !player->airborne)
		player->z = z;

	x = player->x;
	y = player->y;
	z = player->z;

	GetInput(player);

	if (player->angle_delta || (!player->sin && !player->cos)) {
		player->angle += (int16_t)player->angle_delta << ANGLE_DELTA_SHIFT;
		player->sin = Sin(player->angle);
		player->cos = Cos(player->angle);
		player->angle_delta = 0;
	}

	x += player->sin >> XY_DELTA_SHIFT;
	x = Minimum(Maximum(x, MIN_XYZ), MAX_XYZ);
	y += player->cos >> XY_DELTA_SHIFT;
	y = Minimum(Maximum(y, MIN_XYZ), MAX_XYZ);

	if (player->z_delta) {
		z += (int16_t)player->z_delta << Z_DELTA_SHIFT;
		z = Minimum(Maximum(z, MIN_XYZ), MAX_XYZ);
		player->z_delta = 0;
	}

	if (z >= arena[ArenaIndex(x, y)]) {
		player->x = x;
		player->y = y;
	}

	if (z >= arena[ArenaIndex(player->x, player->y)])
		player->z = z;
}
