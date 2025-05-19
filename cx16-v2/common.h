/*
    Contents: "common" C header file (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#ifndef _COMMON_H
#define _COMMON_H

/* Constants */
#define SCALE_FC	512	/* 512 corresponds to a Full Circle or 360 degrees */
#define SHIFT_FC	9	/* log2(SCALE_FC) */

#define SCALE_1_0	4096	/* Equivalent to 1.0 in some contexts */
#define SHIFT_1_0	12	/* log2(SCALE_1_0) */

/* Arenas are confined to cubes of size 16384 = ARENA_(X|Y|Z)_LMT * 1024 */
#define ARENA_X_LMT	16
#define ARENA_Y_LMT	16
#define ARENA_Z_LMT	16

/* Vehicle resolutions when rendering */
#define VEHICLE_HGTS	8
#define VEHICLE_DIRS	32

/* Common structures */
typedef struct {int16_t x, y, z;} VERTEX;
typedef struct {int8_t index_from, index_to;} SEGMENT;
typedef struct {int8_t x, y;} OFFSET;

/* Errors

	ERR_FO: data file open error
	ERR_FC: file content error
	ERR_XM: extended memory allocation error
	ERR_GI: graphics initialization error
	ERR_OB: accessing array out-of-bounds error
	ERR_DC: display configuration has been corrupted
	ERR_AI: AI file access read or write error
	ERR_NO: no error, also used to provide a count of errors
*/
enum {ERR_FO, ERR_FC, ERR_XM, ERR_GI, ERR_OB, ERR_DC, ERR_AI, ERR_NO};

/* Program data sections

	CODE_ID: initializing data
	CODE_EM: error messages
	CODE_TD: trig data
	CODE_AD: arena data
	CODE_VD: vehicle data
	CODE_SD: sound data
	CODE_LD: level data
	CODE_EF: end-of-file
*/
enum {CODE_ID, CODE_EM, CODE_AM, CODE_TD, CODE_AD, CODE_VD, CODE_SD, CODE_LD, CODE_EF};

/* Colors */
enum {CLR16_BLACK, CLR16_WHITE, CLR16_RED, CLR16_CYAN,
	CLR16_MAGENTA, CLR16_GREEN, CLR16_BLUE, CLR16_YELLOW,
	CLR16_ORANGE, CLR16_BROWN, CLR16_LIGHTRED, CLR16_DARKGRAY,
	CLR16_MEDIUMGRAY, CLR16_LIGHTGREEN, CLR16_LIGHTBLUE, CLR16_LIGHTGRAY,
	CLR16_COUNT};

/* Sounds */
enum {MSS_FIRING, MSS_EXPLODING, BELL_RINGING, SOUNDS_CNT};

/* Defined in initialize.c, to use for debugging */
void OutputAsNumber(char prefix, int16_t value);

/* Defined in initialize.c, ExitProgram is called throughout the program as well as by main */
void ExitProgram(uint8_t status);

#endif /* _COMMON_H */
