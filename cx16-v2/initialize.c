/*
    Contents: "initialize" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include <stdio.h>
#include "explore.h"

/* Defines used by InitData */

/* Global variables */
XM_HANDLE
	string_data,
	trig_data,
	arena_data;

bool
	exit_program;

uint16_t
	display_width,
	display_height;

int16_t
	max_vertices,
	max_segments,
	arena_index,
	arena_limit;

/* Internal functions */

static void InitData(char *file);
static void InitBounds(void);
static void InitHorizon(void);
static void InitWalls(void);
static void InitVehicles(void);

static void GetData(void *buffer, size_t size, FILE *ifile);

void InitProgram(void)
{
	/* Set up extended memory */
	InitXM();

	/* Set up error messages, trigonometric data, arenas, etc. */
	InitData("explore.dat");

	/* Initialize the hardware, etc. */
	InitSpecific();

	/* Generic initializations */
	InitBounds();
	InitHorizon();
	InitWalls();
	InitVehicles();
}

static void InitData(char *file)
{
	FILE *ifile;
	int16_t t, count, size;
	char code = CODE_ID;

	if (!(ifile = fopen(file, "rb")))
		ExitProgram(ERR_FO);

	while (code != CODE_EF) {
		GetData(&code, sizeof(char), ifile);
		switch (code) {
			case CODE_EM:
				GetData(&size, sizeof(int16_t), ifile);
				string_data = AllocXM(ERR_NO, size * sizeof(char *));
				for (t = 0; t < ERR_NO; ++t)
					GetData(GetXMAddress(string_data, t), size, ifile);
				break;
			case CODE_TD:
				trig_data = AllocXM(FULL_CIRC, sizeof(int16_t));
				GetData(GetXMAddressInitial(trig_data), sizeof(int16_t) * FULL_CIRC, ifile);
				break;
			case CODE_AD:
				GetData(&count, sizeof(int16_t), ifile);
				GetData(&size, sizeof(int16_t), ifile);
				arena_data = AllocXM(count, size);
				arena_index = 0;
				arena_limit = count;
				GetData(&max_vertices, sizeof(int16_t), ifile);
				GetData(&max_segments, sizeof(int16_t), ifile);
				for (t = 0; t < count; ++t)
					GetData(GetXMAddress(arena_data, t), size, ifile);
				break;
			case CODE_EF:
				fclose(ifile);
				break;
			default:
				ExitProgram(ERR_FC);
		}
	}
}

static void GetData(void *buffer, size_t size, FILE *ifile)
{
	if (!fread(buffer, size, 1, ifile))
		ExitProgram(ERR_FC);
}

static void InitBounds(void)
{
}

static void InitHorizon(void)
{
}

static void InitWalls(void)
{
}

static void InitVehicles(void)
{
}

/* Convenience methods to return Sin/Cos from lookup */
int16_t Sin(int16_t angle)
{
	return GetXMDirectSigned(trig_data, angle & (FULL_CIRC-1));
}

int16_t Cos(int16_t angle)
{
	return GetXMDirectSigned(trig_data, (angle + FULL_CIRC/4) & (FULL_CIRC-1));
}

/* Convenience method to output numbers for debugging purposes without loading in the entire stdio library! */
void OutputAsNumber(char prefix, int16_t value)
{
	int16_t i;
	char c;

	/* Output the prefix, can be anything */
	fputc(prefix, stdout);

	/* Handle negative numbers */
	if (value < 0) {
		value = -value;
		fputc('-', stdout);
	}

	/* Add 1 to account for bias */
	for (value += 1, i = 10000; i > 0; i /= 10) {
		c = '0';
		while (value > i) {
			++c;
			value -= i;
		}
		fputc(c, stdout);
	}
	fputc('\n', stdout);
}

void ExitProgram(int16_t stat)
{
	if (stat != ERR_NO) {
		if (string_data)
			fputs(GetXMAddress(string_data, stat), stdout);
		else
			fputs("\nRun DATA.PRG!\n", stdout);
		exit(stat);
	}
}
