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
#include "vera.h"

#define DCPI			3.141592653589793

#define ARENA_X_OR_Y(i)		(int16_t)(((i) + 0.5) * 1024.0)
#define ARENA_HEIGHT(c)		(int16_t)(16384.0 * (0.075 * (c) - 2.225))

#define VEH_X_LMT		25
#define VEH_Y_LMT		25
#define MAX_VEH_VERTICES	6	/* Maximum V = 15 */
#define MAX_VEH_SEGMENTS	6	/* Maximum S = V(V - 1)/2 */

#define MIN_VEH_SCALE		0.5625
#define MAX_VEH_SCALE		1.1250
#define VEH_XY_OFFSET		12.5

/*
   Heights from lowest to highest: ! # $ % & * +, using formula 0.075 * ascii(x) - 2.225

	!   25%
	#   40%
	$   47%
	%   55%
	&   63%
	*   92%
	+  100%
*/
const char *arena_vertices0 = "!#$%&*+";

const char *arenas0[] = {
	/* a00-arena-1 - welcome */
	"+--------------+"
	"|              |"
	"|              |"
	"|              |"
	"|   #------#   |"
	"|   |      |   |"
	"|   #      #   |"
	"|              |"
	"|              |"
	"|   #      #   |"
	"|   |      |   |"
	"|   #------#   |"
	"|              |"
	"|              |"
	"|              |"
	"+--------------+",
	/* a01-arena-2 - north/south */
	"+--------------+"
	"|              |"
	"|              |"
	"|              |"
	"|  #        #  |"
	"|  |  %  %  |  |"
	"|  |  |  |  |  |"
	"|  |  |  |  |  |"
	"|  |  |  |  |  |"
	"|  |  |  |  |  |"
	"|  |  %  %  |  |"
	"|  #        #  |"
	"|              |"
	"|              |"
	"|              |"
	"+--------------+",
	/* a02-arena-3 - isolation */
	"+--------------+"
	"|              |"
	"|              |"
	"|  *        *  |"
	"|  |        |  |"
	"|  |        |  |"
	"|  *--------*  |"
	"|              |"
	"|              |"
	"|  *--------*  |"
	"|  |        |  |"
	"|  |        |  |"
	"|  *        *  |"
	"|              |"
	"|              |"
	"+--------------+",
	/* a03-arena-4 - east/west */
	"+--------------+"
	"|              |"
	"|              |"
	"|   &------&   |"
	"|              |"
	"|              |"
	"|    $----$    |"
	"|              |"
	"|              |"
	"|    $----$    |"
	"|              |"
	"|              |"
	"|   &------&   |"
	"|              |"
	"|              |"
	"+--------------+",
	/* a04-arena-5 - corners */
	"+--------------+"
	"|              |"
	"|              |"
	"|     $        |"
	"|     |        |"
	"|     |        |"
	"|     |        |"
	"|  $--$        |"
	"|        $--$  |"
	"|        |     |"
	"|        |     |"
	"|        |     |"
	"|        $     |"
	"|              |"
	"|              |"
	"+--------------+",
	/* a05-arena-6 - pillars */
	"+--------------+"
	"|              |"
	"|              |"
	"|              |"
	"|      **      |"
	"|      **      |"
	"|              |"
	"|              |"
	"|              |"
	"|              |"
	"|      **      |"
	"|      **      |"
	"|              |"
	"|              |"
	"|              |"
	"+--------------+"
};

const char *vehicle_vertices0 = "@";

