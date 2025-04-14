/*
    Contents: "AI" C source code (c) 2025
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include <stdio.h>
#include <time.h>
#include "explore.h"
#include "vera.h"

/* Defining constants generically for now, once they are all defined, will come up with a unified naming convention */

#define AI_K11	'a'	/* When initializing AI.DATA, this is the identifier of the first player */
#define AI_K12	'd'	/* When initializing AI.DATA, this is the identifier of the last player */

#define AI_K01	-79	/* To convert AI.DAT file size to randomizer "slope numerator" */
#define AI_K02	1417	/* To convert AI.DAT file size to randomizer "slope denominator" */
#define AI_K03	63	/* To convert AI.DAT file size to randomizer "intercept" */

#define AI_K04	-120	/* Lower bounds of full range setting */
#define AI_K05	120	/* Upper bounds of full range setting */
#define AI_K08	0	/* Middle of full range setting */
#define AI_K06	1	/* Lower bounds of half range setting */
#define AI_K07	120	/* Upper bounds of half range setting */
#define AI_K09	60	/* Middle of half range setting */

#define AI_K10	-54	/* Initial score */

#define AI_K13	-5	/* What to change stuck_cd by when the player is stuck */
#define AI_K14	-1	/* What to change stuck_cd by when a missile is stuck */
#define AI_K15	-11	/* What to change score by when player is advanced when health goes to zero */
#define AI_K16	9	/* What to change score by when attacker is advanced */
#define AI_K17	13	/* What to change score by when attacker eliminates another player */

#define AI_K20	4	/* How much to shift randomizer when adding to escape or evade */
#define AI_K18	24	/* What to set action_cd when mourning */
#define AI_K19	48	/* What to set action_cd when celebrating */

#define AI_K21	5	/* Modulus when setting a_action */
#define AI_K22	-2	/* Offset when setting a_action */
#define AI_K23	3	/* Modulus when setting z_action for escape status */
#define AI_K24	0	/* Offset when setting z_action for escape status */
#define AI_K29	3	/* Modulus when setting z_action for evade status */
#define AI_K30	-2	/* Offset when setting z_action for evade status */
#define AI_K25	5	/* Modulus when setting z_action for celebration statuses */
#define AI_K26	-2	/* Offset when setting z_action for celebration statuses */
#define AI_K27	4	/* Modulus when setting g_action */
#define AI_K28	-1	/* Offset when setting g_action */

enum {AIS_READY, AIS_PURSUE, AIS_ESCAPE, AIS_EVADE, AIS_MOURN, AIS_CELEBRATE};

typedef struct {
	char identifer, parent;
	int8_t score, pursue, escape, evade, stuck_lmt, abuse_lmt, accuracy, randomizer,
		proximity, tit_for_tat, revenge, persistence, lowest_health, lowest_level;
} AI_SETTINGS;

typedef struct {
	uint16_t attackers[PLAYER_COUNT];
	uint8_t status, a_action, z_action, g_action, attacker;
	int8_t score, action_cd, stuck_cd, abuse_cd;
} AI_CURRENT_STATE;

/* Internal data */
static const char
	*f_ai_data = "ai.dat",
	*f_working = "working.dat";

static uint8_t
	f_initialize[] = {'_', AI_K10,
		AI_K09, AI_K09, AI_K09, AI_K09, AI_K09, AI_K09,
		AI_K08, AI_K08, AI_K08, AI_K08, AI_K08, AI_K08, AI_K08},
	*f_random_bytes;

static AI_SETTINGS
	*f_settings;

static AI_CURRENT_STATE
	*f_current_state;

static void InitRNG(void);
static void InitSettings(void);
static void InitStatuses(void);
static uint8_t GetRandomByte(uint8_t modulus);
static int8_t AddRandomValue(uint8_t modulus, int8_t current, int8_t minimum, int8_t maximum);
static void SetAZGActions(AI_CURRENT_STATE *x);

void InitAI() {
	InitRNG();
	InitSettings();
	InitStatuses();
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
	return Max8(Min8(current + delta, maximum), minimum);
}

