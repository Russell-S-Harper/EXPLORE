/*
    Contents: "main" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include <time.h>
#include "explore.h"

void main()
{
	clock_t t;

	InitProgram();
	t = -clock();
	while (!g_exit_program) {
		ProcessVehicles();
		RenderObjects();
		UpdateDisplay();
		++g_frame_counter;
	}
	t += clock();
	OutputAsNumber('T', t);
	OutputAsNumber('/', g_frame_counter);
	OutputAsNumber('/', CLOCKS_PER_SEC);
	ExitProgram(ERR_NO);
}