const char *vehicles0[] = {
	/* v00-airborne-1 - slow w/ missile 1 */
	"            @            "
	"           + +           "
	"          +   +          "
	"          +   +          "
	"         +     +         "
	"        +       +        "
	"       +         +       "
	"      +           +      "
	"      +           +      "
	"     +             +     "
	"    +               +    "
	"   +                 +   "
	"  +                   +  "
	"  +                   +  "
	" +                     + "
	"@                       @"
	" +                     + "
	"  +                   +  "
	"  +                   +  "
	"   +                 +   "
	"    +               +    "
	"     +             +     "
	"     +             +     "
	"      +           +      "
	"       @+++++++++@       ",
	/* v01-airborne-2 - fast w/ missile 1 */
	"        @+++++++@        "
	"        +       +        "
	"       +         +       "
	"       +         +       "
	"       +         +       "
	"      +           +      "
	"      +           +      "
	"      +           +      "
	"     +             +     "
	"     +             +     "
	"     +             +     "
	"    +               +    "
	"    +               +    "
	"    +               +    "
	"   +                 +   "
	"   +                 +   "
	"   +                 +   "
	"  +         @         +  "
	"  +        +++        +  "
	"  +      ++   ++      +  "
	" +     ++       ++     + "
	" +   ++           ++   + "
	" + ++               ++ + "
	"+++                   +++"
	"@                       @",
	/* v02-airborne-3 - slow w/ missile 2 */
	"           +@+           "
	"          +   +          "
	"          +   +          "
	"         +     +         "
	"        +       +        "
	"        +       +        "
	"       +         +       "
	"      +           +      "
	"      +           +      "
	"     +             +     "
	"     +             +     "
	"    +               +    "
	"   +                 +   "
	"   +                 +   "
	"  +                   +  "
	" +                     + "
	" +                     + "
	"@+++++++++++++++++++++++@"
	" +                     + "
	"  ++                 ++  "
	"    ++             ++    "
	"      ++         ++      "
	"        +       +        "
	"         ++   ++         "
	"           +@+           ",
	/* v03-airborne-4 - fast w/ missile 2 */
	"           +@+           "
	"           +++           "
	"          + + +          "
	"          + + +          "
	"         +  +  +         "
	"         +  +  +         "
	"        +   +   +        "
	"        +   +   +        "
	"       +    +    +       "
	"       +    +    +       "
	"      +     +     +      "
	"      +     +     +      "
	"      +     +     +      "
	"     +      +      +     "
	"     +      +      +     "
	"    +       +       +    "
	"    +       +       +    "
	"   +        +        +   "
	"   +        +        +   "
	"  +        +@+        +  "
	"  +      ++   ++      +  "
	" +     ++       ++     + "
	" +   ++           ++   + "
	"+  ++               ++  +"
	"@++                   ++@",
	/* v04-airborne-5 - slow w/ missile 3 */
	"      @           @      "
	"      +           +      "
	"     +             +     "
	"     +             +     "
	"    +               +    "
	"    +               +    "
	"    +       @       +    "
	"   +       +++       +   "
	"   +      ++ ++      +   "
	"  +      ++   ++      +  "
	"  +     +       +     +  "
	"  +    +         +    +  "
	" +   ++           ++   + "
	" +  ++             ++  + "
	"+  +                 +  +"
	"+++                   +++"
	"@                       @"
	" ++                   ++ "
	"   +                 +   "
	"    ++             ++    "
	"      +           +      "
	"       ++       ++       "
	"         +     +         "
	"          ++ ++          "
	"            @            ",
	/* v05-airborne-6 - fast w/ missile 3 */
	"            @            "
	"            +            "
	"           + +           "
	"           + +           "
	"           + +           "
	"          +   +          "
	"          +   +          "
	"          +   +          "
	"@        +     +        @"
	"+        +     +        +"
	"+        +     +        +"
	" +      +       +      + "
	" +      +       +      + "
	" +     +         +     + "
	" +     +         +     + "
	"  +    +         +    +  "
	"  +   +           +   +  "
	"  +   +           +   +  "
	"  +   +     @     +   +  "
	"   + +     + +     + +   "
	"   + +    +   +    + +   "
	"   + +  ++     ++  + +   "
	"   ++  +         +  ++   "
	"    + +           + +    "
	"    @+             +@    ",
	/* v06-missile-1 */
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"            @            "
	"            +            "
	"            +            "
	"            +            "
	"            +            "
	"            +            "
	"            @            "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         ",
	/* v07-missile-2 */
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"            @            "
	"           + +           "
	"           + +           "
	"          +   +          "
	"          +   +          "
	"         @     @         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         ",
	/* v08-missile-3 */
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"           +@+           "
	"          ++ ++          "
	"         ++   ++         "
	"         @     @         "
	"         ++   ++         "
	"          ++ ++          "
	"           +@+           "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         ",
	/* v09-explosion-1 */
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"            @            "
	"            +            "
	"            +            "
	"            +            "
	"        @+++++++@        "
	"            +            "
	"            +            "
	"            +            "
	"            @            "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         ",
	/* v10-explosion-2 */
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"         @     @         "
	"          +   +          "
	"           + +           "
	"            +            "
	"           + +           "
	"          +   +          "
	"         @     @         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         ",
	/* v11-human-id */
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"            @            "
	"           +++           "
	"          ++ ++          "
	"         ++   ++         "
	"        @+++++++@        "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
	"                         "
};

/* Order of the sounds is missile firing, missile exploding, and bell ringing */

const uint8_t waveforms0[] = {VERA_PSG_BUZZ_WAVEFORM, VERA_PSG_NOISE_WAVEFORM, VERA_PSG_CLEAR_WAVEFORM};

