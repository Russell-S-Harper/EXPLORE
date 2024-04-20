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

	FOERR: data file open error
	FCERR: file content error
	XMERR: extended memory allocation error
	GIERR: graphics initialization error
	OBERR: accessing array out-of-bounds error
	DCERR: display configuration has been corrupted
	NOERR: no error, used also to provide a count of errors
*/
enum {FOERR, FCERR, XMERR, GIERR, OBERR, DCERR, NOERR};

/* Program data sections

	IDCODE:	initializing data
	EMCODE:	error messages
	FDCODE: field data
	PDCODE: polygon data
	PPCODE: polygon points
	VDCODE: vehicle data
	VPCODE: vehicle points
	HZCODE: horizon data
	SVCODE: scanning data
	ATCODE: arctan data
	TDCODE: sin data
	HDCODE: tan data
	EFCODE: end-of-file
*/
enum {IDCODE = 1, EMCODE, FDCODE, PDCODE,
	PPCODE, VDCODE, VPCODE, HZCODE,
	SVCODE,	ATCODE, TDCODE, HDCODE,
	EFCODE};

/* Colors */
enum {BLACK, WHITE, RED, CYAN,
	MAGENTA, GREEN, BLUE, YELLOW,
	ORANGE, BROWN, LIGHTRED, DARKGRAY,
	MEDIUMGRAY, LIGHTGREEN, LIGHTBLUE, LIGHTGRAY,
	COLORS};

/* To use for debugging */
void OutputAsNumber(char prefix, int16_t value);

/* Defined in initialize.c, ExitProgram is called throughout the program as well as by main */
void ExitProgram(int16_t status);

#endif /* _COMMON_H */
