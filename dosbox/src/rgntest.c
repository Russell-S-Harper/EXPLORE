
/* rgntest.c */

/*	Link with prgdat.obj ibmspc.obj objrtn.obj fldrtn.obj */

#include <stdlib.h>
#include <stdio.h>
#include "define.h"
#include "declar.h"

#define FCLIM	70
#define FCSTP	10

void main()
{
	work t, u, temp, lastcounter = 0, fc[FCLIM+1];

	InitProgram();

	for (t = 0; t < FCLIM; t++) fc[t] = 0;

	while (!quit_prg) {
		vbcounter = 0;

		ProcessVeh();
		ScanField();
		RenderObj();
		UpdateDisplay();

		if ((temp = vbcounter) < FCLIM) fc[temp]++;
		if (temp < lastcounter) lastcounter--;
		else lastcounter = temp;
		while (vbcounter < lastcounter);
	}

	ExitProgram(NOERR);

	for (t = u = 0; t < FCLIM; t++) if (fc[t] > u) u = fc[t];
	if (u > FCSTP * 40) u = FCSTP * 40;
	u = u / FCSTP * FCSTP - FCSTP/2;

	for (; u > 0; u -= FCSTP) {
		fputc('\n', stdout);
		for (t = 0; t < FCLIM; t++) {
			if (fc[t] >= u) fputc('*', stdout);
			else fputc(' ', stdout);
		}
	}
	fputc('\n', stdout);

	for (t = 0; t < FCLIM; t++) fputc('-', stdout);
	fputc('\n', stdout);

	for (t = 0; t < FCLIM; t++)
		fputc((char)('0' + (t - t / FCSTP * FCSTP)), stdout);
	fputc('\n', stdout);

	for (t = 0; t < FCLIM; t += FCSTP) {
		fputc((char)('0' + t / FCSTP), stdout);
		for (u = 0; u < FCSTP - 1; u++) fputc(' ', stdout);
	}
	fputc('\n', stdout);
}
