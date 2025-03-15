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

/* Error messages */
const char *strings0[] = {
	/*ERR_FO*/ "\nThe data file has been erased or\nrenamed. Recreate or restore from\nbackup.\n",
	/*ERR_FC*/ "\nThe data file has been corrupted.\nRecreate or restore from backup.\n",
	/*ERR_XM*/ "\nA banked memory request larger than 8K\nwas requested.\n",
	/*ERR_GI*/ "\nThere is a problem involving graphics\ninitialization.\n",
	/*ERR_OB*/ "\nThere was an attempt to access an array\nout of bounds.\n",
	/*ERR_DC*/ "\nThe display configuration has been\ncorrupted.\n"
};

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
	"                         "
};

/* Order of the sounds is missile firing, missile exploding, and bell ringing */

const uint8_t waveforms0[] = {VERA_PSG_BUZZ_WAVEFORM, VERA_PSG_NOISE_WAVEFORM, VERA_PSG_CLEAR_WAVEFORM};

const float frequencies0[] = {233.08f /* A♯3 */, 164.81f /* E3 */, 466.16f /* A♯4 */};

const uint8_t envelopes0[] = {
	32, 48, 42, 36, 24, 18, 12, 6, 3, 2, 1, 0,
	63, 47, 35, 26, 19, 14, 10, 7, 5, 3, 2, 1, 0,
	4, 8, 13, 49, 62, 37, 28, 21, 16, 11, 9, 63, 47, 35, 26, 19, 14, 10, 7, 5, 3, 2, 1, 0
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

static void OutputStrings(FILE *ofile);
static void OutputTrigData(FILE *ofile);
static void OutputArenaData(FILE *ofile);
static void OutputVehicleData(FILE *ofile);
static void OutputSoundData(FILE *ofile);
static void OutputLevelData(FILE *ofile);

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

	if (!(ofile = fopen("explore.dat", "wb"))) {
		fputs("\nCould not create EXPLORE.DAT!\n", stderr);
		return EXIT_FAILURE;
	}

	/* Set up the error messages first so that we can display them if there's a subsequent problem */
	OutputStrings(ofile);

	/* The trigonometric data */
	OutputTrigData(ofile);

	/* The remaining data */
	OutputArenaData(ofile);
	OutputVehicleData(ofile);
	OutputSoundData(ofile);
	OutputLevelData(ofile);

	/* Indicate end-of-data */
	fputc(CODE_EF, ofile);
	fclose(ofile);

	return EXIT_SUCCESS;
}

static void OutputStrings(FILE *ofile)
{
	const char **strings = strings0;
	int16_t t, u, length, max, count = sizeof(strings0) / sizeof(char *);

	fputs("\nError Messages ", stdout);
	fputc(CODE_EM, ofile);
	/* Get the longest string */
	for (t = 0, max = 0; t < count; ++t) {
		length = strlen(strings[t]);
		if (max < length) max = length;
	}
	/* Add one for NULL terminator */
	++max;
	/* Write out the size */
	fwrite (&max, sizeof(int16_t), 1, ofile);
	/* Write out the strings */
	for (t = 0; t < count; ++t) {
		length = strlen(strings[t]);
		fputs(strings[t], ofile);
		for (u = length; u < max; ++u)
			fputc('\0', ofile);
	}
	fputs("done\n", stdout);
}

static void OutputTrigData(FILE *ofile)
{
	int16_t t, tmp;
	double d, i;

	/* Sine and cosine data used throughout */
	fputs("Trig Data ", stdout);
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
				count += 1;
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
	static const char *s_ordinals[] = {
		"\n  1 ", "\n  2 ", "\n  3 ", "\n  4 ", "\n  5 ", "\n  6 ",
		"\n  7 ", "\n  8 ", "\n  9 ", "\n 10 ", "\n 11 ", "\n 12 "
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

static void OutputSoundData(FILE *ofile) {
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
			else if (volumes[key] != value)
				fputs("\nEnvelope conflict!\n", stdout);
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
