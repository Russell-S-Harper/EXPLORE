
/* define.h */

#ifndef DFN_HDR
#define DFN_HDR

/******** compiler specific defines; adjust as req'd ********/

#define work short	/* 16 bits; any size >= 16 bits is okay */

#define prec long	/* 32 bits; any size >= 32 bits is okay */

/*	<work> is used for most calculations and <prec> is used
	for calculations requiring more precision than <work>.
	#define's are used in this case instead of typedef's because
	<work> and <prec> are often prefixed by <unsigned> and typedef's
	can't handle that kind of construction */

#define PackPrec(a, b, c) (a) = (c); *((work *)&(a) + 1) = (b)
/*	PackPrec packs two 4 digit quantities into a prec;
	(b) and (c) are the high and low order integers respectively
	(eg. after PackPrec(num, 0x1020, 0x3040) num == 0x10203040) */

#define Prec2nd(a) *((work *)&(a) + 1)
/*	Prec2nd picks out the second lowest group of 4 digits
	from a prec; (eg. Prec2nd(0x40506070) == 0x4050) */


/******** defines; no adjustments req'd ********/

#define DGTSHFT 4
/*	DGTSHFT == # of bits in a hex digit; an integer is often shifted
	left or right a multiple of DGTSHFT times to increase or decrease
	its precision */

#define WRKABS	(work)((unsigned work)~0 >> 1)
/*	x & WRKABS == |x|, also WRKABS is the largest positive work */

#define WRKSHFT \
	(2*DGTSHFT * sizeof(work))	/* # of bits in a work */

#define SIN90	4096	/* Equivalent to sin(90) in some contexts */
#define FCIRC	512	/* 512 = 360 degrees */
#define FCSHFT	9	/* == log2(FCIRC) */
#define VIEWA	FCIRC/4	/* Viewing angle is 90 degrees */

#define LGEAR	9	/* Divide SIN90 by 2^LGEAR to give low gear */
#define HGEAR	6	/* and by 2^HGEAR to give high gear */

#define RECINIT	0

#define XMIN	0
#define YMIN	0

#define NUMCOL	16		/* Number of colors */

#if	!defined(__COLORS)
#define __COLORS

enum	{BLACK, BLUE, GREEN, CYAN,
	RED, MAGENTA, BROWN, LIGHTGRAY,
	DARKGRAY, LIGHTBLUE, LIGHTGREEN, LIGHTCYAN,
	LIGHTRED, LIGHTMAGENTA, YELLOW, WHITE};
#endif

#define COLTYP	WHITE		/* Color of alphanumeric type */
#define COLSPC	LIGHTGREEN	/* Color of selected target */
#define COLNRM	LIGHTGRAY	/* Color of normal targets */
#define COLBAK	BLACK		/* Background color */

#define NCHAN	4		/* Number of sound channels */

#define Sin(a) trigdata[(a) & (FCIRC-1)]
#define Cos(a) trigdata[((a) + FCIRC/4) & (FCIRC-1)]

/*	Field constants */
#define FLDXDM	16	/* Field 'x' dimension; must be a power of 2 */
#define FLDYDM	20	/* Field 'y' dimension */
#define FLDSHFT	4	/* == log2(FLDXDM) see FLDXDM above */
#define MAXFL	34	/* == sqrt(2*sqr(FLDXDM) + 2*sqr(FLDYDM)) - 2 */

#define FPREC	(1 << 2 * DGTSHFT)	/* Extra precision */
#define MAXFX	(FLDXDM * FPREC -1)	/* FLDXDM and FLDYDM in */
#define MAXFY	(FLDYDM * FPREC -1)	/* more precise units */

#define FLDLEN	(FLDXDM * FLDYDM)	/* Field size */

#define NOVEH	-1
#define LMVEH	(NOVEH+1)
#define NOFLD	-1
#define LMFLD	(NOFLD+1)

#define TYPH	4096	/* Outside ceiling */
#define TYPL	96		/* Corresponds to eye level */

#define VEHINIT	0		/* Object constants */
#define VEHDIR	64		/* Vehicle can move VEHDIR ways */
#define VDSHFT	6		/* == log2(VEHDIR) */

#define VLYR	25
#define FLYR	3*VLYR

#define VIDL	-1		/* Vehicle constants */
#define VCLR	0
#define VSET	1

#define NOSEL	-1
#define DESEL	0
#define DOSEL	1
#define NXSEL	128

#define NOACT	-1
#define LMACT	(NOACT+1)

#define MFLDOBJ	180		/* Array limits */
#define MOBJPNT	40
#define MPLYFIG	250
#define MACTVEH	15
#define MVEHTYP	30
#define MVEHFIG	(50+MACTVEH)

#define MFLDHZ	5
#define MAXFLD	20
#define MAXSND	(60+NCHAN)

#define PREOR	-1		/* Program data file defines */
#define LMEOR	(PREOR+1)
#define XYZ		3

#define MCHAR	128
#define LCHAR	(MCHAR-2)

#define NOSEQ	-1
#define LMSEQ	(NOSEQ+1)
#define STSEQ	0x7f
#define SPSEQ	0xff

/*	Program data case defines */
enum {FDCODE = 2, PDCODE, PPCODE, VDCODE,
	VPCODE, SDCODE, HZCODE, EFCODE};

/*	Sound routine case defines */
enum {ZERO, TONE, NOIZ, TNNS, TIME, LOOP,
	REST, NOPR, HALT, IDLE = ZERO - 1};	

/*	RenderObj() case defines */
enum {IGNORE, VEHCLE, FULWAL, TOPBTM, RGTEDG, LFTEDG, PENTGN,
	WALL3E, SNGDRW, TPONLY, POLYS4, DRWMSK, SPEC00, SPEC01};

