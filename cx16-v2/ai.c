#include "explore.h"

void NPCAI(VEHICLE *player) {
	int16_t z;
	/* For now do a dummy routine */
	if (player->active) {
		z = g_vehicles[PLAYER_INDEX].z;
		player->a_delta = 1;
		if (player->z < z)
			player->z_delta = player->gear;
		else if (player->z > z)
			player->z_delta = -player->gear;
		if (!player->loading_cd)
			player->firing = true;
	}
}

void MissileAI(VEHICLE *missile) {
	VEHICLE *player;
	int16_t delta;
	/* Just chase the target! */
	if (missile->active && missile->target < PLAYER_COUNT) {
		player = g_vehicles + missile->target;
		if (player->active) {
			delta = SpecialMultiply(player->x - missile->x, missile->cos) - SpecialMultiply(player->y - missile->y, missile->sin);
			if (delta < -MSS_XY_TOL)
				missile->a_delta = -missile->mss_delta;
			else if (delta > MSS_XY_TOL)
				missile->a_delta = missile->mss_delta;
			delta = missile->z - player->z;
			if (delta < -VEHICLE_Z_TOL)
				missile->z_delta = missile->mss_delta;
			else if (delta > VEHICLE_Z_TOL)
				missile->z_delta = -missile->mss_delta;
		}
	}
}
