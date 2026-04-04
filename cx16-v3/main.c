/*
    Contents: "main" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include "explore.h"

void main()
{
#ifdef REMOTE_AI
	char c;
	int16_t i;
	VEHICLE *t;
#endif /* REMOTE_AI */

	InitProgram();
#ifdef REMOTE_AI
	OutputAsNumber(AIR_VERSION, AIR_CURRENT);
#endif /* REMOTE_AI */
	while (!g_exit_program) {
		ProcessVehicles();
		RenderObjects();
		UpdateDisplay();
#ifdef REMOTE_AI
		if (g_frame_counter & 1) {
			for (i = PLAYER_INDEX, t = g_vehicles + PLAYER_INDEX; i < PLAYER_LIMIT; ++i, ++t) {
				OutputAsNumber(AIR_PLAYER, AIR_UNUSED);
				OutputAsNumber(c = 'a', t->active);
				OutputAsNumber(++c, t->npc);
				OutputAsNumber(++c, t->airborne);
				OutputAsNumber(++c, t->exploding);
				OutputAsNumber(++c, t->firing);
				OutputAsNumber(++c, t->health);
				OutputAsNumber(++c, t->z_delta);
				OutputAsNumber(++c, t->last_z_delta);
				OutputAsNumber(++c, t->a_delta);
				OutputAsNumber(++c, t->last_a_delta);
				OutputAsNumber(++c, t->level);
				OutputAsNumber(++c, t->gear);
				OutputAsNumber(++c, t->identifier - PLAYER_INDEX);
				OutputAsNumber(++c, t->target - PLAYER_INDEX);
				OutputAsNumber(++c, t->joy);
				OutputAsNumber(++c, t->mss_delta);
				OutputAsNumber(++c, t->damage);
				OutputAsNumber(++c, t->hit_cd);
				OutputAsNumber(++c, t->loading_cd);
				OutputAsNumber(++c, t->live_cd);
				OutputAsNumber(++c, t->x);
				OutputAsNumber(++c, t->y);
				OutputAsNumber(++c, t->z);
				OutputAsNumber(++c, ((t->angle + SCALE_FC / 2) & (SCALE_FC - 1)) - SCALE_FC / 2);
				OutputAsNumber(++c, t->sin);
				OutputAsNumber(++c, t->cos);
			}
			OutputAsNumber(AIR_EOT, AIR_UNUSED);
		}
#endif /* REMOTE_AI */
		++g_frame_counter;
	}
	ExitProgram(ERR_NO);
#ifdef REMOTE_AI
	OutputAsNumber(AIR_QUIT, AIR_UNUSED);
#endif /* REMOTE_AI */
}