static void InitSettings(void)
{
	FILE *ifile, *ofile;
	AI_SETTINGS *s, t;
	int16_t j;
	uint8_t i, modulus;
	char c;

	/* Set up space */
	if (!f_settings)
		f_settings = malloc(sizeof(AI_SETTINGS) * PLAYER_COUNT);

	/* Check if the file exists */
	ifile = fopen(f_ai_data, "rb");
	if (!ifile) {
		/* It doesn't, so create it */
		ofile = fopen(f_ai_data, "wb");
		if (!ofile)
			ExitProgram(ERR_AI);

		for (c = AI_K11; c <= AI_K12; ++c) {
			fwrite(&c, sizeof(char), 1, ofile);
			fwrite(f_initialize, sizeof(AI_SETTINGS) - sizeof(char), 1, ofile);
		}
		fclose(ofile);
		/* Now should be able to read it! */
		ifile = fopen(f_ai_data, "rb");
		if (!ifile)
			ExitProgram(ERR_AI);
	}
	/* Read in the settings, cycling through to the begining */
	for (i = 0, j = 0; fread(f_settings + i, sizeof(AI_SETTINGS), 1, ifile) ; i = (i + 1) % PLAYER_COUNT)
		j += sizeof(AI_SETTINGS);
	fclose(ifile);

	/* Magic numbers to generate moduli [AI_K06, AI_K09] decreasing with file size */
	modulus = Max16(Min16(MultiplyThenDivide(j, AI_K01, AI_K02) + AI_K03, AI_K09), AI_K06);

	/* Randomize settings */
	for (i = 0, s = f_settings; i < PLAYER_COUNT; ++i, ++s) {
		s->score = AI_K10;
		s->pursue = AddRandomValue(modulus, s->pursue, AI_K06, AI_K07);
		s->escape = AddRandomValue(modulus, s->escape, AI_K06, AI_K07);
		s->evade = AddRandomValue(modulus, s->evade, AI_K06, AI_K07);
		s->stuck_lmt = AddRandomValue(modulus, s->stuck_lmt, AI_K06, AI_K07);
		s->abuse_lmt = AddRandomValue(modulus, s->abuse_lmt, AI_K06, AI_K07);
		s->accuracy = AddRandomValue(modulus, s->accuracy, AI_K06, AI_K07);
		s->randomizer = AddRandomValue(modulus, s->randomizer, AI_K06, AI_K07);
		s->proximity = AddRandomValue(modulus, s->proximity, AI_K04, AI_K05);
		s->tit_for_tat = AddRandomValue(modulus, s->tit_for_tat, AI_K04, AI_K05);
		s->revenge = AddRandomValue(modulus, s->revenge, AI_K04, AI_K05);
		s->persistence = AddRandomValue(modulus, s->persistence, AI_K04, AI_K05);
		s->lowest_health = AddRandomValue(modulus, s->lowest_health, AI_K04, AI_K05);
		s->lowest_level = AddRandomValue(modulus, s->lowest_level, AI_K04, AI_K05);
	}

	/* Append the new settings by copying, adding additional, and removing the original */
	rename(f_ai_data, f_working);
	ifile = fopen(f_working, "rb");
	ofile = fopen(f_ai_data, "wb");
	while (fread(&t, sizeof(AI_SETTINGS), 1, ifile))
		fwrite(&t, sizeof(AI_SETTINGS), 1, ofile);
	fclose(ifile);
	remove(f_working);
	fwrite(f_settings, sizeof(AI_SETTINGS), PLAYER_COUNT, ofile);
	fclose(ofile);
}

