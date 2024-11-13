/*
    Contents: "main" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include <time.h>
#include "explore.h"

#define DEMO_FRAMES_PER_LEVEL	120

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
		if (!(f % DEMO_FRAMES_PER_LEVEL))
			g_exit_program = !AdvanceVehicle(g_vehicles + PLAYER_INDEX);
	}
	t += clock();
	OutputAsNumber('T', t);
	OutputAsNumber('/', f);
	OutputAsNumber('/', CLOCKS_PER_SEC);
	ExitProgram(ERR_NO);
}
