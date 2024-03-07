
/* savefil.c */

#include <stdlib.h>
#include <stdio.h>
#include "define.h"

#define FLDREC	6
#define PNTREC	26
#define SNDEOR	0x7f00
#define FLLR	0
#define STEMP	50
#define UC_		unsigned char	/* shorthand */
#define PCKSIZ	2
#define SEQSIZ	(3*PCKSIZ)

/*	Available colors:
	BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHTGRAY,
	DARKGRAY, LIGHTBLUE, LIGHTGREEN, LIGHTCYAN,
	LIGHTRED, LIGHTMAGENTA, YELLOW, WHITE
*/

#define COLSH3	LIGHTRED	/* Probe color 3 */
#define COLSH2	LIGHTBLUE	/* Probe color 2 */
#define COLSH1	LIGHTGRAY	/* Probe color 1 */
#define COLSH0	DARKGRAY	/* Probe color 0 */

#define COLFL3	MAGENTA		/* Field color 3 */
#define COLFL2	YELLOW		/* Field color 2 */
#define COLFL1	GREEN		/* Field color 1 */
#define COLFL0	BROWN		/* Field color 0 */

typedef struct {
	work *fieldc, (*field)[FLDLEN], (*addveh)[MACTVEH],
		(*remveh)[MACTVEH];
	work *polydata, pdsize;
	work (*polypnts)[PNTREC];
	work *vehicdata, vdsize;
	work (*vehicpnts)[XYZ*MOBJPNT+2], *vehpoly, (*vehfield)[3*VLYR];
	work *snddat;
	work *horizon, hzsize;
} PRGDAT;

work fieldc0[]= {
	/* (id < MAXFLD = 20) typ hrz fx fy fz */
		 0,0,0,1792,3840,192,
		 1,0,0,1792,3584,192,
		PREOR};
	/* (id < MAXFLD = 20) typ hrz fx fy fz */

