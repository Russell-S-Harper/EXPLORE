/*
    Contents: "main" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include "explore.h"

void main()
{
	InitProgram();
	while (!g_exit_program) {
		ProcessVehicles();
		RenderObjects();
		UpdateDisplay();
		++g_frame_counter;
	}
	ExitProgram(ERR_NO);
}
