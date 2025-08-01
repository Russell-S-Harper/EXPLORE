/*
    Contents: "AI" C source code (c) 2025
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>
#include "explore.h"

/* Defining constants generically for now, once they are all defined, will come up with a unified naming convention */

#define AI_K01	4	/* What to shift value from rand() to get "good" randomness */

#define AI_K02	'a'	/* When initializing AI.DATA, identifier of the first player */
#define AI_K03	'z'	/* Last legal identifier */
#define AI_K04	'_'	/* Placeholder for identifiers */
#define AI_K05	2	/* Winners offset */

#define AI_K06	-79	/* To convert AI.DAT file size to randomizer "slope numerator" */
#define AI_K07	1417	/* To convert AI.DAT file size to randomizer "slope denominator" */
#define AI_K08	63	/* To convert AI.DAT file size to randomizer "intercept" */

#define AI_K09	-120	/* Lower bounds of full range setting */
#define AI_K10	120	/* Upper bounds of full range setting */
#define AI_K11	0	/* Middle of full range setting */
#define AI_K12	1	/* Lower bounds of half range setting */
#define AI_K13	120	/* Upper bounds of half range setting */
#define AI_K14	60	/* Middle of half range setting */

#define AI_K15	-5	/* Initial score */
#define AI_K16	0	/* Initial tally */

#define AI_K17	8	/* What to shift delta XYZ to use when calculating distance */
#define AI_K18	3	/* What to shift (delta Z)² to account for height bias */
#define AI_K19	2	/* Modulus to randomly pick in the event of ties */
#define AI_K20	145	/* Tally adjustment to "focus on human" */

#define AI_K21	-10	/* What to change score by when player is advanced when health goes to zero */
#define AI_K22	5	/* What to change score by when attacker is advanced */
#define AI_K23	15	/* What to change score by when attacker eliminates another player */

#define AI_K24	4	/* How much to shift randomizer when adding to escape or evade */
#define AI_K25	24	/* What to set action_cd when mourning */
#define AI_K26	60	/* What to set action_cd when celebrating */
#define AI_K27	1	/* ceil(log2(AI_K13 / PLAYER_HEALTH)); what to shift player damage */

#define AI_K28	5	/* Modulus when setting a_action */
#define AI_K29	-2	/* Offset when setting a_action */
#define AI_K30	3	/* Modulus when setting z_action for escape status */
#define AI_K31	0	/* Offset when setting z_action for escape status */
#define AI_K32	3	/* Modulus when setting z_action for evade status */
#define AI_K33	-2	/* Offset when setting z_action for evade status */
#define AI_K34	5	/* Modulus when setting z_action for celebration status */
#define AI_K35	-2	/* Offset when setting z_action for celebration status */
#define AI_K36	3	/* Modulus when setting g_action */
#define AI_K37	-1	/* Offset when setting g_action */

#define AI_K38	1	/* Shift of action_cd for 3 players */
#define AI_K39	2	/* Shift of action_cd for 2 players */
#define AI_K40	4	/* Minimum value after shifting action_cd */

/* Avoiding is meant to handle corners, so best to define a preset action, like an instinct */
#define AI_K41	32	/* Offset for action_cd when avoiding */
#define AI_K42	4	/* Modulus for action_cd when avoiding (to provide some variability) */
#define AI_K43	2	/* Modulus to determine sign of a_action when avoiding */
#define AI_K44	2	/* What to set a_action when avoiding */
#define AI_K45	1	/* z_action when avoiding */
#define AI_K46	-1	/* g_action when avoiding */

#define AI_K47	1	/* What to set a_delta or z_delta if initiating */
#define AI_K48	2	/* What to set a_delta or z_delta if continuing */
#define AI_K49	1	/* AND of player identifier to determine a_delta direction if target is directly behind */
#define AI_K50	2	/* What to set a_delta if target is directly behind; based on player identifier */

#define AI_K51	9	/* What to shift dx and dy for accuracy calculation */
#define AI_K52	961	/* Constant used in ensure result of accuracy calculation is [-120, 120] */
#define AI_K53	3	/* What to shift to ensure result of accuracy calculation is [-120, 120] */

/* Most AI is kept separate from the regular operation of the game */

enum {AIS_READY, AIS_PURSUE, AIS_ESCAPE, AIS_EVADE, AIS_AVOID, AIS_MOURN, AIS_CELEBRATE, AIS_DEAD};

typedef struct {
	char identifier, parent;
	int8_t score, pursue, escape, evade, impeded_lmt, abuse_lmt, accuracy, randomizer,
		proximity, tit_for_tat, revenge, persistence, lowest_health, lowest_level;
} AI_SETTINGS;

