/*
    Contents: "main" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include <time.h>
#include "explore.h"

#define DEMO_FRAMES_PER_ARENA	120

void main()
{
	clock_t t;
	uint16_t f = 0;

	InitProgram();
	t = -clock();
	while (!g_exit_program) {
		ProcessVehicles();
		ScanField();
		RenderObjects();
		UpdateDisplay();
		++f;
		if (!(f % DEMO_FRAMES_PER_ARENA)) {
			if (!IndexExistsForXM(g_arena_data, ++g_arena_index))
				g_exit_program = true; // g_arena_index = 0;
			else
				g_vehicles[PLAYER_INDEX].appearance[APP_PRM] = g_vehicle_data[g_arena_index];
		}
	}
	t += clock();
	OutputAsNumber('T', t);
	OutputAsNumber('/', f);
	OutputAsNumber('/', CLOCKS_PER_SEC);
	ExitProgram(ERR_NO);
}
