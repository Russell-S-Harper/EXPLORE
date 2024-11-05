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
	arena_data,
	*vehicle_data;

VEHICLE
	vehicles[VEHICLE_COUNT];

bool
	exit_program;

uint16_t
	display_width,
	display_height;

int16_t
	arena_index,
	max_arena_segments,
	max_arena_vertices,
	vehicle_index,
	max_vehicle_segments,
	max_vehicle_vertices;

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

	/* Initialize the vehicles */
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
				trig_data = AllocXM(SCALE_FC, sizeof(int16_t));
				GetData(GetXMAddressInitial(trig_data), sizeof(int16_t) * SCALE_FC, ifile);
				break;
			case CODE_AD:
				GetData(&count, sizeof(int16_t), ifile);
				GetData(&size, sizeof(int16_t), ifile);
				arena_data = AllocXM(count, size);
				arena_index = 0;
				GetData(&max_arena_vertices, sizeof(int16_t), ifile);
				GetData(&max_arena_segments, sizeof(int16_t), ifile);
				for (t = 0; t < count; ++t)
					GetData(GetXMAddress(arena_data, t), size, ifile);
				break;
			case CODE_VD:
				GetData(&count, sizeof(int16_t), ifile);
				GetData(&size, sizeof(int16_t), ifile);
				vehicle_data = malloc(sizeof(XM_HANDLE) * count);
				for (t = 0; t < count; ++t)
					vehicle_data[t] = AllocXM(1, size);
				GetData(&max_vehicle_vertices, sizeof(int16_t), ifile);
				GetData(&max_vehicle_segments, sizeof(int16_t), ifile);
				for (t = 0; t < count; ++t)
					GetData(GetXMAddressInitial(vehicle_data[t]), size, ifile);
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

static void InitVehicles(void)
{
	int i;

	for (i = 0; i < VEHICLE_COUNT; ++i) {
		vehicles[i].angle = (i << (SHIFT_FC - 2)) + (SCALE_FC / 8);
		vehicles[i].sin = Sin(vehicles[i].angle);
		vehicles[i].cos = Cos(vehicles[i].angle);
		vehicles[i].x = (MAX_XYZ >> 1) + SpecialMultiply(MAX_XYZ >> 2, vehicles[i].sin);
		vehicles[i].y = (MAX_XYZ >> 1) + SpecialMultiply(MAX_XYZ >> 2, vehicles[i].cos);
		vehicles[i].z = MIN_XYZ;
		vehicles[i].appearance[APP_PRM] = vehicle_data[0];
		vehicles[i].appearance[APP_AUX] = 0;
		vehicles[i].airborne = true;
	}
}

/* Convenience methods to return Sin/Cos from lookup */
int16_t Sin(int16_t angle)
{
	return GetXMDirectSigned(trig_data, angle & (SCALE_FC-1));
}

int16_t Cos(int16_t angle)
{
	return GetXMDirectSigned(trig_data, (angle + SCALE_FC/4) & (SCALE_FC-1));
}

/* Convenience method to output numbers for debugging purposes without linking in the entire stdio library! */
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
	/* Output each digit */
	for (i = 10000; i > 1; i /= 10) {
		c = '0';
		while (value >= i) {
			++c;
			value -= i;
		}
		fputc(c, stdout);
	}
	/* Last digit */
	fputc('0' + value, stdout);
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
