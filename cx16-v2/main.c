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
	while (!exit_program) {
		ProcessVehicles();
		ScanField();
		RenderObjects();
		UpdateDisplay(NULL);
		++f;
		if (!(f % DEMO_FRAMES_PER_ARENA)) {
			if (!IndexExistsForXM(arena_data, ++arena_index))
				arena_index = 0;
		}
	}
	t += clock();
	OutputAsNumber('T', t);
	OutputAsNumber('/', f);
	OutputAsNumber('/', CLOCKS_PER_SEC);
	ExitProgram(ERR_NO);
}
