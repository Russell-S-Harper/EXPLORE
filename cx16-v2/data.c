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

#define DCPI	3.141592653589793

#define X_OR_Y(i)	(int16_t)(((i) + 0.5) * 1024.0)
#define HEIGHT(c)	(int16_t)(16384.0 * (0.075 * (c) - 2.225))

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
const char *vertices0 = "!#$%&*+";

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
	"|  +        +  |"
	"|  |        |  |"
	"|  |        |  |"
	"|  +--------+  |"
	"|              |"
	"|              |"
	"|  +--------+  |"
	"|  |        |  |"
	"|  |        |  |"
	"|  +        +  |"
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

static void OutputStrings(FILE *ofile);
static void OutputTrigData(FILE *ofile);
static void OutputArenaData(FILE *ofile);

static void OutputArena16x16(const char *arena, FILE *ofile);
static VERTEX *OutputArenaVertices(const char *arena, int16_t limit, FILE *ofile);
static void OutputArenaSegments(const char *arena, VERTEX *vertices, int16_t limit, FILE *ofile);

static int16_t CountVertices(const char *arena);
static int16_t CountSegments(const char *arena);
static char CharacterAt(const char *arena, int16_t x, int16_t y);
static int16_t IndexOfVertex(VERTEX *vertices, VERTEX *vertex, int16_t count);
static int CompareVertices(const void *p, const void *q);

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
		tmp = CountVertices(arenas0[t]);
		if (vertices < tmp) vertices = tmp;
		tmp = CountSegments(arenas0[t]);
		if (segments < tmp) segments = tmp;
	}
	/* Size of an arena data structure */
	tmp = X_LIMIT * Y_LIMIT * sizeof(int16_t) + (vertices + 1) * sizeof(VERTEX) + (segments + 1) * sizeof(SEGMENT);
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
		working = malloc(sizeof(int16_t) * X_LIMIT * Y_LIMIT);
	for (y = 0; y < Y_LIMIT; ++y) {
		for (x = 0; x < X_LIMIT; ++x) {
			focus = CharacterAt(arena, x, y);
			if (strchr(vertices0, focus)) {
				height = HEIGHT(focus);
				working[y * X_LIMIT + x] = height;
				for (xt = x + 1; CharacterAt(arena, xt, y) == '-'; ++xt)
					working[y * X_LIMIT + xt] = height;
				for (yt = y + 1; CharacterAt(arena, x, yt) == '|'; ++yt)
					working[yt * X_LIMIT + x] = height;
			} else if (focus == ' ')
				working[y * X_LIMIT + x] = 0;
		}
	}
	fwrite(working, sizeof(int16_t), X_LIMIT * Y_LIMIT, ofile);
}

static VERTEX *OutputArenaVertices(const char *arena, int16_t limit, FILE *ofile)
{
	static VERTEX *vertices = NULL;
	char focus;
	int16_t i, x, y;

	if (!vertices)
		vertices = malloc(sizeof(VERTEX) * limit);
	for (y = 0, i = 0; y < Y_LIMIT; ++y) {
		for (x = 0; x < X_LIMIT; ++x) {
			focus = CharacterAt(arena, x, y);
			if (strchr(vertices0, focus)) {
				vertices[i].x = X_OR_Y(x);
				vertices[i].y = X_OR_Y(y);
				vertices[i].z = HEIGHT(focus);
				++i;
				vertices[i].x = vertices[i - 1].x;
				vertices[i].y = vertices[i - 1].y;
				vertices[i].z = 0;
				++i;
			}
		}
	}
	/* Sorting for easier searching later */
	qsort(vertices, i, sizeof(VERTEX), CompareVertices);
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
	count = CountVertices(arena);
	for (y = 0, i = 0; y < Y_LIMIT; ++y) {
		for (x = 0; x < X_LIMIT; ++x) {
			focus = CharacterAt(arena, x, y);
			if (strchr(vertices0, focus)) {
				v1.x = v2.x = X_OR_Y(x);
				v1.y = v2.y = X_OR_Y(y);
				v1.z = 0;
				v2.z = HEIGHT(focus);
				S[i].index_from = IndexOfVertex(vertices, &v1, count);
				S[i].index_to = IndexOfVertex(vertices, &v2, count);
				++i;
				for (xt = x + 1; CharacterAt(arena, xt, y) == '-'; ++xt) ;
				if (CharacterAt(arena, xt, y) == focus) {
					v3.x = X_OR_Y(xt);
					v3.y = v2.y;
					v3.z = v2.z;
					S[i].index_from = IndexOfVertex(vertices, &v2, count);
					S[i].index_to = IndexOfVertex(vertices, &v3, count);
					++i;
					v3.z = 0;
					S[i].index_from = IndexOfVertex(vertices, &v1, count);
					S[i].index_to = IndexOfVertex(vertices, &v3, count);
					++i;
				}
				for (yt = y + 1; CharacterAt(arena, x, yt) == '|'; ++yt) ;
				if (CharacterAt(arena, x, yt) == focus) {
					v3.x = v2.x;
					v3.y = X_OR_Y(yt);
					v3.z = v2.z;
					S[i].index_from = IndexOfVertex(vertices, &v2, count);
					S[i].index_to = IndexOfVertex(vertices, &v3, count);
					++i;
					v3.z = 0;
					S[i].index_from = IndexOfVertex(vertices, &v1, count);
					S[i].index_to = IndexOfVertex(vertices, &v3, count);
					++i;
				}
			}
		}
	}
	for (; i < limit; ++i)
		S[i].index_from = S[i].index_to = -1;
	fwrite(S, sizeof(SEGMENT), limit, ofile);
}


static int16_t CountVertices(const char *arena)
{
	int16_t count = 0;

	while (*arena) {
		if (strchr(vertices0, *arena))
			count += 2;
		++arena;
	}

	return count;
}

static int CountSegments(const char *arena)
{
	char focus, tmp;
	int16_t x, y, count = 0;

	for (y = 0; y < Y_LIMIT; ++y) {
		for (x = 0; x < X_LIMIT; ++x) {
			focus = CharacterAt(arena, x, y);
			if (strchr(vertices0, focus)) {
				count += 1;
				tmp = CharacterAt(arena, x + 1, y);
				if (tmp == '-' || tmp == focus)
					count += 2;
				tmp = CharacterAt(arena, x, y + 1);
				if (tmp == '|' || tmp == focus)
					count += 2;
			}
		}
	}

	return count;
}

static char CharacterAt(const char *arena, int16_t x, int16_t y)
{
	char result = ' ';

	if (y >= 0 && y < Y_LIMIT && x >= 0 && x < X_LIMIT)
		result = arena[y * X_LIMIT + x];

	return result;
}

static int16_t IndexOfVertex(VERTEX *vertices, VERTEX *vertex, int16_t count)
{
	VERTEX *v = bsearch(vertex, vertices, count, sizeof(VERTEX), CompareVertices);

	return v? (int16_t)(v - vertices): -1;
}

static int CompareVertices(const void *p, const void *q)
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