typedef struct {
	uint16_t attackers[PLAYER_COUNT];
	uint8_t status, attacker;
	int8_t a_action, z_action, g_action, action_cd, impeded_cd, abuse_cd;
} AI_CURRENT_STATE;

/* Internal data */

XM_HANDLE
	f_random_bytes;

static AI_CURRENT_STATE
	*f_current_state;

static AI_SETTINGS
	*f_settings,
	f_initialize = {AI_K04, AI_K04, AI_K15,
		AI_K14, AI_K14, AI_K14, AI_K14, AI_K14, AI_K14,
		AI_K11, AI_K11, AI_K11, AI_K11, AI_K11, AI_K11, AI_K11};

static uint8_t
	f_modulus,
	f_crc;

static char
	f_identifier,
	f_working[] = {'a', 'i', '-', '0', '0', '0', '0', '0', '.', 't', 'm', 'p', '\0'},
	f_archive[] = {'a', 'i', '-', '0', '0', '0', '0', '0', '.', 'd', 'a', 't', '\0'};

static const char
	*f_ai_data = "ai.dat";

static void InitRNG(void);
static void InitSettings(void);
static void UpdateSettings(AI_SETTINGS *s);
static int CompareSettings(const void *p, const void *q);
static void InitStatuses(void);
static uint8_t GetRandomByte(uint8_t modulus);
static int8_t AddRandomValue(uint8_t modulus, int8_t current, int8_t minimum, int8_t maximum);
static void RandomizeFilename(char *filename);
static uint8_t UpdateCRC8(uint8_t crc, const void *s, uint8_t count);
static void SetAZGActions(AI_CURRENT_STATE *x);

void InitAI()
{
	InitRNG();
	InitSettings();
	InitStatuses();

	/* Set focus to a random player */
	g_vehicle_index = PLAYER_INDEX + GetRandomByte(PLAYER_COUNT);
}

static void InitRNG(void)
{
	int16_t i;
	uint8_t *p;

	if (!f_random_bytes)
		f_random_bytes = AllocXM(UINT8_MAX + 1, sizeof(uint8_t));

	/* Gather 256 pseudo-random bytes */
	for (__randomize(), i = 0, p = GetXMAddressInitial(f_random_bytes); i <= UINT8_MAX; ++i)
		p[i] = (rand() >> AI_K01) & UINT8_MAX;
}

static uint8_t GetRandomByte(uint8_t modulus)
{
	/* Index will roll over as required */
	static uint8_t s_index;
	uint8_t result;

	/* Get the raw byte */
	GetXM(f_random_bytes, ++s_index, &result);

	switch (modulus) {
		/* Special case of zero to return the raw byte */
		case 0:
			return result;
		/* Modulus of one can return only 0 */
		case 1:
			/* Reuse the byte for next invocation */
			--s_index;
			return 0;
		/* Optimize for powers of two */
		case 2:
		case 4:
		case 8:
		case 16:
		case 32:
		case 64:
		case 128:
			return result & (modulus - 1);
		/* Everything else, just calculate a regular modulus */
		default:
			return result % modulus;
	}
}

static int8_t AddRandomValue(uint8_t modulus, int8_t current, int8_t minimum, int8_t maximum)
{
	static uint8_t s_bit;
	int8_t delta;

	/* Random bit for the sign */
	s_bit <<= 1;
	if (!s_bit)
		++s_bit;

	/* Get signed byte */
	delta = GetRandomByte(modulus);
	if (GetRandomByte(0) & s_bit)
		delta = -delta;

	/* Add to current and clip within limits */
	return Min8(Max8(current + delta, minimum), maximum);
}

static void RandomizeFilename(char *filename)
{
	uint8_t i;

	for (i = 0; filename[i]; ++i) {
		if (isdigit(filename[i]))
			filename[i] = '0' + GetRandomByte(10);
	}
}

static uint8_t UpdateCRC8(uint8_t crc, const void *s, uint8_t count)
{
	const uint8_t *p = s, *CRC8 = GetXMAddressInitial(g_crc_data);
	uint8_t i;

	if (p) {
		for (i = 0; i < count; ++i)
			crc = CRC8[crc ^ p[i]];
	}
	return crc;
}