const float frequencies0[] = {233.08f /* A♯3 */, 164.81f /* E3 */, 466.16f /* A♯4 */};

const uint8_t envelopes0[] = {
	32, 48, 42, 36, 24, 18, 12, 6, 3, 2, 1, 0,
	63, 47, 35, 26, 19, 14, 10, 7, 5, 3, 2, 1, 0,
	4, 8, 13, 20, 31, 49, 60, 62, 37, 28, 21, 16, 11, 9, 61, 63, 47, 35, 26, 19, 14, 10, 7, 5, 3, 2, 1, 0
};

#define PER_LEVEL	7

const uint8_t levels0[] = {
/*	Arena	Player	Airbrn	Gear	Missile	Delta	Damage */
	0,	0,	1,	1,	6,	1,	3,
	1,	1,	1,	2,	6,	1,	3,
	2,	2,	1,	1,	7,	2,	5,
	3,	3,	1,	2,	7,	2,	5,
	4,	4,	1,	1,	8,	3,	8,
	5,	5,	1,	2,	8,	3,	8
};

static void OutputTrigData(FILE *ofile);
static void OutputCRC8Data(FILE *ofile);
static void OutputArenaData(FILE *ofile);
static void OutputVehicleData(FILE *ofile);
static void OutputLevelData(FILE *ofile);
static void OutputSoundData(FILE *ofile);

static void OutputArena16x16(const char *arena, FILE *ofile);
static VERTEX *OutputArenaVertices(const char *arena, int16_t limit, FILE *ofile);
static void OutputArenaSegments(const char *arena, VERTEX *vertices, int16_t limit, FILE *ofile);

static int16_t CountArenaVertices(const char *arena);
static int16_t CountArenaSegments(const char *arena);

static char ArenaCharacterAt(const char *arena, int16_t x, int16_t y);
static int16_t IndexOfArenaVertex(VERTEX *vertices, VERTEX *vertex, int16_t count);
static int CompareArenaVertices(const void *p, const void *q);

static OFFSET *GetVehicleVertices(const char *vehicle, int16_t limit);
static int16_t CountVehicleVertices(const char *vehicle);
static void OutputVehicleSegments(const char *vehicle, OFFSET *vertices, int16_t limit, FILE *ofile);
static void OutputVehicleVertices(const char *vehicle, OFFSET *vertices, int16_t limit, FILE *ofile);

static bool VehicleVerticesAreConnected(const char *vehicle, OFFSET *v1, OFFSET *v2);
static char VehicleCharacterAt(const char *vehicle, int16_t x, int16_t y);
static int CompareVehicleVertices(const void *p, const void *q);

int main(void)
{
	FILE *ofile;

	if (!(ofile = fopen("pvp-ai.dat", "wb"))) {
		fputs("\nCould not create PVP-AI.DAT!\n", stderr);
		return EXIT_FAILURE;
	}

	/* The trigonometric and CRC8 data */
	OutputTrigData(ofile);
	OutputCRC8Data(ofile);

	/* Arena, vehicle, level, and sound data */
	OutputArenaData(ofile);
	OutputVehicleData(ofile);
	OutputLevelData(ofile);
	OutputSoundData(ofile);

	/* Indicate end-of-data */
	fputc(CODE_EF, ofile);
	fclose(ofile);

	return EXIT_SUCCESS;
}

static void OutputTrigData(FILE *ofile)
{
	int16_t t, tmp;
	double d, i;

	/* Sine and cosine data used throughout */
	fputs("\nTrig Data ", stdout);
	fputc(CODE_TD, ofile);
	d = 0.0;
	i = 2.0 * DCPI / SCALE_FC;
	for (t = 0; t < SCALE_FC; ++t, d += i) {
		tmp = (int16_t)(sin(d) * SCALE_1_0);
		fwrite(&tmp, sizeof(int16_t), 1, ofile);
		if (t % 32 == 31) fputc('*', stdout);
	}
	fputs(" done\n", stdout);
}

