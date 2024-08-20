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
	/*ERR_FO*/ "\nThe data file has been erased or\nrenamed. Recreate or restore from\nbackup.\n",
	/*ERR_FC*/ "\nThe data file has been corrupted.\nRecreate or restore from backup.\n",
	/*ERR_XM*/ "\nA banked memory request larger than 8K\nwas requested.\n",
	/*ERR_GI*/ "\nThere is a problem involving graphics\ninitialization.\n",
	/*ERR_OB*/ "\nThere was an attempt to access an array\nout of bounds.\n",
	/*ERR_DC*/ "\nThe display configuration has been\ncorrupted.\n"
};

/* Arenas are confined to cubes of size 16384.

   Heights from lowest to highest: ! # $ % & * +, using formula 0.075 * ascii(x) - 2.225

	!   25%
	#   40%
	$   47%
	%   55%
	&   63%
	*   92%
	+  100%
*/
char *arenas0[] = {
	/* Arena 1 - room */
	"+--------------+"
	"|              |"
	"|              |"
	"|              |"
	"|   #------#   |"
	"|   |      |   |"
	"|   |      |   |"
	"|   |      |   |"
	"|   |      |   |"
	"|   |      |   |"
	"|   |      |   |"
	"|   #------#   |"
	"|              |"
	"|              |"
	"|              |"
	"+--------------+",
	/* Arena 2 - north/south */
	"+--------------+"
	"|              |"
	"|              |"
	"|              |"
	"|  #        &  |"
	"|  |  $  %  |  |"
	"|  |  |  |  |  |"
	"|  |  |  |  |  |"
	"|  |  |  |  |  |"
	"|  |  |  |  |  |"
	"|  |  $  %  |  |"
	"|  #        &  |"
	"|              |"
	"|              |"
	"|              |"
	"+--------------+",
	/* Arena 3 - east/west */
	"+--------------+"
	"|              |"
	"|              |"
	"|   &------&   |"
	"|              |"
	"|              |"
	"|    $----$    |"
	"|              |"
	"|              |"
	"|    #----#    |"
	"|              |"
	"|              |"
	"|   %------%   |"
	"|              |"
	"|              |"
	"+--------------+",
	/* Arena 4 - isolation */
	"+--------------+"
	"|              |"
	"|              |"
	"|  *--------*  |"
	"|  |        |  |"
	"|  |        |  |"
	"|  *--------*  |"
	"|              |"
	"|              |"
	"|  *--------*  |"
	"|  |        |  |"
	"|  |        |  |"
	"|  *--------*  |"
	"|              |"
	"|              |"
	"+--------------+",
	/* Arena 5 - corners */
	"+--------------+"
	"|              |"
	"|              |"
	"|     $  &     |"
	"|     |  |     |"
	"|     |  |     |"
	"|  $--$  &--&  |"
	"|              |"
	"|              |"
	"|  !--!  %--%  |"
	"|     |  |     |"
	"|     |  |     |"
	"|     !  %     |"
	"|              |"
	"|              |"
	"+--------------+",
	/* Arena 6 - empty */
	"+--------------+"
	"|              |"
	"|              |"
	"|              |"
	"|              |"
	"|              |"
	"|              |"
	"|              |"
	"|              |"
	"|              |"
	"|              |"
	"|              |"
	"|              |"
	"|              |"
	"|              |"
	"+--------------+"
};

static void OutputStrings(FILE *ofile);
static void OutputTrigData(FILE *ofile);
static void OutputArenaData(FILE *ofile);

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
	OutputArenaData(ofile);

	/* Indicate end-of-data */
	fputc(CODE_EF, ofile);
	fclose(ofile);

	return EXIT_SUCCESS;
}

static void OutputStrings(FILE *ofile)
{
	int16_t t, u, num, max;
	char **strings = strings0;
	int16_t cnt = sizeof(strings0) / sizeof(char *);

	fputs("\nError Messages\n", stdout);
	fputc(CODE_EM, ofile);
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
	fputc(CODE_TD, ofile);
	d = 0.0;
	i = 2.0 * DCPI / FULL_CIRC;
	for (t = 0; t < FULL_CIRC; ++t, d += i) {
		dat = (int16_t)(sin(d) * SCALE_1_0);
		fwrite(&dat, sizeof(int16_t), 1, ofile);
		if (t % 26 == 17) fputc('*', stdout);
	}
	fputc('\n', stdout);
}

static void OutputArenaData(FILE *ofile)
{
	if (!ofile)
		fputs("?", stdout);
}
