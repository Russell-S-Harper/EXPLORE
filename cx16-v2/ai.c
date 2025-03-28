/*
    Contents: "AI" C source code (c) 2025
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include <stdio.h>
#include <time.h>
#include "explore.h"
#include "vera.h"

typedef struct {
	char identifer, parent;
	int8_t score, pursue, escape, evade, stuck_lmt, abuse_lmt, accuracy, randomizer,
		proximity, tit_for_tat, revenge, persistence, lowest_health, lowest_level;
} SETTINGS;

/* Internal data */
static const char
	*f_ai_data = "ai.dat",
	*f_working = "working.dat";

static uint8_t
	f_initialize[] = {'_', 0, 60, 60, 60, 60, 60, 60, 0, 0, 0, 0, 0, 0, 0},
	*f_random_bytes;

static SETTINGS
	*f_settings;

static void InitSettings(void);
static void InitRNG(void);
static uint8_t GetRandomByte(uint8_t modulus);
static int8_t AddRandomByte(uint8_t modulus, int8_t current, int8_t minimum, int8_t maximum);

void InitAI() {
	/* Initialize RNG */
	InitRNG();

	/* Initialize settings */
	InitSettings();
}

static void InitRNG(void)
{
	int16_t i;

	if (!f_random_bytes)
		f_random_bytes = malloc(sizeof(uint8_t) * UINT8_MAX);
	/* Using the current scan line and clock as the seed, gather 256 pseudo-random bytes */
	for (srand((clock() << 9) + CURRENT_SCAN_LINE), i = 0; i < UINT8_MAX; ++i)
		f_random_bytes[i] = (rand() >> 4) & UINT8_MAX;
}

static uint8_t GetRandomByte(uint8_t modulus)
{
	/* Index will roll over as required */
	static uint8_t s_index;

	switch (modulus) {
		/* Special case of zero to return the raw byte */
		case 0:
			return f_random_bytes[++s_index];
		/* Modulus of one can return only 0 */
		case 1:
			return 0;
		/* Optimize for powers of two */
		case 2:
		case 4:
		case 8:
		case 16:
		case 32:
		case 64:
		case 128:
			return f_random_bytes[++s_index] & (modulus - 1);
		/* Everything else, just calculate a regular modulus */
		default:
			return f_random_bytes[++s_index] % modulus;
	}
}

static int8_t AddRandomByte(uint8_t modulus, int8_t current, int8_t minimum, int8_t maximum)
{
	static uint8_t s_bit;
	int8_t delta;

	/* Random bit for the sign */
	s_bit <<= 1;
	if (!s_bit)
		s_bit = 0x01;

	/* Get signed byte */
	delta = GetRandomByte(modulus);
	if (GetRandomByte(0) & s_bit)
		delta = -delta;

	/* Add to current and clip within limits */
	return Maximum(Minimum(current + delta, maximum), minimum);
}