static void OutputCRC8Data(FILE *ofile)
{
	static const unsigned char CRC8[] = {
		0x00,	0x07,	0x0e,	0x09,	0x1c,	0x1b,	0x12,	0x15,
		0x38,	0x3f,	0x36,	0x31,	0x24,	0x23,	0x2a,	0x2d,
		0x70,	0x77,	0x7e,	0x79,	0x6c,	0x6b,	0x62,	0x65,
		0x48,	0x4f,	0x46,	0x41,	0x54,	0x53,	0x5a,	0x5d,
		0xe0,	0xe7,	0xee,	0xe9,	0xfc,	0xfb,	0xf2,	0xf5,
		0xd8,	0xdf,	0xd6,	0xd1,	0xc4,	0xc3,	0xca,	0xcd,
		0x90,	0x97,	0x9e,	0x99,	0x8c,	0x8b,	0x82,	0x85,
		0xa8,	0xaf,	0xa6,	0xa1,	0xb4,	0xb3,	0xba,	0xbd,
		0xc7,	0xc0,	0xc9,	0xce,	0xdb,	0xdc,	0xd5,	0xd2,
		0xff,	0xf8,	0xf1,	0xf6,	0xe3,	0xe4,	0xed,	0xea,
		0xb7,	0xb0,	0xb9,	0xbe,	0xab,	0xac,	0xa5,	0xa2,
		0x8f,	0x88,	0x81,	0x86,	0x93,	0x94,	0x9d,	0x9a,
		0x27,	0x20,	0x29,	0x2e,	0x3b,	0x3c,	0x35,	0x32,
		0x1f,	0x18,	0x11,	0x16,	0x03,	0x04,	0x0d,	0x0a,
		0x57,	0x50,	0x59,	0x5e,	0x4b,	0x4c,	0x45,	0x42,
		0x6f,	0x68,	0x61,	0x66,	0x73,	0x74,	0x7d,	0x7a,
		0x89,	0x8e,	0x87,	0x80,	0x95,	0x92,	0x9b,	0x9c,
		0xb1,	0xb6,	0xbf,	0xb8,	0xad,	0xaa,	0xa3,	0xa4,
		0xf9,	0xfe,	0xf7,	0xf0,	0xe5,	0xe2,	0xeb,	0xec,
		0xc1,	0xc6,	0xcf,	0xc8,	0xdd,	0xda,	0xd3,	0xd4,
		0x69,	0x6e,	0x67,	0x60,	0x75,	0x72,	0x7b,	0x7c,
		0x51,	0x56,	0x5f,	0x58,	0x4d,	0x4a,	0x43,	0x44,
		0x19,	0x1e,	0x17,	0x10,	0x05,	0x02,	0x0b,	0x0c,
		0x21,	0x26,	0x2f,	0x28,	0x3d,	0x3a,	0x33,	0x34,
		0x4e,	0x49,	0x40,	0x47,	0x52,	0x55,	0x5c,	0x5b,
		0x76,	0x71,	0x78,	0x7f,	0x6a,	0x6d,	0x64,	0x63,
		0x3e,	0x39,	0x30,	0x37,	0x22,	0x25,	0x2c,	0x2b,
		0x06,	0x01,	0x08,	0x0f,	0x1a,	0x1d,	0x14,	0x13,
		0xae,	0xa9,	0xa0,	0xa7,	0xb2,	0xb5,	0xbc,	0xbb,
		0x96,	0x91,	0x98,	0x9f,	0x8a,	0x8d,	0x84,	0x83,
		0xde,	0xd9,	0xd0,	0xd7,	0xc2,	0xc5,	0xcc,	0xcb,
		0xe6,	0xe1,	0xe8,	0xef,	0xfa,	0xfd,	0xf4,	0xf3
	};

	fputs("CRC Data", stdout);
	fputc(CODE_CD, ofile);
	fwrite(&CRC8, sizeof(CRC8), 1, ofile);
	fputs(" done\n", stdout);
}

static void OutputArenaData(FILE *ofile)
{
	int16_t t, tmp, vertices = 0, segments = 0;
	VERTEX *V;
	/* Arena data */
	fputs("Arena Data ", stdout);
	fputc(CODE_AD, ofile);
	/* How many arenas */
	tmp = sizeof(arenas0) / sizeof(char *);
	fwrite(&tmp, sizeof(int16_t), 1, ofile);
	/* Determine the limits */
	for (t = 0; t < sizeof(arenas0) / sizeof(char *); ++t) {
		tmp = CountArenaVertices(arenas0[t]);
		if (vertices < tmp) vertices = tmp;
		tmp = CountArenaSegments(arenas0[t]);
		if (segments < tmp) segments = tmp;
	}
	/* Size of an arena data structure */
	tmp = ARENA_X_LMT * ARENA_Y_LMT * sizeof(int16_t) + (vertices + 1) * sizeof(VERTEX) + (segments + 1) * sizeof(SEGMENT);
	fwrite(&tmp, sizeof(int16_t), 1, ofile);
	/* Max number of vertices */
	fwrite(&vertices, sizeof(int16_t), 1, ofile);
	/* Max number of segments */
	fwrite(&segments, sizeof(int16_t), 1, ofile);
	fputc('*', stdout);
	/* Output the arenas */
	for (t = 0; t < sizeof(arenas0) / sizeof(char *); ++t) {
		OutputArena16x16(arenas0[t], ofile);
		V = OutputArenaVertices(arenas0[t], vertices + 1, ofile);
		OutputArenaSegments(arenas0[t], V, segments + 1, ofile);
		fputc('*', stdout);
	}
	fputs(" done\n", stdout);
}