static void InitSettings(void)
{
	FILE *ifile, *ofile;
	AI_SETTINGS *s;
	int16_t j;
	uint8_t i;

	/* Set up space */
	if (!f_settings)
		f_settings = malloc(sizeof(AI_SETTINGS) * PLAYER_COUNT);

	/* Check if the file exists */
	ifile = fopen(f_ai_data, g_read_mode);
	if (!ifile) {
		/* It doesn't, so create it */
		ofile = fopen(f_ai_data, g_write_mode);
		if (!ofile)
			ExitProgram(ERR_AI);
		for (i = 0; i < PLAYER_COUNT; ++i) {
			f_initialize.identifier = AI_K02 + i;
			fwrite(&f_initialize, sizeof(AI_SETTINGS), 1, ofile);
		}
		fclose(ofile);
		/* Now should be able to read it! */
		ifile = fopen(f_ai_data, g_read_mode);
		if (!ifile)
			ExitProgram(ERR_AI);
	}
	/* Read in the settings, cycling through to the beginning */
	for (i = 0, j = 0, f_crc = 0; ; i = (i + 1) % PLAYER_COUNT) {
		s = f_settings + i;
		if (!fread(s, sizeof(AI_SETTINGS), 1, ifile))
			break;
		/* Save a checksum of the file's contents */
		f_crc = UpdateCRC8(f_crc, s, sizeof(AI_SETTINGS));
		j += sizeof(AI_SETTINGS);
	}
	fclose(ifile);

	/* Magic numbers to generate moduli [AI_K12, AI_K14] decreasing with file size */
	f_modulus = Max16(Min16(MulDiv16(j, AI_K06, AI_K07) + AI_K08, AI_K14), AI_K12);

	/* Randomize settings */
	for (i = 0, s = f_settings; i < PLAYER_COUNT; ++i, ++s) {
		s->score = AI_K15;
		s->pursue = AddRandomValue(f_modulus, s->pursue, AI_K12, AI_K13);
		s->escape = AddRandomValue(f_modulus, s->escape, AI_K12, AI_K13);
		s->evade = AddRandomValue(f_modulus, s->evade, AI_K12, AI_K13);
		s->impeded_lmt = AddRandomValue(f_modulus, s->impeded_lmt, AI_K12, AI_K13);
		s->abuse_lmt = AddRandomValue(f_modulus, s->abuse_lmt, AI_K12, AI_K13);
		s->accuracy = AddRandomValue(f_modulus, s->accuracy, AI_K12, AI_K13);
		s->randomizer = AddRandomValue(f_modulus, s->randomizer, AI_K12, AI_K13);
		s->proximity = AddRandomValue(f_modulus, s->proximity, AI_K09, AI_K10);
		s->tit_for_tat = AddRandomValue(f_modulus, s->tit_for_tat, AI_K09, AI_K10);
		s->revenge = AddRandomValue(f_modulus, s->revenge, AI_K09, AI_K10);
		s->persistence = AddRandomValue(f_modulus, s->persistence, AI_K09, AI_K10);
		s->lowest_health = AddRandomValue(f_modulus, s->lowest_health, AI_K09, AI_K10);
		s->lowest_level = AddRandomValue(f_modulus, s->lowest_level, AI_K09, AI_K10);
	}
}

/* These defines and the static strings & character arrays in UpdateSettings assume
   the summary message is formatted as "\eT<color 0 - F>  <ordinal 1st - 4th>  " */
#define SM_SUMMARY_MSG_IDX	2
#define SM_REVISE_STR_IDX	10
#define SM_IDENTIFIER_IDX	1
#define SM_CONVERT_TO_UPPER	('A' - 'a')
#define SM_REVISE_SCORE_IDX	7
#define SM_BASE_CONSTANT	10
#define SM_SCORE_MULTIPLIER	5

