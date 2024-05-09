/*
    Contents: "main" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include <time.h>
#include "explore.h"

void main()
{
	PLAYER_STATUS *p;
	clock_t t;
	uint16_t f = 0;

	InitProgram();
	t = -clock();
	while (!exit_program) {
		p = ProcessVehicles();
		ScanField();
		RenderObjects(p);
		UpdateDisplay(NULL);
		++f;
	}
	t += clock();
	OutputAsNumber('T', t);
	OutputAsNumber('/', f);
	OutputAsNumber('/', CLOCKS_PER_SEC);
	ExitProgram(ERR_NO);
}