static void OutputArena16x16(const char *arena, FILE *ofile)
{
	static int16_t *s_heights = NULL;
	char focus;
	int16_t x, y, xt, yt, height;

	if (!s_heights)
		s_heights = malloc(sizeof(int16_t) * ARENA_X_LMT * ARENA_Y_LMT);
	for (y = 0; y < ARENA_Y_LMT; ++y) {
		for (x = 0; x < ARENA_X_LMT; ++x) {
			focus = ArenaCharacterAt(arena, x, y);
			if (strchr(arena_vertices0, focus)) {
				height = ARENA_HEIGHT(focus);
				s_heights[y * ARENA_X_LMT + x] = height;
				for (xt = x + 1; ArenaCharacterAt(arena, xt, y) == '-'; ++xt)
					s_heights[y * ARENA_X_LMT + xt] = height;
				for (yt = y + 1; ArenaCharacterAt(arena, x, yt) == '|'; ++yt)
					s_heights[yt * ARENA_X_LMT + x] = height;
			} else if (focus == ' ')
				s_heights[y * ARENA_X_LMT + x] = 0;
		}
	}
	fwrite(s_heights, sizeof(int16_t), ARENA_X_LMT * ARENA_Y_LMT, ofile);
}

static VERTEX *OutputArenaVertices(const char *arena, int16_t limit, FILE *ofile)
{
	static VERTEX *s_vertices = NULL;
	char focus;
	int16_t i, x, y;

	if (!s_vertices)
		s_vertices = malloc(sizeof(VERTEX) * limit);
	for (y = 0, i = 0; y < ARENA_Y_LMT; ++y) {
		for (x = 0; x < ARENA_X_LMT; ++x) {
			focus = ArenaCharacterAt(arena, x, y);
			if (strchr(arena_vertices0, focus)) {
				s_vertices[i].x = ARENA_X_OR_Y(x);
				s_vertices[i].y = ARENA_X_OR_Y(y);
				s_vertices[i].z = ARENA_HEIGHT(focus);
				++i;
#ifndef ABSTRACT_2D
				s_vertices[i].x = s_vertices[i - 1].x;
				s_vertices[i].y = s_vertices[i - 1].y;
				s_vertices[i].z = 0;
				++i;
#endif
			}
		}
	}
	/* Sorting for easier searching later */
	qsort(s_vertices, i, sizeof(VERTEX), CompareArenaVertices);
	for (; i < limit; ++i)
		s_vertices[i].z = s_vertices[i].y = s_vertices[i].x = -1;
	fwrite(s_vertices, sizeof(VERTEX), limit, ofile);

	/* Will need this in OutputArenaSegments */
	return s_vertices;
}

