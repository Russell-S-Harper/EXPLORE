
#include <stdlib.h>
#include <stdio.h>
#include "define.h"
#include "declar.h"

/*	link with ibmspc.obj and prgdat.obj */

main()
{
	work t, array[30], *temp;
	prec a, b, c;

	PackPrec(a, 0x1020, 0x3040);
	PackPrec(b, 0x9080, 0x7060);
	PackPrec(c, 0x3030, 0xf0f0);
	if (a == 0x10203040 &&
		b == 0x90807060 &&
		c == 0x3030f0f0) puts("PackPrec passes");
	else puts("PackPrec fails");

	t = Prec2nd(a);
	if (t == 0x1020) puts("Prec2nd passes");
	else puts("Prec2nd fails");

	if (SlopeCalc(4, 5, 2) == 10 &&
		SlopeCalc(4000, 5000, 2000) == 10000 &&
		SlopeCalc(0, 1, 1) == 0 &&
		SlopeCalc(-2010, 3215, 6462) == -1000) puts("SlopeCalc passes");
	else puts("SlopeCalc fails");

	if (SpecMult(1, 1) == 0 &&
		SpecMult(4096, 1) == 1 &&
		SpecMult(-1286, 88) == -28 &&
		SpecMult(-1001, -10016) == 2447) puts("SpecMult passes");
	else puts("SpecMult fails");

	if (SpecDiv(1, 1) == 4096 &&
		SpecDiv(-235, 518) == -1858 &&
		SpecDiv(24105, 16545) == 5967) puts("SpecDiv passes");
	else puts("SpecDiv fails");

	InitInterrupt();

	printf("\nInterrupt at %d\nPress return", vbcounter);
	putchar(getchar());
	printf("Interrupt at %d (should be different)\n", vbcounter);

	puts("Approximately one a second");

	vbcounter = 0;
	t = 10;

	while (t--) {
		while (vbcounter < 73);
		vbcounter -= 73;
		putchar('*');
	}
	puts(" - done");

	ExitInterrupt();

	printf("Interrupt at %d\nPress return", vbcounter);
	putchar(getchar());
	printf("Interrupt at %d (should be the same)\n\n", vbcounter);

	for (t = 0; t < 30; t++) array[t] = 0;

	for (t = 0; t < 30; t++) printf("%d ", array[t]);
	printf(" - init\n");

	temp = FillArray(15, array, 6);
	for (t = 0; t < 30; t++) printf("%d ", array[t]);
	printf(" - 1st half == 6\n");

	FillArray(15, temp, 4);
	for (t = 0; t < 30; t++) printf("%d ", array[t]);
	printf(" - 2nd half == 4\n");

	FillArray(28, array + 1, 2);
	for (t = 0; t < 30; t++) printf("%d ", array[t]);
	printf(" - set middle\n");

	FillArray(1, array + 2, 8);
	for (t = 0; t < 30; t++) printf("%d ", array[t]);
	printf(" - set #3 to 8\n");

	FillArray(0, array + 3, 0);
	for (t = 0; t < 30; t++) printf("%d ", array[t]);
	printf(" - no change\n");
	putchar(getchar());

	ExitProgram(NOERR);
}

/*	Dummy functions so that everything doesn't have to be linked */
work dummy;

void SetField(work pstat)
{
	dummy = pstat;
}

void InitGraphics(void)
{
}

void InitPlot(void)
{
}
