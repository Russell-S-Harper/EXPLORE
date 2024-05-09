/*
    Contents: "data" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "common.h"

#define DCPI	3.141592653589793

/* Error messages */
char *strings0[] = {
	"\nThe data file has been erased or\nrenamed. Recreate or restore from\nbackup.\n",
	"\nThe data file has been corrupted.\nRecreate or restore from backup.\n",
	"\nThere is not enough memory to run the\nprogram. It can require 160K.\n",
	"\nThere is a problem involving TGI\ninitialization.\n",
	"\nA banked memory request larger than 8K\nwas requested.\n",
	"\nThere was an attempt to access an array\nout of bounds.\n",
	"\nThe display configuration has been\ncorrupted.\n"
};

/* Data used in vehicle scanning - related to arctangent, note VEH_DIR + 1 entries */
char arctan0[] = {
	0x03, 0x08, 0x0D, 0x12, 0x16, 0x1B, 0x1F, 0x24,
	0x28, 0x2C, 0x2F, 0x33, 0x36, 0x39, 0x3C, 0x3F,
	0x41, 0x44, 0x46, 0x48, 0x4A, 0x4C, 0x4E, 0x4F,
	0x51, 0x52, 0x54, 0x55, 0x56, 0x58, 0x59, 0x5A,
	0x5B, 0x5C, 0x5D, 0x5D, 0x5E, 0x5F, 0x60, 0x61,
	0x61, 0x62, 0x63, 0x63, 0x64, 0x64, 0x65, 0x66,
	0x66, 0x67, 0x67, 0x67, 0x68, 0x68, 0x69, 0x69,
	0x6A, 0x6A, 0x6A, 0x6B, 0x6B, 0x6B, 0x6C, 0x6C, 0x6C
};

static void OutputStrings(FILE *ofile);
static void OutputTrigData(FILE *ofile);

int main(void)
{
	FILE *ofile;

	if (!(ofile = fopen("explore.dat", "wb"))) {
		fputs("\nCould not create EXPLORE.DAT!\n", stderr);
		return EXIT_FAILURE;
	}

	/* Set up the error messages first so that we can display them if there's a subsequent problem */
	OutputStrings(ofile);

	/* Then the trigonometric data which is needed to set up the remaining data */
	OutputTrigData(ofile);

	/* The remaining data */

	/* Indicate end-of-data */
	fputc(EFCODE, ofile);
	fclose(ofile);

	return EXIT_SUCCESS;
}

static void OutputStrings(FILE *ofile)
{
	int16_t t, u, num, max;
	char **strings = strings0;
	int16_t cnt = sizeof(strings0) / sizeof(char *);

	fputs("\nError Messages\n", stdout);
	fputc(EMCODE, ofile);
	/* Get the longest string */
	for (t = 0, max = 0; t < cnt; ++t) {
		num = strlen(strings[t]);
		if (max < num) max = num;
	}
	/* Add one for NULL terminator */
	++max;
	/* Write out the size */
	fwrite (&max, sizeof(int16_t), 1, ofile);
	/* Write out the strings */
	for (t = 0; t < cnt; ++t) {
		num = strlen(strings[t]);
		fputs(strings[t], ofile);
		for (u = num; u < max; ++u)
			fputc('\0', ofile);
	}
}

static void OutputTrigData(FILE *ofile)
{
	int16_t t, dat;
	double d, i;

	/* Sine and cosine data used throughout */
	fputs("Sin Data ", stdout);
	fputc(TDCODE, ofile);
	d = 0.0;
	i = 2.0 * DCPI / FULL_CIRC;
	for (t = 0; t < FULL_CIRC; ++t, d += i) {
		dat = (int16_t)(sin(d) * SCALE_1_0);
		fwrite(&dat, sizeof(int16_t), 1, ofile);
		if (t % 26 == 17) fputc('*', stdout);
	}
	fputc('\n', stdout);

	/* Tangent data used to render the horizon */
	fputs("Tan Data ", stdout);
	fputc(HDCODE, ofile);
	d = DCPI/-2.0 + DCPI/(double)HALF_CIRC;
	for (t = 0; t < HALF_CIRC - 1; t++, d += i) {
		dat = (int16_t)(tan(d) * SCALE_1_0);
		fwrite(&dat, sizeof(int16_t), 1, ofile);
		if (t % 13 == 7) fputc('*', stdout);
	}
	fputc('\n', stdout);

	/* Arctangent data used in vehicle scanning */
	fputs("Arctan Data\n", stdout);
	fputc(ATCODE, ofile);
	fwrite(arctan0, sizeof(arctan0), 1, ofile);
}