static void OutputArenaSegments(const char *arena, VERTEX *vertices, int16_t limit, FILE *ofile)
{
	static SEGMENT *s_segments = NULL;
	char focus;
	int16_t i, x, y, xt, yt, count;
#ifndef ABSTRACT_2D
	VERTEX v1;
#endif
	VERTEX v2, v3;

	if (!s_segments)
		s_segments = malloc(sizeof(SEGMENT) * limit);
	count = CountArenaVertices(arena);
	for (y = 0, i = 0; y < ARENA_Y_LMT; ++y) {
		for (x = 0; x < ARENA_X_LMT; ++x) {
			focus = ArenaCharacterAt(arena, x, y);
			if (strchr(arena_vertices0, focus)) {
				v2.x = ARENA_X_OR_Y(x);
				v2.y = ARENA_X_OR_Y(y);
				v2.z = ARENA_HEIGHT(focus);
#ifndef ABSTRACT_2D
				v1.x = v2.x;
				v1.y = v2.y;
				v1.z = 0;
				s_segments[i].index_from = IndexOfArenaVertex(vertices, &v1, count);
				s_segments[i].index_to = IndexOfArenaVertex(vertices, &v2, count);
				++i;
#endif
				for (xt = x + 1; ArenaCharacterAt(arena, xt, y) == '-'; ++xt) ;
				if (ArenaCharacterAt(arena, xt, y) == focus) {
					v3.x = ARENA_X_OR_Y(xt);
					v3.y = v2.y;
					v3.z = v2.z;
					s_segments[i].index_from = IndexOfArenaVertex(vertices, &v2, count);
					s_segments[i].index_to = IndexOfArenaVertex(vertices, &v3, count);
					++i;
#ifndef ABSTRACT_2D
					v3.z = 0;
					s_segments[i].index_from = IndexOfArenaVertex(vertices, &v1, count);
					s_segments[i].index_to = IndexOfArenaVertex(vertices, &v3, count);
					++i;
#endif
				}
				for (yt = y + 1; ArenaCharacterAt(arena, x, yt) == '|'; ++yt) ;
				if (ArenaCharacterAt(arena, x, yt) == focus) {
					v3.x = v2.x;
					v3.y = ARENA_X_OR_Y(yt);
					v3.z = v2.z;
					s_segments[i].index_from = IndexOfArenaVertex(vertices, &v2, count);
					s_segments[i].index_to = IndexOfArenaVertex(vertices, &v3, count);
					++i;
#ifndef ABSTRACT_2D
					v3.z = 0;
					s_segments[i].index_from = IndexOfArenaVertex(vertices, &v1, count);
					s_segments[i].index_to = IndexOfArenaVertex(vertices, &v3, count);
					++i;
#endif
				}
			}
		}
	}
	for (; i < limit; ++i)
		s_segments[i].index_from = s_segments[i].index_to = -1;
	fwrite(s_segments, sizeof(SEGMENT), limit, ofile);
}


static int16_t CountArenaVertices(const char *arena)
{
	int16_t count = 0;

	while (*arena) {
		if (strchr(arena_vertices0, *arena))
			count += 2;
		++arena;
	}

	return count;
}

static int CountArenaSegments(const char *arena)
{
	char focus, tmp;
	int16_t x, y, count = 0;

	for (y = 0; y < ARENA_Y_LMT; ++y) {
		for (x = 0; x < ARENA_X_LMT; ++x) {
			focus = ArenaCharacterAt(arena, x, y);
			if (strchr(arena_vertices0, focus)) {
				++count;
				tmp = ArenaCharacterAt(arena, x + 1, y);
				if (tmp == '-' || tmp == focus)
					count += 2;
				tmp = ArenaCharacterAt(arena, x, y + 1);
				if (tmp == '|' || tmp == focus)
					count += 2;
			}
		}
	}

	return count;
}

static char ArenaCharacterAt(const char *arena, int16_t x, int16_t y)
{
	char result = ' ';

	if (y >= 0 && y < ARENA_Y_LMT && x >= 0 && x < ARENA_X_LMT)
		result = arena[y * ARENA_X_LMT + x];

	return result;
}

static int16_t IndexOfArenaVertex(VERTEX *vertices, VERTEX *vertex, int16_t count)
{
	VERTEX *v = bsearch(vertex, vertices, count, sizeof(VERTEX), CompareArenaVertices);
	return v? (int16_t)(v - vertices): -1;
}

static int CompareArenaVertices(const void *p, const void *q)
{
	int16_t result;
	const VERTEX *a = p, *b = q;

	result = a->z - b->z;
	if (!result)
		result = a->x - b->x;
	if (!result)
		result = a->y - b->y;

	return (int)result;
}

static void OutputVehicleData(FILE *ofile)
{
	/* Literal constants so that we don't have to link in the entire stdio library! */
	static const char *s_ordinals[] = {
		"\n  1 ", "\n  2 ", "\n  3 ", "\n  4 ", "\n  5 ",
		"\n  6 ", "\n  7 ", "\n  8 ", "\n  9 ", "\n 10 ",
		"\n 11 ", "\n 12 ", "\n 13 ", "\n 14 ", "\n 15 "
	};
	int16_t t, tmp, vertices, segments;
	OFFSET *V;

	/* Vehicle data */
	fputs("Vehicle Data", stdout);
	fputc(CODE_VD, ofile);
	/* How many vehicles */
	tmp = sizeof(vehicles0) / sizeof(char *);
	fwrite(&tmp, sizeof(int16_t), 1, ofile);
	/* Using hard-coded limits, easier this way! */
	vertices = MAX_VEH_VERTICES;
	segments = MAX_VEH_SEGMENTS;
	/* Size of a vehicle data structure */
	tmp = (segments + 1) * sizeof(SEGMENT) + vertices * sizeof(OFFSET) * VEHICLE_HGTS * VEHICLE_DIRS;
	fwrite(&tmp, sizeof(int16_t), 1, ofile);
	/* Max number of vertices */
	fwrite(&vertices, sizeof(int16_t), 1, ofile);
	/* Max number of segments */
	fwrite(&segments, sizeof(int16_t), 1, ofile);
	fputs(" *", stdout);
	/* Output the vehicles */
	for (t = 0; t < sizeof(vehicles0) / sizeof(char *); ++t) {
		fputs(s_ordinals[t], stdout);
		V = GetVehicleVertices(vehicles0[t], vertices + 1);
		OutputVehicleSegments(vehicles0[t], V, segments + 1, ofile);
		OutputVehicleVertices(vehicles0[t], V, vertices, ofile);
	}
	fputs(" done\n", stdout);
}