static void InitSettings(void)
{
	FILE *ifile, *ofile;
	SETTINGS *s, t;
	int16_t i, j;
	uint8_t modulus;
	char c;

	/* Set up space */
	if (!f_settings)
		f_settings = malloc(sizeof(SETTINGS) * PLAYER_COUNT);

	/* Check if the file exists */
	ifile = fopen(f_ai_data, "rb");
	if (!ifile) {
		/* It doesn't, so create it */
		ofile = fopen(f_ai_data, "wb");
		if (!ofile)
			ExitProgram(ERR_AI);

		for (c = 'a'; c <= 'd'; ++c) {
			fwrite(&c, sizeof(char), 1, ofile);
			fwrite(f_initialize, sizeof(SETTINGS) - sizeof(char), 1, ofile);
		}
		fclose(ofile);
		/* Now should be able to read it! */
		ifile = fopen(f_ai_data, "rb");
		if (!ifile)
			ExitProgram(ERR_AI);
	}
	/* Read in the settings, cycling through to the begining */
	for (i = j = 0; fread(f_settings + i, sizeof(SETTINGS), 1, ifile) ; i = (i + 1) % PLAYER_COUNT)
		j += sizeof(SETTINGS);
	fclose(ifile);

	/* Magic numbers to generate moduli [1, 60] decreasing with file size */
	modulus = Maximum(Minimum(63 - MultiplyThenDivide(j, 79, 1417), 60), 1);

	/* Randomize settings */
	for (i = 0, s = f_settings; i < PLAYER_COUNT; ++i, ++s) {
		s->pursue = AddRandomByte(modulus, s->pursue, 1, 120);
		s->escape = AddRandomByte(modulus, s->escape, 1, 120);
		s->evade = AddRandomByte(modulus, s->evade, 1, 120);
		s->stuck_lmt = AddRandomByte(modulus, s->stuck_lmt, 1, 120);
		s->abuse_lmt = AddRandomByte(modulus, s->abuse_lmt, 1, 120);
		s->accuracy = AddRandomByte(modulus, s->accuracy, 1, 120);
		s->randomizer = AddRandomByte(modulus, s->randomizer, 1, 120);
		s->proximity = AddRandomByte(modulus, s->proximity, -120, 120);
		s->tit_for_tat = AddRandomByte(modulus, s->tit_for_tat, -120, 120);
		s->revenge = AddRandomByte(modulus, s->revenge, -120, 120);
		s->persistence = AddRandomByte(modulus, s->persistence, -120, 120);
		s->lowest_health = AddRandomByte(modulus, s->lowest_health, -120, 120);
		s->lowest_level = AddRandomByte(modulus, s->lowest_level, -120, 120);
	}

	/* Append the new settings by copying, adding additional, and removing the original */
	rename(f_ai_data, f_working);
	ifile = fopen(f_working, "rb");
	ofile = fopen(f_ai_data, "wb");
	while (fread(&t, sizeof(SETTINGS), 1, ifile))
		fwrite(&t, sizeof(SETTINGS), 1, ofile);
	fclose(ifile);
	remove(f_working);
	fwrite(f_settings, sizeof(SETTINGS), PLAYER_COUNT, ofile);
	fclose(ofile);
}

void NPCAI(VEHICLE *player)
{
	int16_t z;
	/* For now do a dummy routine - remember to account for land and airborne */
	if (player->active) {
		z = g_vehicles[PLAYER_INDEX].z;
		player->a_delta = 1;
		if (player->z < z)
			player->z_delta = player->gear;
		else if (player->z > z)
			player->z_delta = -player->gear;
		if (!player->loading_cd)
			player->firing = true;
	}
}

void MissileAI(VEHICLE *missile)
{
	VEHICLE *player;
	int16_t delta;
	/* Just chase the target! This logic will work for land and/or airborne */
	if (missile->active && missile->target < PLAYER_COUNT) {
		player = g_vehicles + missile->target;
		if (player->active) {
			delta = SpecialMultiply(player->x - missile->x, missile->cos) - SpecialMultiply(player->y - missile->y, missile->sin);
			if (delta < -MSS_XY_TOL)
				missile->a_delta = -missile->mss_delta;
			else if (delta > MSS_XY_TOL)
				missile->a_delta = missile->mss_delta;
			delta = missile->z - player->z;
			if (delta < -VEHICLE_Z_TOL)
				missile->z_delta = missile->mss_delta;
			else if (delta > VEHICLE_Z_TOL)
				missile->z_delta = -missile->mss_delta;
		}
	}
}

void ReportToAI(VEHICLE *player, AI_EVENT event, int16_t extra)
{
	switch (event) {
		case AIE_NEW_ARENA:
			/* extra = arena index */
			break;
		case AIE_STUCK_PLAYER:
			/* extra = wall height */
			if (!player) OutputAsNumber('W', extra);
			break;
		case AIE_STUCK_MISSILE:
			/* extra = wall height */
			break;
		case AIE_DAMAGED_PLAYER:
			/* extra = identifier of attacker */
			break;
		case AIE_ADVANCED_PLAYER:
			/* extra = identifier of attacker */
			break;
		case AIE_ELIMINATED_PLAYER:
			/* extra = identifier of attacker */
			break;
		case AIE_WINNING_PLAYER:
			/* extra = not used */
			break;
	}
}
