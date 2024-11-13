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
	g_string_data,
	g_trig_data,
	g_arena_data,
	g_exploding_prm,
	g_exploding_aux,
	*g_vehicle_data;

VEHICLE
	*g_vehicles;

LEVEL
	*g_levels;

bool
	g_exit_program;

uint16_t
	g_display_width,
	g_display_height;

int16_t
	g_arena_index,
	g_max_arena_segments,
	g_max_arena_vertices,
	g_vehicle_index,
	g_max_vehicle_segments,
	g_max_vehicle_vertices;

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

	/* Set up error messages, trigonometric data, arenas, and vehicles */
	InitData("explore.dat");

	/* Initialize the hardware, etc. */
	InitSpecific();

	/* Initialize the vehicles */
	InitVehicles();
}

static void InitData(char *file)
{
	FILE *ifile;
	int8_t tmp;
	int16_t t, count, size;
	char code = CODE_ID;

	if (!(ifile = fopen(file, "rb")))
		ExitProgram(ERR_FO);

	while (code != CODE_EF) {
		GetData(&code, sizeof(char), ifile);
		switch (code) {
			case CODE_EM:
				GetData(&size, sizeof(int16_t), ifile);
				g_string_data = AllocXM(ERR_NO, size * sizeof(char *));
				for (t = 0; t < ERR_NO; ++t)
					GetData(GetXMAddress(g_string_data, t), size, ifile);
				break;
			case CODE_TD:
				g_trig_data = AllocXM(SCALE_FC, sizeof(int16_t));
				GetData(GetXMAddressInitial(g_trig_data), sizeof(int16_t) * SCALE_FC, ifile);
				break;
			case CODE_AD:
				GetData(&count, sizeof(int16_t), ifile);
				GetData(&size, sizeof(int16_t), ifile);
				g_arena_data = AllocXM(count, size);
				g_arena_index = 0;
				GetData(&g_max_arena_vertices, sizeof(int16_t), ifile);
				GetData(&g_max_arena_segments, sizeof(int16_t), ifile);
				for (t = 0; t < count; ++t)
					GetData(GetXMAddress(g_arena_data, t), size, ifile);
				break;
			case CODE_VD:
				GetData(&count, sizeof(int16_t), ifile);
				GetData(&size, sizeof(int16_t), ifile);
				g_vehicle_data = malloc(sizeof(XM_HANDLE) * count);
				for (t = 0; t < count; ++t)
					g_vehicle_data[t] = AllocXM(1, size);
				g_exploding_prm = g_vehicle_data[count - EXP_APP_PRM_OFFSET];
				g_exploding_aux = g_vehicle_data[count - EXP_APP_AUX_OFFSET];
				GetData(&g_max_vehicle_vertices, sizeof(int16_t), ifile);
				GetData(&g_max_vehicle_segments, sizeof(int16_t), ifile);
				for (t = 0; t < count; ++t)
					GetData(GetXMAddressInitial(g_vehicle_data[t]), size, ifile);
				break;
			case CODE_LD:
				GetData(&count, sizeof(int16_t), ifile);
				g_levels = malloc(count * sizeof(LEVEL));
				for (t = 0; t < count; ++t) {
					GetData(&tmp, sizeof(int8_t), ifile);
					g_levels[t].arena = tmp;
					GetData(&tmp, sizeof(int8_t), ifile);
					g_levels[t].vehicle = g_vehicle_data[tmp];
					GetData(&tmp, sizeof(int8_t), ifile);
					g_levels[t].airborne = !!tmp;
					GetData(&tmp, sizeof(int8_t), ifile);
					g_levels[t].gear = tmp;
					GetData(&tmp, sizeof(int8_t), ifile);
					g_levels[t].missile = g_vehicle_data[tmp];
					GetData(&tmp, sizeof(int8_t), ifile);
					g_levels[t].delta = tmp;
					GetData(&tmp, sizeof(int8_t), ifile);
					g_levels[t].damage = tmp;
					g_levels[t].last = (t + 1 == count);
				}
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
	VEHICLE *vehicle;

	g_vehicles = calloc(VEHICLE_COUNT, sizeof(VEHICLE));

	for (i = 0, vehicle = g_vehicles; i < PLAYER_COUNT; ++i, ++vehicle) {
		vehicle->active = true;
		vehicle->angle = (i << (SHIFT_FC - 2)) + (SCALE_FC / 8);
		vehicle->sin = Sin(vehicle->angle);
		vehicle->cos = Cos(vehicle->angle);
		vehicle->x = (MAX_XYZ >> 1) + SpecialMultiply(MAX_XYZ >> 2, vehicle->sin);
		vehicle->y = (MAX_XYZ >> 1) + SpecialMultiply(MAX_XYZ >> 2, vehicle->cos);
		vehicle->z = MIN_XYZ;
		vehicle->level = -1;
		AdvanceVehicle(vehicle);
	}
}

bool AdvanceVehicle(VEHICLE *vehicle)
{
	LEVEL *level;

	if (vehicle->level >= 0 && g_levels[vehicle->level].last)
		return false;
	++vehicle->level;
	level = g_levels + vehicle->level;
	if (g_arena_index < level->arena)
		g_arena_index = level->arena;
	vehicle->appearance[APP_PRM] = level->vehicle;
	vehicle->airborne = level->airborne;
	vehicle->gear = level->gear;
	vehicle->missile = level->missile;
	vehicle->delta = level->delta;
	vehicle->damage = level->damage;
	return true;
}

/* Convenience methods to return Sin/Cos from lookup */
int16_t Sin(int16_t angle)
{
	return GetXMDirectSigned(g_trig_data, angle & (SCALE_FC-1));
}

int16_t Cos(int16_t angle)
{
	return GetXMDirectSigned(g_trig_data, (angle + SCALE_FC/4) & (SCALE_FC-1));
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
		if (g_string_data)
			fputs(GetXMAddress(g_string_data, stat), stdout);
		else
			fputs("\nRun DATA.PRG!\n", stdout);
		exit(stat);
	}
}