static int16_t CountVehicleVertices(const char *vehicle)
{
	int16_t count = 0;

	while (*vehicle) {
		if (strchr(vehicle_vertices0, *vehicle))
			++count;
		++vehicle;
	}

	return count;
}

static OFFSET *GetVehicleVertices(const char *vehicle, int16_t limit)
{
	static OFFSET *s_vertices = NULL;
	char focus;
	int16_t i, x, y;

	if (!s_vertices)
		s_vertices = malloc(sizeof(OFFSET) * limit);
	for (y = 0, i = 0; y < VEH_Y_LMT; ++y) {
		for (x = 0; x < VEH_X_LMT; ++x) {
			focus = VehicleCharacterAt(vehicle, x, y);
			if (strchr(vehicle_vertices0, focus)) {
				s_vertices[i].x = x;
				s_vertices[i].y = y;
				++i;
				if (i == limit) {
					fputs("\nIncrease MAX_VEH_VERTICES!\n", stderr);
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	/* Sorting for easier searching later */
	qsort(s_vertices, i, sizeof(OFFSET), CompareVehicleVertices);
	for (; i < limit; ++i)
		s_vertices[i].y = s_vertices[i].x = -1;

	/* Will need this in lots of places! */
	return s_vertices;
}

static void OutputVehicleSegments(const char *vehicle, OFFSET *vertices, int16_t limit, FILE *ofile)
{
	static SEGMENT *s_segments = NULL;
	int16_t i, j1, j2, count;

	if (!s_segments)
		s_segments = malloc(sizeof(SEGMENT) * limit);
	count = CountVehicleVertices(vehicle);
	for (i = 0, j1 = 0; j1 < count; ++j1) {
		for (j2 = j1 + 1; j2 < count; ++j2) {
			if (VehicleVerticesAreConnected(vehicle, vertices + j1, vertices + j2)) {
				s_segments[i].index_from = j1;
				s_segments[i].index_to = j2;
				++i;
				if (i == limit) {
					fputs("\nIncrease MAX_VEH_SEGMENTS!\n", stderr);
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	for (; i < limit; ++i)
		s_segments[i].index_from = s_segments[i].index_to = -1;
	fwrite(s_segments, sizeof(SEGMENT), limit, ofile);
}

static void OutputVehicleVertices(const char *vehicle, OFFSET *vertices, int16_t limit, FILE *ofile)
{
	static OFFSET *s_offsets = NULL;
	static double *s_sn, *s_cs;
	int16_t t, u, height, count;
	double d, i, scale, x, y;

	if (!s_offsets) {
		s_offsets = malloc(sizeof(OFFSET) * limit);
		s_sn = malloc(sizeof(double) * VEHICLE_DIRS);
		s_cs = malloc(sizeof(double) * VEHICLE_DIRS);
		d = 0.0;
		i = 2.0 * DCPI / VEHICLE_DIRS;
		for (t = 0; t < VEHICLE_DIRS; ++t, d += i) {
			s_sn[t] = sin(d);
			s_cs[t] = cos(d);
		}
		fputc('*', stdout);
	}

	count = CountVehicleVertices(vehicle);
	i = (MAX_VEH_SCALE - MIN_VEH_SCALE) / (VEHICLE_HGTS - 1);
	for (height = 0, scale = MAX_VEH_SCALE; height < VEHICLE_HGTS; ++height, scale -= i) {
		for (t = 0; t < VEHICLE_DIRS; ++t) {
			for (u = 0; u < count; ++u) {
				x = vertices[u].x - VEH_XY_OFFSET;
				y = vertices[u].y - VEH_XY_OFFSET;
				s_offsets[u].x = round((s_cs[t] * x - s_sn[t] * y) / scale);
				s_offsets[u].y = round((s_sn[t] * x + s_cs[t] * y) / scale);
			}
			for (; u < limit; ++u)
				s_offsets[u].x = s_offsets[u].y = -1;
			fwrite(s_offsets, sizeof(OFFSET), limit, ofile);
		}
		fputc('*', stdout);
	}
}

static bool VehicleVerticesAreConnected(const char *vehicle, OFFSET *v1, OFFSET *v2)
{
	double x1, y1, x2, y2, x3, y3;
	int16_t x, y, count = 0;

	/* Midpoint */
	x2 = (v1->x + v2->x) / 2.0;
	y2 = (v1->y + v2->y) / 2.0;
	for (count = 0, y = -1; y <= 1; ++y) {
		for (x = -1; x <= 1; ++x)
			if (VehicleCharacterAt(vehicle, (int16_t)(x2 + x), (int16_t)(y2 + y)) != ' ')
				++count;
	}
	if (count < 2)
		return false;

	/* Between v1 and midpoint */
	x1 = (v1->x + x2) / 2.0;
	y1 = (v1->y + y2) / 2.0;
	for (count = 0, y = -1; y <= 1; ++y) {
		for (x = -1; x <= 1; ++x)
			if (VehicleCharacterAt(vehicle, (int16_t)(x1 + x), (int16_t)(y1 + y)) != ' ')
				++count;
	}
	if (count < 2)
		return false;

	/* Between midpoint and v2 */
	x3 = (x2 + v2->x) / 2.0;
	y3 = (y2 + v2->y) / 2.0;
	for (count = 0, y = -1; y <= 1; ++y) {
		for (x = -1; x <= 1; ++x)
			if (VehicleCharacterAt(vehicle, (int16_t)(x3 + x), (int16_t)(y3 + y)) != ' ')
				++count;
	}
	if (count < 2)
		return false;

	return true;
}

static char VehicleCharacterAt(const char *vehicle, int16_t x, int16_t y)
{
	char result = ' ';

	if (y >= 0 && y < VEH_Y_LMT && x >= 0 && x < VEH_X_LMT)
		result = vehicle[y * VEH_X_LMT + x];

	return result;
}

static int CompareVehicleVertices(const void *p, const void *q)
{
	int8_t result;
	const OFFSET *a = p, *b = q;

	result = a->x - b->x;
	if (!result)
		result = a->y - b->y;

	return (int)result;
}

static void OutputLevelData(FILE *ofile)
{
	int16_t tmp;

	fputs("Level Data", stdout);
	fputc(CODE_LD, ofile);
	tmp = sizeof(levels0) / sizeof(uint8_t) / PER_LEVEL;
	fwrite(&tmp, sizeof(int16_t), 1, ofile);
	fwrite(levels0, sizeof(uint8_t), sizeof(levels0) / sizeof(uint8_t), ofile);
	fputs(" done\n", stdout);
}

static void OutputSoundData(FILE *ofile)
{
	uint8_t i, j, key, value, initializers[SOUNDS_CNT], volumes[VERA_PSG_VOLUMES];
	uint16_t psg;

	fputs("Sound Data", stdout);
	fputc(CODE_SD, ofile);
	/* Initialize */
	for (i = 0; i < VERA_PSG_VOLUMES; ++i)
		volumes[i] = 0;
	/* Merge the envelopes */
	for (i = j = 0; i < sizeof(envelopes0) / sizeof(uint8_t) - 1; ++i) {
		key = envelopes0[i];
		value = envelopes0[i + 1];
		if (!i)
			initializers[j++] = key;
		if (!key)
			initializers[j++] = value;
		else {
			if (!volumes[key])
				volumes[key] = value;
			else if (volumes[key] != value) {
				fputs("\nEnvelope conflict!\n", stderr);
				exit(EXIT_FAILURE);
			}
		}
	}
	/* Output the settings */
	for (i = 0; i < SOUNDS_CNT; ++i) {
		/* Convert from frequency to PSG value */
		psg = roundf(frequencies0[i] * 2.68435456f);
		fwrite(&psg, sizeof(int16_t), 1, ofile);
		initializers[i] |= VERA_PSG_RL_OUTPUT;
		fwrite(initializers + i, sizeof(uint8_t), 1, ofile);
		fwrite(waveforms0 + i, sizeof(uint8_t), 1, ofile);
	}
	/* Output the merged envelopes as volumes */
	fwrite(volumes, sizeof(uint8_t), VERA_PSG_VOLUMES, ofile);
	fputs(" done\n", stdout);
}

