/*
    Contents: "initialize" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include <stdio.h>
#include "explore.h"
#include "vera.h"

/* Defines used by InitData */

/* Global variables */
XM_HANDLE
	g_string_data,
	g_trig_data,
	g_arena_data,
	g_exploding_prm,
	g_exploding_aux,
	g_human_badge_aux,
	*g_vehicle_data;

VEHICLE
	*g_vehicles;

LEVEL
	*g_levels;

bool
	g_exit_program;

uint8_t
	*g_psg_settings,
	*g_psg_volumes;

uint16_t
	g_display_width,
	g_display_height,
	g_frame_counter,
	*g_squares;

int16_t
	g_arena_index,
	g_max_arena_segments,
	g_max_arena_vertices,
	g_vehicle_index,
	g_max_vehicle_segments,
	g_max_vehicle_vertices;

/* Internal functions */

static void InitData(char *file);
static void InitSquares(void);
static void InitPlayers(void);

static void GetData(void *buffer, size_t size, FILE *ifile);

void InitProgram(void)
{
	/* Set up extended memory */
	InitXM();

	/* Set up error messages, trigonometric data, arenas, vehicles, and levels */
	InitData("explore.dat");

	/* Initialize the hardware, etc. */
	InitSpecific();

	/* Initialize the squares */
	InitSquares();

	/* Initialize the players */
	InitPlayers();

	/* Initialize the AI */
	InitAI();
}

static void InitData(char *file)
{
	char code = CODE_ID;
	uint8_t tmp;
	int16_t t, count, size;
	FILE *ifile;

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
				g_human_badge_aux = g_vehicle_data[count - HMN_BDG_AUX_OFFSET];
				GetData(&g_max_vehicle_vertices, sizeof(int16_t), ifile);
				GetData(&g_max_vehicle_segments, sizeof(int16_t), ifile);
				for (t = 0; t < count; ++t)
					GetData(GetXMAddressInitial(g_vehicle_data[t]), size, ifile);
				break;
			case CODE_SD:
				size = VERA_PSG_BLOCK_SIZE * SOUNDS_CNT * sizeof(uint8_t);
				g_psg_settings = malloc(size);
				GetData(g_psg_settings, size, ifile);
				size = VERA_PSG_VOLUMES * sizeof(uint8_t);
				g_psg_volumes = malloc(size);
				GetData(g_psg_volumes, size, ifile);
				break;
			case CODE_LD:
				GetData(&count, sizeof(int16_t), ifile);
				g_levels = malloc(count * sizeof(LEVEL));
				for (t = 0; t < count; ++t) {
					GetData(&tmp, sizeof(uint8_t), ifile);
					g_levels[t].arena = tmp;
					GetData(&tmp, sizeof(uint8_t), ifile);
					g_levels[t].player = g_vehicle_data[tmp];
					GetData(&tmp, sizeof(uint8_t), ifile);
					g_levels[t].airborne = !!tmp;
					GetData(&tmp, sizeof(uint8_t), ifile);
					g_levels[t].gear = tmp;
					GetData(&tmp, sizeof(uint8_t), ifile);
					g_levels[t].missile = g_vehicle_data[tmp];
					GetData(&tmp, sizeof(uint8_t), ifile);
					g_levels[t].mss_delta = tmp;
					GetData(&tmp, sizeof(uint8_t), ifile);
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

static void InitSquares(void)
{
	uint16_t s, i, j;

	g_squares = calloc(MSS_XY_TOL + 1, sizeof(uint16_t));

	for (s = i = 0, j = 1; i <= MSS_XY_TOL; ++i) {
		g_squares[i] = s;
		s += j;
		j += 2;
	}
}

static void InitPlayers(void)
{
	int16_t i;
	VEHICLE *player;

	g_vehicles = calloc(VEHICLE_COUNT, sizeof(VEHICLE));

	for (i = 0, player = g_vehicles; i < PLAYER_COUNT; ++i, ++player) {
		/* To prevent collisions with your own missiles! */
		player->identifier = i;
		/* Starting as active */
		player->active = true;
		/* Default to player as the target, except for the player */
		player->target = (i == PLAYER_INDEX)? PLAYER_COUNT: PLAYER_INDEX;
		if (i == PLAYER_INDEX) player->appearance[APP_AUX] = g_human_badge_aux;
		/* Initialize direction and position */
		player->angle = (i << (SHIFT_FC - 2)) + (SCALE_FC / 8);
		player->sin = Sin(player->angle);
		player->cos = Cos(player->angle);
		player->x = (MAX_XYZ >> 1) - SpecialMultiply(MAX_XYZ >> 2, player->sin);
		player->y = (MAX_XYZ >> 1) - SpecialMultiply(MAX_XYZ >> 2, player->cos);
		player->z = MIN_XYZ;
		/* To use for missiles */
		player->live_cd = MSS_LIVE;
		/* Will advance right away to level 0 */
		player->level = -1;
		AdvancePlayer(player);
	}
}

bool AdvancePlayer(VEHICLE *player)
{
	LEVEL *level;

	if (player->level >= 0 && g_levels[player->level].last)
		return false;
	++player->level;
	level = g_levels + player->level;
	if (g_arena_index < level->arena) {
		g_arena_index = level->arena;
		ReportToAI(player, AIE_NEW_ARENA, g_arena_index);
	}
	player->health = PLAYER_HEALTH;
	player->appearance[APP_PRM] = level->player;
	player->appearance[APP_MSS] = level->missile;
	player->airborne = level->airborne;
	player->gear = level->gear;
	player->mss_delta = level->mss_delta;
	player->damage = level->damage;
	return true;
}

/* Convenience methods to return Sin/Cos from lookup */
int16_t Sin(int16_t angle)
{
	return GetXMDirectSigned(g_trig_data, angle & (SCALE_FC - 1));
}

int16_t Cos(int16_t angle)
{
	return GetXMDirectSigned(g_trig_data, (angle + SCALE_FC/4) & (SCALE_FC - 1));
}

/* Convenience method to output numbers for debugging purposes without linking in the entire stdio library! */
void OutputAsNumber(char prefix, int16_t value)
{
	char c;
	int16_t i;

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

void ExitProgram(uint8_t stat)
{
	StopSounds();
	if (stat != ERR_NO) {
		if (g_string_data)
			fputs(GetXMAddress(g_string_data, stat), stdout);
		else
			fputs("\nRun DATA.PRG!\n", stdout);
		exit(stat);
	}
}