/* The provided settings are those of the winner! */
static void UpdateSettings(AI_SETTINGS *s)
{
/* No room for summary when debugging! */
#ifndef DEBUG
	static const char
		*s_parent_is_na = "---",
		*s_human_identifier = "Human",
		*s_initial_score = "       0  ";			/* Lots of spaces for alignment */
	static char
		s_parent[] = {'\"', AI_K04, '\"', '\0'},
		s_identifier[] = {'\"', AI_K04, '\"', ' ', ' ', '\0'},	/* Two extra spaces to align with "Human" */
		s_score[11];						/* See s_initial_score */
#endif /* DEBUG */

	FILE *ifile, *ofile;
	AI_SETTINGS t;
	uint8_t i, crc;
	char identifier;

/* No room for summary... */
#ifndef DEBUG
	int16_t score;
	uint8_t j;
	char *working;
	bool negative;
#endif /* DEBUG */

	/* Will need this when sorting */
	f_identifier = s->identifier;
	/* Sort according to winner and scores */
	qsort(f_settings, PLAYER_COUNT, sizeof(AI_SETTINGS), CompareSettings);

/* No room,,, */
#ifndef DEBUG
	/* Set up a summary */
	for (i = 0, s = f_settings; i < PLAYER_COUNT; ++i, ++s) {
		working = GetXMAddress(g_summary_messages, i + SM_SUMMARY_MSG_IDX);
		working[SM_REVISE_STR_IDX] = '\0';
		if (s->identifier != AI_K04) {
			s_identifier[SM_IDENTIFIER_IDX] = SM_CONVERT_TO_UPPER + s->identifier;
			strcat(working, s_identifier);
		} else
			strcat(working, s_human_identifier);
		score = s->score * SM_SCORE_MULTIPLIER;
		negative = score < 0;
		if (negative)
			score = -score;
		for (j = SM_REVISE_SCORE_IDX, strcpy(s_score, s_initial_score); score; --j, score /= SM_BASE_CONSTANT)
			s_score[j] = '0' + score % SM_BASE_CONSTANT;
		if (negative)
			s_score[j] = '-';
		strcat(working, s_score);
		if (s->parent != AI_K04) {
			s_parent[SM_IDENTIFIER_IDX] = SM_CONVERT_TO_UPPER + s->parent;
			strcat(working, s_parent);
		} else
			strcat(working, s_parent_is_na);
	}
	/* Display it */
	QueueNewMessages(g_summary_messages);
#endif /* DEBUG */

	if (!g_human_joined) {
		crc = 0;
		if (f_modulus > AI_K12) {
			/* Append settings */
			ifile = fopen(f_ai_data, g_read_mode);
			do
				RandomizeFilename(f_working);
			while (!(ofile = fopen(f_working, g_write_mode)));
			identifier = '\0';
			while (fread(&t, sizeof(AI_SETTINGS), 1, ifile)) {
				/* Update CRC */
				crc = UpdateCRC8(crc, &t, sizeof(AI_SETTINGS));
				/* Also get the latest identifier */
				if (identifier < t.identifier)
					identifier = t.identifier;
				fwrite(&t, sizeof(AI_SETTINGS), 1, ofile);
			}
			fclose(ifile);
			/* Append the sorted settings */
			fwrite(f_settings, sizeof(AI_SETTINGS), PLAYER_COUNT, ofile);
			/* If there's room, clone the winners and append */
			if (identifier <= AI_K03 - AI_K05) {
				/* Update parentage */
				for (i = PLAYER_COUNT - AI_K05; i < PLAYER_COUNT; ++i) {
					s = f_settings + i;
					s->parent = s->identifier;
					s->identifier = ++identifier;
				}
				/* Output cloned players */
				fwrite(f_settings + PLAYER_COUNT - AI_K05, sizeof(AI_SETTINGS), AI_K05, ofile);
			}
			/* Done */
			fclose(ofile);
			/* Is the file the same as before? */
			if (crc == f_crc) {
				/* Replace the existing file with the new file */
				remove(f_ai_data);
				rename(f_working, f_ai_data);
			}
#ifdef REMOVE_TMP
			 else
				remove(f_working);
#endif
		} else {
			/* Get the CRC of the current data file */
			ifile = fopen(f_ai_data, g_read_mode);
			while (fread(&t, sizeof(AI_SETTINGS), 1, ifile))
				crc = UpdateCRC8(crc, &t, sizeof(AI_SETTINGS));
			fclose(ifile);
			/* Reset identifiers and parents */
			for (i = 0; i < PLAYER_COUNT; ++i) {
				s = f_settings + i;
				s->identifier = AI_K02 + i;
				s->parent = AI_K04;
			}
			/* Set up a new data file using the winners of the current one */
			do
				RandomizeFilename(f_working);
			while (!(ofile = fopen(f_working, g_write_mode)));
			fwrite(f_settings, sizeof(AI_SETTINGS), PLAYER_COUNT, ofile);
			fclose(ofile);
			/* Is the file the same as before? */
			if (crc == f_crc) {
				/* Archive the existing file and replace with the new file */
				do
					RandomizeFilename(f_archive);
				while (rename(f_ai_data, f_archive));
				rename(f_working, f_ai_data);
			}
#ifdef REMOVE_TMP
			 else
				remove(f_working);
#endif
		}
	}
}

static int CompareSettings(const void *p, const void *q)
{
	const AI_SETTINGS *a = p, *b = q;
	/* Winner last */
	if (a->identifier == f_identifier)
		return 1;
	if (b->identifier == f_identifier)
		return -1;
	/* Score descending */
	if (a->score > b->score)
		return 1;
	if (a->score < b->score)
		return -1;
	/* Randomizer ascending */
	if (a->randomizer < b->randomizer)
		return 1;
	if (a->randomizer > b->randomizer)
		return -1;
	/* Got to here? They're equal */
	return 0;
}