static void InitStatuses(void)
{
	AI_CURRENT_STATE *x;
	AI_SETTINGS *s;
	uint8_t i, j;

	/* Set up space */
	if (!f_current_state)
		f_current_state = malloc(sizeof(AI_CURRENT_STATE) * PLAYER_COUNT);

	for (i = 0, x = f_current_state, s = f_settings; i < PLAYER_COUNT; ++i, ++x, ++s) {
		x->status = AIS_READY;
		x->a_action = 0;
		x->z_action = 0;
		x->g_action = 0;
		x->attacker = PLAYER_COUNT;
		for (j = 0; j < PLAYER_COUNT; ++j)
			x->attackers[j] = 0;
		x->action_cd = 0;
		x->stuck_cd = s->stuck_lmt;
		x->abuse_cd = s->abuse_lmt;
	}
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
	VEHICLE *v;
	AI_CURRENT_STATE *x;
	AI_SETTINGS *s;
	uint8_t i, j;

	/* Will need these */
	x = f_current_state + player->identifier;
	s = f_settings + player->identifier;

	switch (event) {
		case AIE_NEW_ARENA:
			/* extra = arena index */
			for (i = 0, x = f_current_state, s = f_settings; i < PLAYER_COUNT; ++i, ++x, ++s) {
				/* Reset stuck countdown */
				x->stuck_cd = s->stuck_lmt;
				/* No longer escaping */
				if (x->status == AIS_ESCAPE)
					x->status = AIS_READY;
			}
			break;
		case AIE_SWITCHED_FOCUS:
			/* extra = not used */
			for (i = 0, x = f_current_state; i < PLAYER_COUNT; ++i, ++x) {
				/* No longer mourning */
				if (x->status == AIS_MOURN)
					x->status = AIS_READY;
			}
			break;
		case AIE_STUCK_PLAYER:
		case AIE_STUCK_MISSILE:
			/* extra = wall height */
			if (extra < MAX_XYZ) {
				/* Only difference between STUCK PLAYER and STUCK MISSILE is how much stuck_cd is adjusted */
				x->stuck_cd += (event == AIE_STUCK_PLAYER)? AI_K13: AI_K14;
				if (x->stuck_cd <= 0) {
					/* Need to escape stuck situation! */
					j = s->randomizer >> AI_K20;
					if (!j) j = AI_K06;
					x->status = AIS_ESCAPE;
					x->action_cd = AddRandomValue(j, s->escape, AI_K06, AI_K07);
					x->stuck_cd = s->stuck_lmt;
					SetAZGActions(x);
				}
			}
			break;
		case AIE_DAMAGED_PLAYER:
			/* extra = identifier of attacker */
			/* Keep track of statistics */
			x->attacker = extra;
			x->attackers[extra] += 1;
			x->abuse_cd -= 1;
			if (x->abuse_cd <= 0) {
				/* Need to evade attacker! */
				j = s->randomizer >> AI_K20;
				if (!j) j = AI_K06;
				x->status = AIS_EVADE;
				x->action_cd = AddRandomValue(j, s->evade, AI_K06, AI_K07);
				x->abuse_cd = s->abuse_lmt;
				SetAZGActions(x);
			}
			/* Attacker gets reset */
			x = f_current_state + extra;
			s = f_settings + extra;
			x->abuse_cd = s->abuse_lmt;
			break;
		case AIE_ADVANCED_PLAYER:
			/* extra = identifier of attacker */
			/* Adjust scores */
			s->score += AI_K15;
			s = f_settings + extra;
			s->score += AI_K16;
			break;
		case AIE_ELIMINATED_PLAYER:
			/* extra = identifier of attacker */
			/* Pause a bit to mourn */
			x->status = AIS_MOURN;
			x->action_cd = AI_K18;
			/* Remove eliminated player from consideration */
			for (i = 0, x = f_current_state, v = g_vehicles, j = player->identifier; i < PLAYER_COUNT; ++i, ++x, ++v) {
				/* Zero counts for eliminated player */
				if (x->attacker == j)
					x->attacker = PLAYER_COUNT;
				x->attackers[j] = 0;
				/* Change targets */
				if (x->status == AIS_PURSUE && v->target == j)
					x->status = AIS_READY;
			}
			/* Adjust score for attacker */
			s = f_settings + extra;
			s->score += AI_K17;
			break;
		case AIE_WINNING_PLAYER:
			/* extra = not used */
			/* Pause a bit to celebrate */
			x->status = AIS_CELEBRATE;
			x->action_cd = AI_K19;
			SetAZGActions(x);
			break;
	}
}

static void SetAZGActions(AI_CURRENT_STATE *x) {
	x->a_action = GetRandomByte(AI_K21) + AI_K22;
	switch (x->status) {
		case AIS_ESCAPE:
			x->z_action = GetRandomByte(AI_K23) + AI_K24;
			break;
		case AIS_EVADE:
			x->z_action = GetRandomByte(AI_K29) + AI_K30;
			break;
		case AIS_CELEBRATE:
			x->z_action = GetRandomByte(AI_K25) + AI_K26;
			break;
	}
	x->g_action = GetRandomByte(AI_K27) + AI_K28;
}
