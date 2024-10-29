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

#define DCPI			3.141592653589793

#define ARENA_X_OR_Y(i)		(int16_t)(((i) + 0.5) * 1024.0)
#define ARENA_HEIGHT(c)		(int16_t)(16384.0 * (0.075 * (c) - 2.225))

#define VEH_X_LIMIT		25
#define VEH_Y_LIMIT		25
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
	/* Arena 1 - welcome */
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
	/* Arena 2 - north/south */
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
	/* Arena 3 - isolation */
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
	/* Arena 4 - east/west */
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
	/* Arena 5 - corners */
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
	/* Arena 6 - pillars */
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
	/* air-level-1 */
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
	/* air-level-2 */
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
	/* air-level-3 */
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
	/* air-level-4 */
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
	/* air-level-5 */
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
	/* air-level-6 */
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
	"    @+             +@    "
};

static void OutputStrings(FILE *ofile);
static void OutputTrigData(FILE *ofile);
static void OutputArenaData(FILE *ofile);
static void OutputVehicleData(FILE *ofile);

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
	tmp = ARENA_X_LIMIT * ARENA_Y_LIMIT * sizeof(int16_t) + (vertices + 1) * sizeof(VERTEX) + (segments + 1) * sizeof(SEGMENT);
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
	static int16_t *working = NULL;
	char focus;
	int16_t x, y, xt, yt, height;

	if (!working)
		working = malloc(sizeof(int16_t) * ARENA_X_LIMIT * ARENA_Y_LIMIT);
	for (y = 0; y < ARENA_Y_LIMIT; ++y) {
		for (x = 0; x < ARENA_X_LIMIT; ++x) {
			focus = ArenaCharacterAt(arena, x, y);
			if (strchr(arena_vertices0, focus)) {
				height = ARENA_HEIGHT(focus);
				working[y * ARENA_X_LIMIT + x] = height;
				for (xt = x + 1; ArenaCharacterAt(arena, xt, y) == '-'; ++xt)
					working[y * ARENA_X_LIMIT + xt] = height;
				for (yt = y + 1; ArenaCharacterAt(arena, x, yt) == '|'; ++yt)
					working[yt * ARENA_X_LIMIT + x] = height;
			} else if (focus == ' ')
				working[y * ARENA_X_LIMIT + x] = 0;
		}
	}
	fwrite(working, sizeof(int16_t), ARENA_X_LIMIT * ARENA_Y_LIMIT, ofile);
}

static VERTEX *OutputArenaVertices(const char *arena, int16_t limit, FILE *ofile)
{
	static VERTEX *vertices = NULL;
	char focus;
	int16_t i, x, y;

	if (!vertices)
		vertices = malloc(sizeof(VERTEX) * limit);
	for (y = 0, i = 0; y < ARENA_Y_LIMIT; ++y) {
		for (x = 0; x < ARENA_X_LIMIT; ++x) {
			focus = ArenaCharacterAt(arena, x, y);
			if (strchr(arena_vertices0, focus)) {
				vertices[i].x = ARENA_X_OR_Y(x);
				vertices[i].y = ARENA_X_OR_Y(y);
				vertices[i].z = ARENA_HEIGHT(focus);
				++i;
				vertices[i].x = vertices[i - 1].x;
				vertices[i].y = vertices[i - 1].y;
				vertices[i].z = 0;
				++i;
			}
		}
	}
	/* Sorting for easier searching later */
	qsort(vertices, i, sizeof(VERTEX), CompareArenaVertices);
	for (; i < limit; ++i)
		vertices[i].z = vertices[i].y = vertices[i].x = -1;
	fwrite(vertices, sizeof(VERTEX), limit, ofile);

	/* Will need this in OutputArenaSegments */
	return vertices;
}