static void InitStatuses(void)
{
	AI_CURRENT_STATE *x;
	AI_SETTINGS *s;
	uint8_t i, j;

	/* Set up space */
	if (!f_current_state)
		f_current_state = malloc(sizeof(AI_CURRENT_STATE) * PLAYER_COUNT);

	/* Initialize statuses */
	for (i = 0, x = f_current_state, s = f_settings; i < PLAYER_COUNT; ++i, ++x, ++s) {
		x->status = AIS_READY;
		x->a_action = 0;
		x->z_action = 0;
		x->g_action = 0;
		x->attacker = PLAYER_LIMIT;
		for (j = 0; j < PLAYER_COUNT; ++j)
			x->attackers[j] = 0;
		x->action_cd = 0;
		x->impeded_cd = s->impeded_lmt;
		x->abuse_cd = s->abuse_lmt;
	}
}

void NPCAI(VEHICLE *player)
{
	static int16_t s_tallies[PLAYER_COUNT];
	AI_CURRENT_STATE *x;
	AI_SETTINGS *s;
	VEHICLE *t;
	int16_t dx, dy, dz, fb, lr, current, working;
	uint8_t i, j, k;

	/* Will need these */
	x = f_current_state + player->identifier - PLAYER_INDEX;
	s = f_settings + player->identifier - PLAYER_INDEX;
	/* Exit if not NPC and not mourning and not celebrating */
	if (!player->npc && x->status != AIS_MOURN && x->status != AIS_CELEBRATE)
		return;
	switch (x->status) {
		case AIS_READY:
			/* Initialize using the randomizer and add in tit-for-tat, persistence, and focus on human */
			for (i = PLAYER_INDEX, j = 0, t = g_vehicles + PLAYER_INDEX; i < PLAYER_LIMIT; ++i, ++j, ++t) {
				if (t->active && t->identifier != player->identifier) {
					s_tallies[j] = AddRandomValue(s->randomizer, AI_K16, AI_K12, AI_K13);
					if (i == x->attacker)
						s_tallies[j] += s->tit_for_tat;
					if (i == player->target)
						s_tallies[j] += s->persistence;
					if (g_focus_on_human && !t->npc)
						s_tallies[j] += AI_K20;
				} else
					s_tallies[j] = INT16_MIN;
			}
			/* Get the closest target */
			for (i = 0, j = PLAYER_COUNT, current = INT16_MAX, t = g_vehicles + PLAYER_INDEX; i < PLAYER_COUNT; ++i, ++t) {
				if (!t->active || t->identifier == player->identifier)
					continue;
				working = GetXMDirectUnsigned(g_squares, abs(t->x - player->x) >> AI_K17)
						+ GetXMDirectUnsigned(g_squares, abs(t->y - player->y) >> AI_K17)
						+ (GetXMDirectUnsigned(g_squares, abs(t->z - player->z) >> AI_K17) << AI_K18);
				if (current > working || (current == working && GetRandomByte(AI_K19))) {
					current = working;
					j = i;
				}
			}
			if (j != PLAYER_COUNT)
				s_tallies[j] += s->proximity;
			/* Get the target that hit the player the most */
			for (i = 0, j = PLAYER_COUNT, current = INT16_MIN, t = g_vehicles + PLAYER_INDEX; i < PLAYER_COUNT; ++i, ++t) {
				if (!t->active || t->identifier == player->identifier)
					continue;
				working = x->attackers[i];
				if (current < working || (current == working && GetRandomByte(AI_K19))) {
					current = working;
					j = i;
				}
			}
			if (j != PLAYER_COUNT)
				s_tallies[j] += s->revenge;
			/* Get the target with the lowest health */
			for (i = 0, j = PLAYER_COUNT, current = INT16_MAX, t = g_vehicles + PLAYER_INDEX; i < PLAYER_COUNT; ++i, ++t) {
				if (!t->active || t->identifier == player->identifier)
					continue;
				working = t->health;
				if (current > working || (current == working && GetRandomByte(AI_K19))) {
					current = working;
					j = i;
				}
			}
			if (j != PLAYER_COUNT)
				s_tallies[j] += s->lowest_health;
			/* Get the target with the lowest level */
			for (i = 0, j = PLAYER_COUNT, current = INT16_MAX, t = g_vehicles + PLAYER_INDEX; i < PLAYER_COUNT; ++i, ++t) {
				if (!t->active || t->identifier == player->identifier)
					continue;
				working = t->level;
				if (current > working || (current == working && GetRandomByte(AI_K19))) {
					current = working;
					j = i;
				}
			}
			if (j != PLAYER_COUNT)
				s_tallies[j] += s->lowest_level;
			/* Pick the target with the highest tally */
			for (i = 0, j = PLAYER_COUNT, current = INT16_MIN; i < PLAYER_COUNT; ++i) {
				working = s_tallies[i];
				if (current < working || (current == working && GetRandomByte(AI_K19))) {
					current = working;
					j = i;
				}
			}
			if (j != PLAYER_COUNT) {
				player->target = j + PLAYER_INDEX;
				k = s->randomizer >> AI_K24;
				if (!k) k = AI_K12;
				x->action_cd = AddRandomValue(k, s->pursue, AI_K12, AI_K13);
				x->status = AIS_PURSUE;
			}
			break;
		case AIS_PURSUE:
			t = g_vehicles + player->target;
			if (t->active) {
				/* Will need these */
				dx = t->x - player->x;
				dy = t->y - player->y;
				dz = t->z - player->z;
				/* Front or back */
				fb = SpcMul16(dx, player->sin) + SpcMul16(dy, player->cos);
				/* Left or right */
				lr = SpcMul16(dx, player->cos) - SpcMul16(dy, player->sin);
				/* Steer towards */
				if (fb > TGT_NR_XY_TOL) {
					if (lr < -TGT_NR_XY_TOL)
						player->a_delta = (player->last_a_delta < 0)? -AI_K48: -AI_K47;
					else if (lr > TGT_NR_XY_TOL)
						player->a_delta = (player->last_a_delta > 0)? AI_K48: AI_K47;
				} else if (fb < -TGT_NR_XY_TOL) {
					if (lr < 0)
						player->a_delta = -AI_K48;
					else if (lr > 0)
						player->a_delta = AI_K48;
					else
						/* Use a constant to prevent "jittering" */
						player->a_delta = (player->identifier & AI_K49)? -AI_K50: AI_K50;
					/* To reduce endgame "circling" */
					ReportToAI(player, EVT_PLAYER_IMPEDED, IMP_LOW);
				}
				player->last_a_delta = player->a_delta;
				/* Try to match height */
				if (player->airborne) {
					if (dz < -VEHICLE_Z_TOL)
						player->z_delta = (player->last_z_delta < 0)? -AI_K48: -AI_K47;
					else if (dz > VEHICLE_Z_TOL)
						player->z_delta = (player->last_z_delta > 0)? AI_K48: AI_K47;
					player->last_z_delta = player->z_delta;
				} else
					++player->gear;
				/* Fire */
				if (!player->loading_cd && abs(dz) < VEHICLE_Z_TOL) {
					/* Don't need signs any more */
					dx = abs(dx);
					dy = abs(dy);
					/* Really close, just fire */
					if (dx < TGT_FR_XY_TOL && dy < TGT_FR_XY_TOL)
						player->firing = true;
					/* Not so close, check accuracy */
					else if (fb > 0 && abs(lr) < TGT_FR_XY_TOL) {
						dx >>= AI_K51;
						dy >>= AI_K51;
						if ((AI_K52 - GetXMDirectUnsigned(g_squares, dx) - GetXMDirectUnsigned(g_squares, dy)) >> AI_K53 >= s->accuracy)
							player->firing = true;
					}
				}
				/* Countdown */
				--x->action_cd;
				if (x->action_cd <= 0) {
					x->status = AIS_READY;
					/* Player was not successful in pursuing */
					ReportToAI(player, EVT_PLAYER_IMPEDED, IMP_EXTREME);
				}
			} else
				x->status = AIS_READY;
			break;
		case AIS_CELEBRATE:
			/* Countdown */
			--x->action_cd;
			if (x->action_cd <= 0) {
				UpdateSettings(s);
				g_frame_counter = 0;
				/* Set up a new game */
				g_human_joined = false;
				g_focus_on_human = false;
				g_no_refresh_at_last_level = false;
				g_arena_index = 0;
				InitPlayers();
				InitAI();
			}
			break;
		case AIS_ESCAPE:
		case AIS_EVADE:
		case AIS_AVOID:
			player->a_delta = x->a_action;
			if (player->airborne)
				player->z_delta = x->z_action;
			else
				player->gear += x->g_action;
			/* Countdown */
			--x->action_cd;
			if (x->action_cd <= 0)
				x->status = AIS_READY;
			break;
		case AIS_MOURN:
			/* Countdown */
			--x->action_cd;
			if (x->action_cd <= 0) {
				if (g_vehicle_index == player->identifier) {
					/* Find the next active player or NPC, if any */
					for (i = 1; i < PLAYER_COUNT; ++i) {
						j = g_vehicle_index + i;
						if (j >= PLAYER_LIMIT)
							j = PLAYER_INDEX;
						if (g_vehicles[j].active) {
							g_vehicle_index = j;
							break;
						}
					}
				}
				x->status = AIS_DEAD;
			}
			break;
	}
}

