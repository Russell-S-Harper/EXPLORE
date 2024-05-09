/*
    Contents: "common" C header file (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#ifndef _COMMON_H
#define _COMMON_H

/* Constants */
#define FULL_CIRC	512		/* 512 corresponds to 360 degrees */
#define HALF_CIRC	(FULL_CIRC / 2)	/* Corresponds to 180 degrees */
#define VEH_DIR		64		/* Vehicles can move in VEH_DIR directions */

#define SCALE_1_0	4096		/* Equivalent to 1.0 in some contexts */
#define SCALE_0_5	(SCALE_1_0 / 2)	/* Equivalent to 0.5 in some contexts */
#define SHIFT_1_0	12		/* log2(SCALE_1_0) */

/* Errors

	ERR_FO: data file open error
	ERR_FC: file content error
	ERR_XM: extended memory allocation error
	ERR_GI: graphics initialization error
	ERR_OB: accessing array out-of-bounds error
	ERR_DC: display configuration has been corrupted
	ERR_NO: no error, used also to provide a count of errors
*/
enum {ERR_FO, ERR_FC, ERR_XM, ERR_GI, ERR_OB, ERR_DC, ERR_NO};

/* Program data sections

	CODE_ID: initializing data
	CODE_EM: error messages
	CODE_FD: field data
	CODE_PD: polygon data
	CODE_PP: polygon points
	CODE_VD: vehicle data
	CODE_VP: vehicle points
	CODE_HZ: horizon data
	CODE_SV: scanning data
	CODE_AT: arctan data
	CODE_TD: sin data
	CODE_HD: tan data
	CODE_EF: end-of-file
*/
enum {CODE_ID, CODE_EM, CODE_FD, CODE_PD,
	CODE_PP, CODE_VD, CODE_VP, CODE_HZ,
	CODE_SV, CODE_AT, CODE_TD, CODE_HD,
	CODE_EF};

/* Colors */
enum {CLR16_BLACK, CLR16_WHITE, CLR16_RED, CLR16_CYAN,
	CLR16_MAGENTA, CLR16_GREEN, CLR16_BLUE, CLR16_YELLOW,
	CLR16_ORANGE, CLR16_BROWN, CLR16_LIGHTRED, CLR16_DARKGRAY,
	CLR16_MEDIUMGRAY, CLR16_LIGHTGREEN, CLR16_LIGHTBLUE, CLR16_LIGHTGRAY,
	CLR16_COUNT};

/* Defined in initialize.c, to use for debugging */
void OutputAsNumber(char prefix, int16_t value);

/* Defined in initialize.c, ExitProgram is called throughout the program as well as by main */
void ExitProgram(int16_t status);

#endif /* _COMMON_H */