static void OutputArenaSegments(const char *arena, VERTEX *vertices, int16_t limit, FILE *ofile)
{
	static SEGMENT *S = NULL;
	char focus;
	int16_t i, x, y, xt, yt, count;
	VERTEX v1, v2, v3;

	if (!S)
		S = malloc(sizeof(SEGMENT) * limit);
	count = CountArenaVertices(arena);
	for (y = 0, i = 0; y < ARENA_Y_LIMIT; ++y) {
		for (x = 0; x < ARENA_X_LIMIT; ++x) {
			focus = ArenaCharacterAt(arena, x, y);
			if (strchr(arena_vertices0, focus)) {
				v1.x = v2.x = ARENA_X_OR_Y(x);
				v1.y = v2.y = ARENA_X_OR_Y(y);
				v1.z = 0;
				v2.z = ARENA_HEIGHT(focus);
				S[i].index_from = IndexOfArenaVertex(vertices, &v1, count);
				S[i].index_to = IndexOfArenaVertex(vertices, &v2, count);
				++i;
				for (xt = x + 1; ArenaCharacterAt(arena, xt, y) == '-'; ++xt) ;
				if (ArenaCharacterAt(arena, xt, y) == focus) {
					v3.x = ARENA_X_OR_Y(xt);
					v3.y = v2.y;
					v3.z = v2.z;
					S[i].index_from = IndexOfArenaVertex(vertices, &v2, count);
					S[i].index_to = IndexOfArenaVertex(vertices, &v3, count);
					++i;
					v3.z = 0;
					S[i].index_from = IndexOfArenaVertex(vertices, &v1, count);
					S[i].index_to = IndexOfArenaVertex(vertices, &v3, count);
					++i;
				}
				for (yt = y + 1; ArenaCharacterAt(arena, x, yt) == '|'; ++yt) ;
				if (ArenaCharacterAt(arena, x, yt) == focus) {
					v3.x = v2.x;
					v3.y = ARENA_X_OR_Y(yt);
					v3.z = v2.z;
					S[i].index_from = IndexOfArenaVertex(vertices, &v2, count);
					S[i].index_to = IndexOfArenaVertex(vertices, &v3, count);
					++i;
					v3.z = 0;
					S[i].index_from = IndexOfArenaVertex(vertices, &v1, count);
					S[i].index_to = IndexOfArenaVertex(vertices, &v3, count);
					++i;
				}
			}
		}
	}
	for (; i < limit; ++i)
		S[i].index_from = S[i].index_to = -1;
	fwrite(S, sizeof(SEGMENT), limit, ofile);
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

	for (y = 0; y < ARENA_Y_LIMIT; ++y) {
		for (x = 0; x < ARENA_X_LIMIT; ++x) {
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

	if (y >= 0 && y < ARENA_Y_LIMIT && x >= 0 && x < ARENA_X_LIMIT)
		result = arena[y * ARENA_X_LIMIT + x];

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
	static char *ordinals[] = {
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
		fputs(ordinals[t], stdout);
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
	static OFFSET *vertices = NULL;
	char focus;
	int16_t i, x, y;

	if (!vertices)
		vertices = malloc(sizeof(OFFSET) * limit);
	for (y = 0, i = 0; y < VEH_Y_LIMIT; ++y) {
		for (x = 0; x < VEH_X_LIMIT; ++x) {
			focus = VehicleCharacterAt(vehicle, x, y);
			if (strchr(vehicle_vertices0, focus)) {
				vertices[i].x = x;
				vertices[i].y = y;
				++i;
				if (i == limit) {
					fputs("\nIncrease MAX_VEH_VERTICES!\n", stderr);
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	/* Sorting for easier searching later */
	qsort(vertices, i, sizeof(OFFSET), CompareVehicleVertices);
	for (; i < limit; ++i)
		vertices[i].y = vertices[i].x = -1;

	/* Will need this in lots of places! */
	return vertices;
}

static void OutputVehicleSegments(const char *vehicle, OFFSET *vertices, int16_t limit, FILE *ofile)
{
	static SEGMENT *S = NULL;
	int16_t i, j1, j2, count;

	if (!S)
		S = malloc(sizeof(SEGMENT) * limit);
	count = CountVehicleVertices(vehicle);
	for (i = 0, j1 = 0; j1 < count; ++j1) {
		for (j2 = j1 + 1; j2 < count; ++j2) {
			if (VehicleVerticesAreConnected(vehicle, vertices + j1, vertices + j2)) {
				S[i].index_from = j1;
				S[i].index_to = j2;
				++i;
				if (i == limit) {
					fputs("\nIncrease MAX_VEH_SEGMENTS!\n", stderr);
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	for (; i < limit; ++i)
		S[i].index_from = S[i].index_to = -1;
	fwrite(S, sizeof(SEGMENT), limit, ofile);
}

static void OutputVehicleVertices(const char *vehicle, OFFSET *vertices, int16_t limit, FILE *ofile)
{
	static double *sines, *cosines;
	int8_t tmp;
	int16_t t, u, height, count;
	double d, i, scale, x, y;

	if (!sines || !cosines) {
		sines = malloc(sizeof(double) * VEHICLE_DIRS);
		cosines = malloc(sizeof(double) * VEHICLE_DIRS);
		d = 0.0;
		i = 2.0 * DCPI / VEHICLE_DIRS;
		for (t = 0; t < VEHICLE_DIRS; ++t, d += i) {
			sines[t] = sin(d);
			cosines[t] = cos(d);
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
				tmp = (cosines[t] * x - sines[t] * y) / scale;
				fwrite(&tmp, sizeof(int8_t), 1, ofile);
				tmp = (sines[t] * x + cosines[t] * y) / scale;
				fwrite(&tmp, sizeof(int8_t), 1, ofile);
			}
			for (tmp = -1; u < limit; ++u) {
				fwrite(&tmp, sizeof(int8_t), 1, ofile);
				fwrite(&tmp, sizeof(int8_t), 1, ofile);
			}
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
			if (VehicleCharacterAt(vehicle, (int16_t)(x2 + x), (int16_t)(y2 + y)) == '+')
				++count;
	}
	if (count < 2)
		return false;

	/* Between v1 and midpoint */
	x1 = (v1->x + x2) / 2.0;
	y1 = (v1->y + y2) / 2.0;
	for (count = 0, y = -1; y <= 1; ++y) {
		for (x = -1; x <= 1; ++x)
			if (VehicleCharacterAt(vehicle, (int16_t)(x1 + x), (int16_t)(y1 + y)) == '+')
				++count;
	}
	if (count < 2)
		return false;

	/* Between midpoint and v2 */
	x3 = (x2 + v2->x) / 2.0;
	y3 = (y2 + v2->y) / 2.0;
	for (count = 0, y = -1; y <= 1; ++y) {
		for (x = -1; x <= 1; ++x)
			if (VehicleCharacterAt(vehicle, (int16_t)(x3 + x), (int16_t)(y3 + y)) == '+')
				++count;
	}
	if (count < 2)
		return false;

	return true;
}

static char VehicleCharacterAt(const char *vehicle, int16_t x, int16_t y)
{
	char result = ' ';

	if (y >= 0 && y < VEH_Y_LIMIT && x >= 0 && x < VEH_X_LIMIT)
		result = vehicle[y * VEH_X_LIMIT + x];

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