work field0[][FLDLEN]= {
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,242,0,0,243,0,0,0,0,0,0,
	0,0,0,0,0,0,0,246,247,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,248,249,0,0,0,0,0,0,0,
	0,0,0,0,0,0,244,0,0,245,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

work addveh0[][MACTVEH]= {
		{0+MACTVEH,1+MACTVEH,2+MACTVEH,
		MVEHFIG-7,MVEHFIG-6,MVEHFIG-5,MVEHFIG-1,PREOR},
		{PREOR}},

	remveh0[][MACTVEH]= {
		{MVEHFIG-7,MVEHFIG-6,MVEHFIG-5,MVEHFIG-1,PREOR},
		{PREOR}};

#define SHPL	(TYPL - 10)
#define SHPM	130
#define LNGR	0
#define RAD	34
#define PLT	42
#define SHBL	(TYPL+SHPL+PLT/14)
#define DISL	(SHBL+RAD*7/3)

work polydata0[] = {
	/* id btm top typ col def spc lml lmh dat */
	/* (id < MPLYFIG) (def < MPLYFIG) MPLYFIG = 250 */
	  0,TYPL,TYPH,IGNORE,FLLR,FLLR,NOPSPC,FLLR,FLLR,FLLR,
	  1,FLLR,FLLR,VEHCLE,FLLR,FLLR,NOPSPC,FLLR,FLLR,FLLR,
	  2,TYPL+256,TYPH,FULWAL,COLFL3,2,NOPSPC,FLLR,FLLR,FLLR,
	242,TYPL,TYPH,SPEC01,COLSH0,242,NOPSPC,FLLR,FLLR,FLLR,
	243,TYPL,TYPH,SPEC01,COLSH0,243,NOPSPC,FLLR,FLLR,FLLR,
	244,TYPL,TYPH,SPEC01,COLSH0,244,NOPSPC,FLLR,FLLR,FLLR,
	245,TYPL,TYPH,SPEC01,COLSH0,245,NOPSPC,FLLR,FLLR,FLLR,
	246,SHBL,TYPH,SPEC00,COLSH1,246,NOPSPC,FLLR,FLLR,FLLR,
	247,SHBL,TYPH,SPEC00,COLSH1,247,NOPSPC,FLLR,FLLR,FLLR,
	248,SHBL,TYPH,SPEC00,COLSH1,248,NOPSPC,FLLR,FLLR,FLLR,
	249,SHBL,TYPH,SPEC00,COLSH1,249,NOPSPC,FLLR,FLLR,FLLR};
	/* id btm top typ col def spc lml lmh dat */
	/* (id < MPLYFIG) (def < MPLYFIG) MPLYFIG = 250 */
	/* polytyp[0] = IGNORE, polytyp[1] = VEHCASE */

work polypnts0[][PNTREC]= {
	/* (id < MPLYFIG = 250) no. of points
	rest are actual points (with room for 8) */

	{  2,4, -88,0,0,		88,0,0,			66,0,320,
			-66,0,320},

	{242,6,	64,-80,LNGR,	80,-64,LNGR,	128,-96,LNGR,
			96,-128,LNGR,	128,-192,SHPL,	192,-128,SHPL},
	{243,6,	-64,-80,LNGR,	-80,-64,LNGR,	-128,-96,LNGR,
			-96,-128,LNGR,	-128,-192,SHPL,	-192,-128,SHPL},
	{244,6,	64,80,LNGR,		80,64,LNGR,		128,96,LNGR,
			96,128,LNGR,	128,192,SHPL,	192,128,SHPL},
	{245,6,	-64,80,LNGR,	-80,64,LNGR,	-128,96,LNGR,
			-96,128,LNGR,	-128,192,SHPL,	-192,128,SHPL},
	{246,7,	-128,64,SHPL,	-64,128,SHPL,	53,53,SHPL,
			-53,-53,SHPL,	-53,-128,SHPL,	128,-128,SHPL,
			128,53,SHPL},
	{247,7,	128,64,SHPL,	64,128,SHPL,	-53,53,SHPL,
			53,-53,SHPL,	53,-128,SHPL,	-128,-128,SHPL,
			-128,53,SHPL},
	{248,7,	-128,-64,SHPL,	-64,-128,SHPL,	53,-53,SHPL,
			-53,53,SHPL,	-53,128,SHPL,	128,128,SHPL,
			128,-53,SHPL},
	{249,7,	128,-64,SHPL,	64,-128,SHPL,	-53,-53,SHPL,
			53,53,SHPL,		53,128,SHPL,	-128,128,SHPL,
			-128,-53,SHPL},
	/* (id < MPLYFIG = 250) no. of points
	rest are actual points (with room for 8) */
	{PREOR,0}};

work vehicdata0[]= {
	/* (id < MVEHFIG = 50+MACTVEH) fx fy fz fd def proc sel dat
	also id cnt inc dat cut opn cls 001 000	for dummy vehicles */

	MACTVEH,1984,2624,SHBL+1,448,1,PLYRIN,DOSEL,FLLR,
	MACTVEH+1,2112,2624,SHBL+1,64,1,PLYRIN,DOSEL,FLLR,
	MACTVEH+2,2112,2496,SHBL+1,192,1,PLYRIN,DOSEL,FLLR,
	MVEHFIG-7,1984,2624,SHPL,448,MVEHTYP-3,NOPROC,NOSEL,FLLR,
	MVEHFIG-6,2112,2624,SHPL,64,MVEHTYP-3,NOPROC,NOSEL,FLLR,
	MVEHFIG-5,2112,2496,SHPL,192,MVEHTYP-3,NOPROC,NOSEL,FLLR,
	MVEHFIG-1,1974,2486,2*RAD+SHPL-TYPL,64,MVEHTYP-1,
		NOPROC,NOSEL,FLLR};

	/* (id < MVEHFIG = 50+MACTVEH) fx fy fz fd def proc sel dat
	also id cnt inc dat cut opn cls 001 000	for dummy vehicles
	first vehicle is the player (no def & uses proc PLYRIN) */

work vehicpnts0[][XYZ*MOBJPNT+2]= {
	/* (id < MVEHTYP = 30) no. of points, point data
	(with room for MOBJPNTs = 40) */

	/* prototype probe */
	{1,4,
	-40,0,-TYPL,40,0,-TYPL,-20,0,128-TYPL,20,0,128-TYPL},
	
	/* probe platform */
	{MVEHTYP-3,14,
	-PLT,PLT,0,PLT,PLT,0,PLT,-PLT,0,-PLT,-PLT,0,
	-PLT*13/14,PLT*13/14,PLT/14,PLT*13/14,PLT*13/14,PLT/14,
	PLT*13/14,-PLT*13/14,PLT/14,-PLT*13/14,-PLT*13/14,PLT/14,
	-PLT/7,PLT/7,PLT/14,PLT/7,PLT/7,PLT/14,
	PLT/7,-PLT/7,PLT/14,-PLT/7,-PLT/7,PLT/14,
	-PLT/14,-PLT*9/14,PLT/14,PLT/14,-PLT*9/14,PLT/14},

	/* receiver dish */
	{MVEHTYP-1,37,
	-RAD*53/128,RAD,0+TYPL,RAD*53/128,RAD,0+TYPL,
	RAD,RAD*53/128,0+TYPL,RAD,-RAD*53/128,0+TYPL,
	RAD*53/128,-RAD,0+TYPL,-RAD*53/128,-RAD,0+TYPL,
	-RAD,-RAD*53/128,0+TYPL,-RAD,RAD*53/128,0+TYPL,
	-RAD*53/192,RAD*2/3,-RAD*11/32+TYPL,
	RAD*53/192,RAD*2/3,-RAD*11/32+TYPL,
	RAD*2/3,RAD*53/192,-RAD*11/32+TYPL,
	RAD*2/3,-RAD*53/192,-RAD*11/32+TYPL,
	RAD*53/192,-RAD*2/3,-RAD*11/32+TYPL,
	-RAD*53/192,-RAD*2/3,-RAD*11/32+TYPL,
	-RAD*2/3,-RAD*53/192,-RAD*11/32+TYPL,
	-RAD*2/3,RAD*53/192,-RAD*11/32+TYPL,
	-RAD*53/384,RAD/3,-RAD/2+TYPL,RAD*53/384,RAD/3,-RAD/2+TYPL,
	RAD/3,RAD*53/384,-RAD/2+TYPL,RAD/3,-RAD*53/384,-RAD/2+TYPL,
	RAD*53/384,-RAD/3,-RAD/2+TYPL,-RAD*53/384,-RAD/3,-RAD/2+TYPL,
	-RAD/3,-RAD*53/384,-RAD/2+TYPL,-RAD/3,RAD*53/384,-RAD/2+TYPL,
	-RAD/8,RAD/8,-RAD+TYPL,RAD/8,RAD/8,-RAD+TYPL,
	RAD/8,-RAD/8,-RAD+TYPL,-RAD/8,-RAD/8,-RAD+TYPL,
	-RAD/12,RAD/12,RAD*3/16+TYPL,RAD/12,RAD/12,RAD*3/16+TYPL,
	RAD/12,-RAD/12,RAD*3/16+TYPL,-RAD/12,-RAD/12,RAD*3/16+TYPL,
	0,0,RAD*53/192+TYPL,
	-RAD*2/3,RAD*2/3,-RAD*2+TYPL,RAD*2/3,RAD*2/3,-RAD*2+TYPL,
	RAD*2/3,-RAD*2/3,-RAD*2+TYPL,-RAD*2/3,-RAD*2/3,-RAD*2+TYPL},

	/* end */
	{PREOR,0}};
	/* (id < MVEHTYP = 30) no. of points, point data
	(with room for MOBJPNTs = 40) */

work vehpoly0[]= {

	/* vehic type 1 */		/* an 'X' shape */
	PLY4,COLSH2,0,1,2,3,
	PREOR,

	/* vehic type MVEHTYP-3 */
	PLY4,COLSH2,4,5,6,7,
	DRW1,COLSH2,0,4,
	DRW1,COLSH2,1,5,
	DRW1,COLSH2,2,6,
	DRW1,COLSH2,3,7,
	DRW1,COLSH2,0,1,
	DRW1,COLSH2,1,2,
	DRW1,COLSH2,2,3,
	DRW1,COLSH2,3,0,
	DRW4,COLSH0,8,9,10,11,
	DRW1,COLSH3,12,13,
	PREOR,

	/* vehic type MVEHTYP-1 */
	DRW4,COLSH0,0,1,9,8,	/* top rung */
	DRW1,COLSH0,1,2,
	DRW4,COLSH0,2,3,11,10,
	DRW1,COLSH0,3,4,
	DRW4,COLSH0,4,5,13,12,
	DRW1,COLSH0,5,6,
	DRW4,COLSH0,6,7,15,14,
	DRW1,COLSH0,7,0,

	DRW1,COLSH0,17,16,		/* bottom rung */
	DRW4,COLSH0,9,10,18,17,
	DRW1,COLSH0,19,18,
	DRW4,COLSH0,11,12,20,19,
	DRW1,COLSH0,21,20,
	DRW4,COLSH0,13,14,22,21,
	DRW1,COLSH0,23,22,
	DRW4,COLSH0,15,8,16,23,

	PLY4,COLSH1,24,25,29,28,	/* center post */
	PLY4,COLSH1,25,26,30,29,
	PLY4,COLSH1,26,27,31,30,
	PLY4,COLSH1,27,24,28,31,
	DRW1,COLSH3,28,32,
	DRW1,COLSH3,29,32,
	DRW1,COLSH3,30,32,
	DRW1,COLSH3,31,32,

	SPC1,COLSH2,24,25,34,33,	/* base */
	SPC1,COLSH2,25,26,35,34,
	SPC1,COLSH2,26,27,36,35,
	SPC1,COLSH2,27,24,33,36,

	PREOR};

work vehfield0[][3*VLYR]= {

	/* vehic type 1 */
	{-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,

	-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,
	-1,-1,0,-1,-1,
	-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,

	-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1},

	/* vehic type MVEHTYP-3 */
	{-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,
	-1,-1,38,-1,-1,
	-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,

	-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,
	-1,-1,44,-1,-1,
	-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,

	6,-1,22,-1,10,
	-1,-1,-1,-1,-1,
	34,-1,0,-1,26,
	-1,-1,-1,-1,-1,
	18,-1,30,-1,14},

	/* vehic type MVEHTYP-1 */
	{36,-1,0,-1,6,
	-1,104,-1,108,-1,
	30,-1,-1,-1,10,
	-1,116,-1,112,-1,
	26,-1,20,-1,16,

	74,-1,40,-1,44,
	-1,-1,80,-1,-1,
	70,98,-1,86,50,
	-1,-1,92,-1,-1,
	64,-1,60,-1,54,

	-1,-1,-1,-1,-1,
	-1,-1,120,-1,-1,
	-1,138,-1,126,-1,
	-1,-1,132,-1,-1,
	-1,-1,-1,-1,-1}
};

work snddat0[] = {
	/* (id < MAXSND = (60 + NCHAN)) rank dat */

	/* start of sound */
	NCHAN,10,ZERO,
	/* noise (2840 +/- 50)Hz at 52dB for 0.029s */
	NOIZ,11,13,1,FLLR,
	/* noise (1740 +/- 40)Hz at 52dB for 0.014s */
	NOIZ,18,13,0,FLLR,
	/* noise (1740 +/- 40)Hz at 56dB for 0.014s */
	NOIZ,18,14,0,FLLR,
	/* tone 77.98Hz at 56dB for 0.029s */
	TONE,67,6,14,1,FLLR,
	/* tone 78.72Hz at 56dB for 0.014s */
	TONE,52,6,14,0,FLLR,
	/* tone 79.47Hz at 60dB for 0.014s */
	TONE,37,6,15,0,FLLR,
	/* noise (1640 +/- 40)Hz at 60dB for 0.029s */
	NOIZ,19,15,1,FLLR,
	/* noise (2840 +/- 50)Hz at 60dB for 0.057s */
	NOIZ,11,15,3,FLLR,
	/* noise (1490 +/- 40)Hz at 60dB for 0.029s */
	NOIZ,21,15,1,FLLR,
	/* tone 84.29Hz at 60dB for 0.057s */
	TONE,203,5,15,3,FLLR,
	/* noise (1420 +/- 40)Hz at 60dB for 0.014s */
	NOIZ,22,15,0,FLLR,
	/* noise (1360 +/- 40)Hz at 56dB for 0.014s */
	NOIZ,23,14,0,FLLR,
	/* noise (2840 +/- 50)Hz at 56dB for 0.057s */
	NOIZ,11,14,3,FLLR,
	/* noise (1300 +/- 40)Hz at 56dB for 0.014s */
	NOIZ,24,14,0,FLLR,
	/* noise (1300 +/- 40)Hz at 52dB for 0.014s */
	NOIZ,24,13,0,FLLR,
	/* tone 77.26Hz at 52dB for 0.014s */
	TONE,82,6,13,0,FLLR,
	/* tone 74.49Hz at 52dB for 0.014s */
	TONE,142,6,13,0,FLLR,
	/* tone 71.31Hz at 48dB for 0.014s */
	TONE,217,6,12,0,FLLR,
	/* tone 70.11Hz at 48dB for 0.014s */
	TONE,247,6,12,0,FLLR,
	/* noise (1300 +/- 40)Hz at 48dB for 0.014s */
	NOIZ,24,12,0,FLLR,
	/* start loop: noise (1950 +/- 40)Hz at 52dB for 0.029s */
	NOIZ,16,13,1,FLLR,
	/* noise (3130 +/- 60)Hz at 52dB for 0.057s */
	NOIZ,10,13,3,FLLR,
	/* noise (1950 +/- 40)Hz at 52dB for 0.029s */
	NOIZ,16,13,1,FLLR,
	/* tone 73.83Hz at 52dB for 0.057s */
	TONE,157,6,13,3-1,FLLR,
	/* end of loop */
	LOOP,-23,SNDEOR,

	NCHAN+1,11,ZERO,TIME,40,FLLR,TONE,195,0,10,120,FLLR,HALT,SNDEOR,
	NCHAN+2,12,ZERO,TIME,80,FLLR,TONE,163,0,10,120,FLLR,HALT,SNDEOR,
	NCHAN+3,8,ZERO,HALT,SNDEOR,	/* cut sound */
	NCHAN+4+0,8,ZERO,NOIZ,195,10,0,FLLR,IDLE,SNDEOR,
	NCHAN+4+1,8,ZERO,NOIZ,184,10,0,FLLR,IDLE,SNDEOR,
	NCHAN+4+2,8,ZERO,NOIZ,176,10,0,FLLR,IDLE,SNDEOR,
	NCHAN+4+3,8,ZERO,NOIZ,150,10,0,FLLR,IDLE,SNDEOR,
	NCHAN+4+4,8,ZERO,NOIZ,135,10,0,FLLR,IDLE,SNDEOR,
	NCHAN+4+5,8,ZERO,NOIZ,127,10,0,FLLR,IDLE,SNDEOR,
	NCHAN+4+6,8,ZERO,NOIZ,184,10,0,FLLR,IDLE,SNDEOR,
	NCHAN+4+7,8,ZERO,HALT,SNDEOR,
	NCHAN+12+0,8,ZERO,HALT,SNDEOR,
	NCHAN+12+1,8,ZERO,NOIZ,195,8,0,FLLR,IDLE,SNDEOR,
	NCHAN+12+2,8,ZERO,NOIZ,184,8,0,FLLR,IDLE,SNDEOR,
	NCHAN+12+3,8,ZERO,NOIZ,176,8,0,FLLR,IDLE,SNDEOR,
	NCHAN+12+4,8,ZERO,NOIZ,150,8,0,FLLR,IDLE,SNDEOR,
	NCHAN+12+5,8,ZERO,NOIZ,135,8,0,FLLR,IDLE,SNDEOR,
	NCHAN+12+6,8,ZERO,NOIZ,127,8,0,FLLR,IDLE,SNDEOR,
	NCHAN+12+7,8,ZERO,NOIZ,184,8,0,FLLR,IDLE,SNDEOR,
	PREOR};
	/* (id < MAXSND = (60 + NCHAN)) rank dat */

#define H00_01	0
#define H01_60	(FCIRC/60)
#define H01_32	(FCIRC/32)
#define H01_30	(FCIRC/30)
#define H01_28	(FCIRC/28)
#define H01_24	(FCIRC/24)
#define H01_20	(FCIRC/20)
#define H01_17	(FCIRC/17)
#define H01_16	(FCIRC/16)
#define H01_15	(FCIRC/15)
#define H03_40	(FCIRC*3/40)
#define H01_13	(FCIRC/13)
#define H01_12	(FCIRC/12)
#define H01_11	(FCIRC/11)
#define H01_10	(FCIRC/10)
#define H03_20	(FCIRC*3/20)
#define H01_06	(FCIRC/6)
#define H11_60	(FCIRC*11/60)
#define H01_05	(FCIRC/5)
#define H13_60	(FCIRC*13/60)
#define H07_30	(FCIRC*7/30)
#define H01_04	(FCIRC/4)
#define H17_60	(FCIRC*17/60)
#define H02_07	(FCIRC*2/7)
#define H03_10	(FCIRC*3/10)
#define H19_60	(FCIRC*19/60)
#define H01_03	(FCIRC/3)
#define H02_05	(FCIRC*2/5)
#define H03_05	(FCIRC*3/5)
#define H02_03	(FCIRC*2/3)
#define H03_04	(FCIRC*3/4)
#define H37_48	(FCIRC*37/48)
#define H39_48	(FCIRC*39/48)
#define H05_06	(FCIRC*5/6)

work horizon0[] =
	/* (id < MFLDHZ = 5) dat (ends with COLBAK) */
	{0,
	COLFL0,-256,H01_12,-237,H01_10,
	COLFL0,-237,H01_10,-214,H01_11,
	COLFL0,-214,H01_11,-187,H01_16,
	COLFL0,-187,H01_16,-162,H01_12,
	COLFL0,-162,H01_12,-146,H01_24,
	COLFL0,-146,H01_24,-140,H01_30,
	COLFL0,-140,H01_30,-112,H01_30,
	COLFL0,-112,H01_30,-72,H01_15,
	COLFL0,-72,H01_15,-40,H01_15,
	COLFL0,-40,H01_15,-30,H01_17,
	COLFL0,-30,H01_17,-3,H01_20,
	COLFL0,-3,H01_20,10,H00_01,
	COLFL0,10,H00_01,32,H01_30,
	COLFL0,32,H01_30,41,H00_01,
	COLFL0,41,H00_01,61,H01_60,
	COLFL0,61,H01_60,88,H01_28,
	COLFL0,88,H01_28,104,H00_01,
	COLFL0,104,H00_01,140,H01_60,
	COLFL0,140,H01_60,177,H01_30,
	COLFL0,177,H01_30,189,H01_13,
	COLFL0,189,H01_13,213,H01_13,
	COLFL0,213,H01_13,234,H01_15,
	COLFL0,234,H01_15,256,H01_12,
	COLFL1,-146,H01_24,-128,H03_10,
	COLFL1,-128,H03_10,-120,H01_06,
	COLFL1,-120,H01_06,-112,H01_30,
	COLFL1,-40,H01_15,-27,H01_04,
	COLFL1,-27,H01_04,-19,H02_05,
	COLFL1,-19,H02_05,-3,H02_07,
	COLFL1,-3,H02_07,1,H01_03,
	COLFL1,1,H01_03,10,H19_60,
	COLFL1,10,H19_60,46,H01_06,
	COLFL0,61,H01_60,69,H01_12,
	COLFL1,46,H01_06,69,H01_12,
	COLFL1,104,H00_01,114,H03_40,
	COLFL1,114,H03_40,125,H03_20,
	COLFL1,125,H03_20,133,H01_15,
	COLFL1,133,H01_15,149,H19_60,
	COLFL1,149,H19_60,154,H07_30,
	COLFL1,154,H07_30,162,H17_60,
	COLFL1,162,H17_60,177,H01_30,
	COLFL0,69,H01_12,86,H13_60,
	COLFL0,86,H13_60,92,H01_05,
	COLFL0,92,H01_05,102,H01_04,
	COLFL0,102,H01_04,114,H03_40,
	COLFL0,-120,H01_06,-91,H01_03,
	COLFL0,-91,H01_03,-79,H11_60,
	COLFL0,-79,H11_60,-33,H02_05,
	COLFL0,-33,H02_05,-27,H01_04,
	COLFL2,-19,H02_05,-16,H03_05,
	COLFL2,-16,H03_05,5,H02_03,
	COLFL2,5,H02_03,10,H19_60,
	COLFL2,-45,H05_06,-44,H39_48,
	COLFL2,-44,H39_48,-44,H37_48,
	COLFL2,-44,H37_48,-45,H03_04,
	COLFL2,-45,H03_04,-47,H03_04,
	COLFL2,-47,H03_04,-48,H37_48,
	COLFL2,-48,H37_48,-48,H39_48,
	COLFL2,-48,H39_48,-47,H05_06,
	COLFL2,-47,H05_06,-45,H05_06,
	COLBAK};
	/* (id < MFLDHZ = 5) dat (ends with COLBAK) */

unsigned char bufs[MCHAR];

work bufp;

FILE *wrspec;

PRGDAT prgdat0 = {fieldc0, field0, addveh0, remveh0, 
				polydata0, sizeof polydata0, 
				polypnts0, 
				vehicdata0, sizeof vehicdata0, 
				vehicpnts0, vehpoly0, vehfield0, 
				snddat0, 
				horizon0, sizeof horizon0};

void LongFile (PRGDAT *);
void SaveField (work *, work *, work *, work *);
void ConvFile (char *, char *);
void FlushBuf (work, FILE *);
work WriteBuf (work, work, FILE *);
work FindSeq (work, work);

void main ()
{
	unsigned char con;
	static char *tempfil = "savefil.dat", *convfil = "zcndfil.dat";

	wrspec = fopen (tempfil, "wb");
	LongFile (&prgdat0);
	con = (UC_)EFCODE;
	fwrite (&con, sizeof (UC_), 1, wrspec);
	fclose (wrspec);

	ConvFile (tempfil, convfil);
	/* unlink (tempfil); */
}

void LongFile (PRGDAT *prgdat)
{
	unsigned char con;
	work dat, num, cnt, eor = PREOR, arr[STEMP], lst[STEMP];

	if (prgdat-> fieldc) {
		con = (UC_)FDCODE;
		fwrite (&con, sizeof (UC_), 1, wrspec);
		for (num = cnt = 0; prgdat-> fieldc[cnt] != PREOR;
			cnt += FLDREC, num++);
		fwrite ((char *)&num, sizeof (work), 1, wrspec);
		for (cnt = 0; cnt < num; cnt++)
			SaveField (&prgdat-> fieldc[cnt * FLDREC],
				prgdat-> field[cnt], prgdat-> addveh[cnt],
				prgdat-> remveh[cnt]);
	}

	if (prgdat-> polydata) {
		con = (UC_)PDCODE;
		fwrite (&con, sizeof (UC_), 1, wrspec);
		fwrite ((char *)prgdat-> polydata,
			(int)prgdat-> pdsize, 1, wrspec);
		fwrite ((char *)&eor, sizeof (work), 1, wrspec);
	}

	if (prgdat-> polypnts) {
		con = (UC_)PPCODE;
		fwrite (&con, sizeof (UC_), 1, wrspec);
		for (num = cnt = 0; prgdat-> polypnts[num][0] != PREOR; num++)
			cnt += prgdat-> polypnts[num][1];
		fwrite ((char *)&cnt, sizeof (work), 1, wrspec);
		for (cnt = 0; cnt < num; cnt++) {
			dat = prgdat-> polypnts[cnt][1];
			fwrite ((char *)prgdat-> polypnts[cnt], sizeof (work),
				(int) (2 + XYZ * dat), wrspec);
		}
		fwrite ((char *)&eor, sizeof (work), 1, wrspec);
	}

	if (prgdat-> vehicdata) {
		con = (UC_)VDCODE;
		fwrite (&con, sizeof (UC_), 1, wrspec);
		fwrite ((char *)prgdat-> vehicdata,
			(int)prgdat-> vdsize, 1, wrspec);
		fwrite ((char *)&eor, sizeof (work), 1, wrspec);
	}

	if (prgdat-> vehicpnts) {
		con = (UC_)VPCODE;
		fwrite (&con, sizeof (UC_), 1, wrspec);
		for (num = cnt = 0; prgdat-> vehicpnts[num][0] != PREOR; num++)
			cnt += prgdat-> vehicpnts[num][1];
		fwrite ((char *)&num, sizeof (work), 1, wrspec);
		fwrite ((char *)&cnt, sizeof (work), 1, wrspec);
		lst[0] = 0;
		for (cnt = dat = 0; cnt < num; dat++) {
			if (prgdat-> vehpoly[dat] == PREOR) {
				lst[cnt + 1] = dat + 1;
				arr[cnt++] = dat - cnt;
			}
		}
	    /*	dat -= num - 1; */
		cnt--;
		fwrite ((char *)&dat, sizeof (work), 1, wrspec);
		for (; cnt > 0; cnt--) arr[cnt] -= arr[cnt - 1];
		for (cnt = 0; cnt < num; cnt++) {
			dat = prgdat-> vehicpnts[cnt][1];
			fwrite ((char *)prgdat-> vehicpnts[cnt], sizeof (work),
				 (int) (2 + XYZ * dat), wrspec);
			fwrite ((char *)prgdat-> vehfield[cnt], sizeof (work),
				(int) (3 * VLYR), wrspec);
			dat = arr[cnt];
			fwrite ((char *)&dat, sizeof (work), 1, wrspec);
			fwrite ((char *)&prgdat-> vehpoly[lst[cnt]], sizeof (work),
				(int)dat, wrspec);
		}
	}

	if (prgdat-> snddat) {
		con = (UC_)SDCODE;
		fwrite (&con, sizeof (UC_), 1, wrspec);
		for (cnt = dat = 0; prgdat-> snddat[cnt] != PREOR;) {
			for (num = 0; prgdat-> snddat[cnt + num] != SNDEOR; num++);
			lst[dat] = cnt;
			arr[dat++] = num - 2;
			cnt += ++num;
		}
		for (num = cnt = 0; cnt < dat; cnt++) num += arr[cnt];
		fwrite ((char *)&num, sizeof (work), 1, wrspec);
		for (cnt = 0; cnt < dat; cnt++) {
			fwrite ((char *)&prgdat-> snddat[lst[cnt]], sizeof (work),
				2, wrspec);
			fwrite ((char *)&arr[cnt], sizeof (work), 1, wrspec);
			for (num = 0; num < arr[cnt]; num++) {
				con = (UC_) (prgdat-> snddat[lst[cnt] + num + 2]);
				fwrite (&con, sizeof (UC_), 1, wrspec);
			}
		}
		fwrite ((char *)&eor, sizeof (work), 1, wrspec);
	}

	if (prgdat-> horizon) {
		con = (UC_)HZCODE;
		fwrite (&con, sizeof (UC_), 1, wrspec);
		fwrite ((char *)&prgdat-> hzsize, sizeof (work), 1, wrspec);
		fwrite ((char *)prgdat-> horizon,
			(int)prgdat-> hzsize, 1, wrspec);
	}
}

void SaveField (work *fieldc, work *field, work *addveh, work *remveh)
{
	work t;

	fwrite ((char *)fieldc, sizeof (work), (int)FLDREC, wrspec);
	for (t = 0, field += (FLDLEN-FLDXDM);
		t < FLDYDM; t++, field -= FLDXDM)
		fwrite ((char *)field, sizeof (work), (int)FLDXDM, wrspec);

	fwrite ((char *)addveh, sizeof (work), (int)MACTVEH, wrspec);
	fwrite ((char *)remveh, sizeof (work), (int)MACTVEH, wrspec);
}

void ConvFile (char *ifile, char *ofile)
{
	unsigned char a, b;
	work t, cnt;
	FILE *idat, *odat;

	idat = fopen (ifile, "rb");
	odat = fopen (ofile, "wb");

	bufp = 0;

	while (a = fgetc (idat), !feof (idat)) {

		bufs[bufp++] = a;

		if (bufp >= LCHAR) {
			FlushBuf ((work)-2, odat);
			bufs[bufp++] = a;
		}

		cnt = 1;

		while (!feof (idat) && (b = fgetc (idat)) == a) cnt++;

		if (!a && cnt > 1 && cnt % 2) {
			bufs[bufp++] = a;
			cnt--;
		}

		if (cnt > 2) {
			FlushBuf ((work)-2, odat);
			if (t = cnt % LCHAR) {
				fputc ((char)t, odat);
				fputc (a, odat);
			}
			for (t = cnt/LCHAR; t > 0; t--) {
				fputc ((char)LCHAR, odat);
				fputc (a, odat);
			}
		}
		else {
			while (--cnt) {
				bufs[bufp++] = a;
				if (bufp >= LCHAR) FlushBuf ((work)-1, odat);
			}
		}

		ungetc (b, idat);
	}

	FlushBuf ((work)-1, odat);

	fclose (idat);
	fclose (odat);
}

void FlushBuf (work n, FILE *odat)
{
	work q, r, s, t = 0;

	bufp += n;

	while (t <= bufp) {

		if (bufp - t < SEQSIZ - 1) t = WriteBuf (t, bufp, odat);

		else {
			if ((s = FindSeq (t, (work)STSEQ)) >= LMSEQ) {
				WriteBuf (t, s - 1, odat);
				q = s;

				while ((r = FindSeq (q+PCKSIZ, (work)LMSEQ)) >= LMSEQ)
					q = r;

				fputc ((char)STSEQ, odat);
				fputc ((char) (q - s + SEQSIZ)/PCKSIZ, odat);

				for (q += (SEQSIZ - PCKSIZ); s <= q; s += PCKSIZ)
					fputc (bufs[s], odat);	/* change */

				t = q + 1;
			}

			else t = WriteBuf (t, bufp, odat);
		}

		t++;
	}

	bufp = 0;
}

work WriteBuf (work pos0, work pos1, FILE *odat)
{

	if (pos1 - pos0 == 1 && !bufs[pos1]) {
		fputc ((char)SPSEQ, odat);
		fputc (bufs[pos0], odat);
	}

	else if (pos1 >= pos0) {
		fputc ((char) (pos1 - pos0 + 1 + MCHAR), odat);
		for (; pos0 <= pos1; pos0++) fputc (bufs[pos0], odat);
	}

	return pos1;
}

work FindSeq (work index, work flag)
{
	work cnt;

	if (bufp - index < SEQSIZ - 1) return NOSEQ;

	if (flag == LMSEQ) {	/* change */

		if (!bufs[index + 1] && !bufs[index + PCKSIZ + 1] &&
			!bufs[index + 2*PCKSIZ + 1]) return index;

		else return NOSEQ;
	}
	else {

		for (cnt = bufp - (SEQSIZ - 1); index <= cnt; index++)
			if (!bufs[index + 1] && !bufs[index + PCKSIZ + 1] &&
				!bufs[index + 2*PCKSIZ + 1]) return index;

		return NOSEQ;
	}
}
