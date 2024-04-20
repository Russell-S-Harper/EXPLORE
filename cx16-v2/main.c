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
	char l = 100;

	InitProgram();
	t = -clock();
	while (!exit_program && l--) {
		ProcessVehicles();
		ScanField();
		RenderObjects();
		UpdateDisplay(NULL);
	}
	t += clock();
	OutputAsNumber('/', CLOCKS_PER_SEC * 100);
	OutputAsNumber('T', t);
	ExitProgram(NOERR);
}