void MissileAI(VEHICLE *missile)
{
	VEHICLE *player;
	int16_t delta;
	/* Just chase the target! This logic will work for land and airborne */
	if (missile->active && missile->target < PLAYER_LIMIT) {
		player = g_vehicles + missile->target;
		if (player->active) {
			delta = SpcMul16(player->x - missile->x, missile->cos) - SpcMul16(player->y - missile->y, missile->sin);
			if (delta < -MSS_XY_TOL)
				missile->a_delta = -missile->mss_delta;
			else if (delta > MSS_XY_TOL)
				missile->a_delta = missile->mss_delta;
			delta = player->z - missile->z;
			if (delta > VEHICLE_Z_TOL)
				missile->z_delta = missile->mss_delta;
			else if (delta < -VEHICLE_Z_TOL)
				missile->z_delta = -missile->mss_delta;
		}
	}
}

void ReportToAI(VEHICLE *player, AI_EVENT event, int16_t extra)
{
	AI_CURRENT_STATE *x;
	AI_SETTINGS *s;
	VEHICLE *v;
	uint8_t a, i, j, t;

	/* Will need these */
	x = f_current_state + player->identifier - PLAYER_INDEX;
	s = f_settings + player->identifier - PLAYER_INDEX;
	/* If celebrating, ignore all reports */
	if (x->status == AIS_CELEBRATE)
		return;
	switch (event) {
		case EVT_HUMAN_JOINED:
			/* extra = game mode */
			player = g_vehicles + HUMAN_ID;
			if (player->npc) {
				player->active = true;
				player->npc = false;
				player->target = PLAYER_LIMIT;
				player->health = PLAYER_HEALTH;
				player->appearance[APP_AUX] = g_human_id_aux;
				g_human_joined = true;
			}
			if (player->active) {
				g_vehicle_index = player->identifier;
				s->identifier = AI_K04;
				s->parent = AI_K04;
				/* Set flags according to mode */
				switch (extra) {
					case MD_JOIN_AS_NORMAL:
						g_focus_on_human = false;
						g_no_refresh_at_last_level = false;
						break;
					case MD_FOCUS_ON_HUMAN:
						g_focus_on_human = true;
						g_no_refresh_at_last_level = false;
						break;
					case MD_NO_REFRESH_AT_LAST_LEVEL:
						g_no_refresh_at_last_level = true;
						g_focus_on_human = false;
						break;
				}
			}
			break;
		case EVT_NEW_ARENA:
			/* extra = arena index */
			for (i = 0, x = f_current_state, s = f_settings; i < PLAYER_COUNT; ++i, ++x, ++s) {
				/* Reset stuck countdown */
				x->impeded_cd = s->impeded_lmt;
				/* No longer escaping or avoiding */
				if (x->status == AIS_ESCAPE || x->status == AIS_AVOID)
					x->status = AIS_READY;
			}
			break;
		case EVT_SWITCHED_FOCUS:
			/* extra = address of character to provide identifier! */
			*(char *)extra = s->identifier != AI_K04? SM_CONVERT_TO_UPPER + s->identifier: ' ';
			for (i = 0, x = f_current_state; i < PLAYER_COUNT; ++i, ++x) {
				/* No longer mourning */
				if (x->status == AIS_MOURN)
					x->status = AIS_DEAD;
			}
			break;
		case EVT_PLAYER_CORNERED:
			/* extra = not used */
			/* Fixed action to avoid being cornered for an extended period of time */
			/* No need to change behaviour if already avoiding */
			if (x->status != AIS_AVOID) {
				x->status = AIS_AVOID;
				x->action_cd = AddRandomValue(AI_K42, AI_K41, AI_K41 - AI_K42, AI_K41 + AI_K42);
				SetAZGActions(x);
			}
			break;
		case EVT_PLAYER_IMPEDED:
			/* extra = severity */
			x->impeded_cd += extra;
			if (x->impeded_cd <= 0) {
				x->impeded_cd = s->impeded_lmt;
				/* No need to change behaviour if already escaping, evading, or avoiding */
				if (x->status != AIS_ESCAPE && x->status != AIS_EVADE && x->status != AIS_AVOID) {
					/* Need to mix things up a bit */
					x->status = AIS_ESCAPE;
					t = s->randomizer >> AI_K24;
					if (!t) t = AI_K12;
					x->action_cd = AddRandomValue(t, s->escape, AI_K12, AI_K13);
					/* If there are fewer players, then don't need to escape for so long */
					switch (g_active_players) {
						case 3:
							x->action_cd >>= AI_K38;
							if (x->action_cd < AI_K40)
								x->action_cd = AI_K40;
							break;
						case 2:
							x->action_cd >>= AI_K39;
							if (x->action_cd < AI_K40)
								x->action_cd = AI_K40;
							break;
					}
					SetAZGActions(x);
				}
			}
			break;
		case EVT_REACHED_TOP:
			/* extra = not used */
			if (x->z_action < 0)
				x->z_action = -x->z_action;
			break;
		case EVT_REACHED_BOTTOM:
			/* extra = not used */
			if (x->z_action > 0)
				x->z_action = -x->z_action;
			break;
		case EVT_DAMAGED_PLAYER:
			/* extra = identifier of attacker */
			a = extra - PLAYER_INDEX;
			/* Keep track of statistics */
			x->attacker = extra;
			++x->attackers[a];
			x->abuse_cd -= g_vehicles[extra].damage << AI_K27;
			if (x->abuse_cd <= 0) {
				x->abuse_cd = s->abuse_lmt;
				/* No need to change behaviour if already evading or avoiding */
				if (x->status != AIS_EVADE && x->status != AIS_AVOID) {
					/* Need to evade attacker! */
					x->status = AIS_EVADE;
					t = s->randomizer >> AI_K24;
					if (!t) t = AI_K12;
					x->action_cd = AddRandomValue(t, s->evade, AI_K12, AI_K13);
					SetAZGActions(x);
				}
			}
			/* Attacker gets abuse and stuck reset */
			x = f_current_state + a;
			s = f_settings + a;
			x->abuse_cd = s->abuse_lmt;
			x->impeded_cd = s->impeded_lmt;
			break;
		case EVT_ADVANCED_PLAYER:
			/* extra = identifier of attacker */
			a = extra - PLAYER_INDEX;
			/* Adjust scores */
			s->score += AI_K21;
			f_settings[a].score += AI_K22;
			break;
		case EVT_ELIMINATED_PLAYER:
			/* extra = identifier of attacker */
			a = extra - PLAYER_INDEX;
			/* Pause a bit to mourn */
			x->status = AIS_MOURN;
			x->action_cd = AI_K25;
			/* Remove eliminated player from consideration */
			for (i = 0, x = f_current_state, v = g_vehicles + PLAYER_INDEX, j = player->identifier - PLAYER_INDEX; i < PLAYER_COUNT; ++i, ++x, ++v) {
				/* Zero counts for eliminated player */
				if (x->attacker == player->identifier)
					x->attacker = PLAYER_LIMIT;
				x->attackers[j] = 0;
				/* Change targets */
				if (x->status == AIS_PURSUE && v->target == player->identifier)
					x->status = AIS_READY;
			}
			/* Adjust score for attacker */
			f_settings[a].score += AI_K23;
			break;
		case EVT_WINNING_PLAYER:
			/* extra = not used */
			/* Pause a bit to celebrate */
			x->status = AIS_CELEBRATE;
			x->action_cd = AI_K26;
			SetAZGActions(x);
			break;
	}
}

static void SetAZGActions(AI_CURRENT_STATE *x)
{
	int8_t a_action;
	switch (x->status) {
		case AIS_AVOID:
			x->a_action = GetRandomByte(AI_K43)? -AI_K44: AI_K44;
			break;
		default:
			do
				a_action = GetRandomByte(AI_K28) + AI_K29;
			while (!a_action || x->a_action == a_action);
			x->a_action = a_action;
			break;
	}
	switch (x->status) {
		case AIS_ESCAPE:
			x->z_action = GetRandomByte(AI_K30) + AI_K31;
			break;
		case AIS_EVADE:
			x->z_action = GetRandomByte(AI_K32) + AI_K33;
			break;
		case AIS_AVOID:
			x->z_action = AI_K45;
			break;
		case AIS_CELEBRATE:
			x->z_action = GetRandomByte(AI_K34) + AI_K35;
			break;
	}
	switch (x->status) {
		case AIS_AVOID:
			x->g_action = AI_K46;
			break;
		default:
			x->g_action = GetRandomByte(AI_K36) + AI_K37;
			break;
	}
}