/*	VehCase() case defines */
enum {PLY3, PLY4, WAL3, WAL4, DRW1, DRW4, SPC0, SPC1};

/*	ProcessVeh() case defines */
enum {NOPROC, PLYRIN, ROTVEH, DOORPD, DOORFD, GTPLYR};

/*	FldTravel() case defines */
enum {NOPSPC, NEWFLD, NEWVEH, INFVEH};

/*	ExitMessage() case defines */
enum {NOERR, MAERR, FOERR, FCERR, GIERR, DTERR};

/*	Array constants; to simplify memory allocation and array
	addressing, arrays frequently used together are combined into
	one large array as subarrays. Typically, a pointer indicates
	the start of a large array and constants are added to the pointer
	to index each subarray. This method was chosen as opposed to
	using structures because often an entire subarray has to be
	accessed and having the subarray in an unbroken block results
	in a more efficient method of accessing the subarray */

#define POLY	0
#define VEHIC	MFLDOBJ
#define SCANX	2*MFLDOBJ
#define SCANY	3*MFLDOBJ
#define SCANRSV	4*MFLDOBJ

#define XCOOR	0
#define YCOOR	MOBJPNT
#define ZCOOR	2*MOBJPNT
#define MAP_X	3*MOBJPNT
#define MAP_Y	4*MOBJPNT
#define COORSEG	5*MOBJPNT
#define COORRSV	4*COORSEG	/* For 1 call plus 3 recursions of Vehicle */

#define PBTM	0
#define PTOP	MPLYFIG
#define PTYP	2*MPLYFIG
#define PCOL	3*MPLYFIG
#define PDEF	4*MPLYFIG
#define PSPC	5*MPLYFIG
#define PLML	6*MPLYFIG
#define PLMH	7*MPLYFIG
#define PDAT	8*MPLYFIG
#define POLYRSV	9*MPLYFIG

#define VEHICPNTS	0
#define VEHFIELD	MVEHTYP
#define VEHPOLY		2*MVEHTYP
#define VTPRSV		3*MVEHTYP

#define VEHLNGTH	0
#define VEHSTART	MVEHTYP
#define VTDRSV		2*MVEHTYP

#define VEHICX	0
#define VEHICY	MVEHFIG
#define VEHICZ	2*MVEHFIG
#define VEHICD	3*MVEHFIG
#define VEHICI	4*MVEHFIG
#define VEHICP	5*MVEHFIG
#define VEHICA	6*MVEHFIG
#define VEHICT	7*MVEHFIG
#define VEHICE	8*MVEHFIG
#define VEHICL	9*MVEHFIG
#define VEHICH	10*MVEHFIG
#define VEHICF	11*MVEHFIG
#define VEHICS	(11*MVEHFIG + MACTVEH)
#define VEHICU	(11*MVEHFIG + 2*MACTVEH)
#define VEHICC	(11*MVEHFIG + 3*MACTVEH)
#define VEHICO	(11*MVEHFIG + 4*MACTVEH)
#define VEHICQ	(11*MVEHFIG + 5*MACTVEH)
#define VEHRSV	(11*MVEHFIG + 6*MACTVEH)

#define VEHCNT	VEHICX	/* Defines used for dummy vehicles */
#define VEHINC	VEHICY
#define DOORDT	VEHICZ
#define CUTSND	VEHICD
#define OPNSND	VEHICI
#define CLSSND	VEHICP
#define VEH001	VEHICT
#define VEH000	VEHICE

#define FLST	0		/* More array constants */
#define AVEH	MAXFLD
#define DVEH	2*MAXFLD
#define FPRSV	3*MAXFLD

#define FLDSX	0
#define FLDSY	MAXFLD
#define FLDSZ	2*MAXFLD
#define RECFX	3*MAXFLD
#define RECFY	4*MAXFLD
#define RECFZ	5*MAXFLD
#define FLDTP	6*MAXFLD
#define HORZN	7*MAXFLD
#define FCRSV	8*MAXFLD

#define FIELDFLG 0
#define FIELDVEH FLDLEN
#define FLDRSV	2*FLDLEN

/******** function declarations ********/

/*	spcrtn.c contains processor, compiler
	and machine specific routines */


/*	Routines called by main */

void InitProgram(void);	/* Defined in prgdat.c */
void ProcessVeh(void);	/* Defined in fldrtn.c */
void ScanField(void);	/* Defined in fldrtn.c */
void RenderObj(void);	/* Defined in objrtn.c */
void UpdateDisplay(void);/* Defined in spcrtn.c */
void ExitProgram(work);	/* Defined in prgdat.c */
/*	ExitProgram is called throughout the program as well as by main */

/*	Routines defined in spcrtn.c and called by InitProgram */

void InitPlot(void);
void InitGraphics(void);
void InitMouse(void);
void InitInterrupt(void);


/*	Math routines defined in spcrtn.c
	and called throughout the program */

work SlopeCalc(work, work, work);
work SpecMult(work, work);
work SpecDiv(work, work);


/*	Keyboard routine defined in spcrtn.c
	and called by ProcessVeh */

work GetInput(work *);


/*	Fill routine defined in spcrtn.c
	and called throughout the program */

work *FillArray(unsigned, work *, work);


/*	Routines defined in spcrtn.c and called by ExitProgram */

void ExitInterrupt(void);
void ExitMessage(work);


/*	Other routines called throughout the program.
	Alloc and SetField are defined in prgdat.c and
	fldrtn.c respectively */

void *Alloc(size_t);
void SetField(work);

/*	Shorthand defines for Alloc */
#define ALLM(n) Alloc((size_t)(n) * sizeof(work))

#define ALLP(n) Alloc((size_t)(n) * sizeof(work *))

#endif /* DFN_HDR */
