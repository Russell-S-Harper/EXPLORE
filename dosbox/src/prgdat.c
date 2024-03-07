
/* prgdat.c */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "define.h"

/******** variables and data; no adjustments req'd ********/

void **mempnt;

void (*plot_pnt)(work, work), (*erase_pnt)(work, work);

char **vlyr, **sndlst, *ldtemp, sndoff[] = {IDLE};

work
	**recinfo, *lborder, *rborder, *tborder,
	*sndrnk, **vehtab, *vehrot,
	**fieldloc, *fieldpnt, *trigdata, *horzdata,
	**horizons, **polypnts,
	**fieldp_, **fieldlst, **addveh, **remveh,
	*field_, *fieldflg, *fieldveh,
	*fieldc_, *scan_, *coor_, *poly_, *vehic_,
	**vehtypp_, **vehicpnts, **vehfield, **vehpoly,
	*vehtypd_, *vehlngth, *vehstart,
	color, numpoly, playerx, playery, playerz, playerd,
	numreq, altcntr, dircntr,
	curfield = NOFLD, newfield = LMFLD,
	vgear = LGEAR, select = NOSEL, playern = LMVEH,
	dxscale, dyscale, xmax, ymax, map_mid_w, map_mid_h,
	lftinit, rgtinit, topinit, btminit, maxreq,
	dxmouse, dymouse, lbutton, rbutton;

volatile work vbcounter, quit_prg;

unsigned work current, map_w, map_h, t_mrk_w, t_mrk_m;

char scanveh[]=
	{22,23,21,24,20,17,18,16,19,15,12,13,11,14,10,7,8,6,9,5,4,2,3,1,0,
	22,23,24,21,20,17,18,19,16,15,12,13,14,11,10,7,8,9,6,5,2,3,4,1,0,
	24,22,23,21,20,19,17,18,16,15,13,14,12,11,10,8,9,7,6,5,3,4,2,1,0,
	24,23,22,21,19,18,20,17,16,14,13,15,12,11,9,8,10,7,6,4,5,3,2,1,0,
	24,23,22,19,21,18,17,20,14,16,13,15,12,9,11,8,10,7,4,6,3,2,5,1,0,
	24,23,22,19,18,21,17,14,20,13,16,12,9,15,11,8,4,7,10,6,3,2,5,1,0,
	24,23,19,22,18,14,21,17,13,20,16,9,12,8,15,11,4,7,10,3,6,2,5,1,0,
	24,23,19,18,22,14,21,17,13,9,20,12,16,8,4,11,15,7,3,10,6,2,5,1,0,
	24,19,23,18,14,22,13,17,9,21,12,8,16,4,20,7,11,3,15,6,2,10,1,5,0,
	24,19,23,18,14,22,9,13,17,21,4,12,8,16,20,7,3,11,15,2,6,10,1,5,0,
	24,19,23,14,18,22,9,13,17,4,8,21,12,16,3,7,20,11,2,15,6,10,1,5,0,
	24,19,14,23,18,9,13,22,4,17,8,12,3,21,7,16,20,11,2,6,15,10,1,5,0,
	24,19,14,23,9,18,13,4,22,8,17,3,12,21,7,16,2,11,20,6,15,10,1,5,0,
	24,19,14,9,23,18,4,13,8,22,17,3,12,7,21,16,2,11,6,20,1,15,10,5,0,
	24,14,19,9,4,23,13,18,8,3,17,22,12,7,2,16,21,11,6,1,15,20,10,5,0,
	14,19,24,9,4,13,18,23,8,3,12,17,22,7,2,11,16,21,6,1,10,15,20,5,0,
	4,14,9,19,24,3,13,8,18,23,7,2,12,17,22,6,1,11,16,21,10,5,0,15,20,
	14,9,4,19,24,13,8,3,18,23,12,7,2,17,22,11,6,1,16,21,10,5,0,15,20,
	4,14,9,19,24,3,13,8,18,23,7,2,12,17,22,6,1,11,16,21,5,0,10,15,20,
	4,9,14,19,3,8,24,13,18,2,7,23,12,17,1,6,22,11,16,0,21,5,10,15,20,
	4,9,14,3,19,8,13,24,2,18,7,23,12,1,17,6,22,11,0,16,5,10,21,15,20,
	4,9,14,3,8,19,13,2,24,7,18,12,1,23,17,6,0,11,22,16,5,10,21,15,20,
	4,9,3,14,8,2,19,13,7,24,18,1,12,6,23,17,0,11,22,5,16,10,21,15,20,
	4,9,3,8,14,2,19,13,7,1,24,12,18,6,0,17,23,11,5,22,16,10,21,15,20,
	4,3,9,8,2,14,7,13,1,19,12,6,18,0,24,11,17,5,23,16,10,22,15,21,20,
	4,3,9,8,2,14,1,7,13,19,0,12,6,18,24,11,5,17,23,10,16,22,15,21,20,
	4,3,9,2,8,14,1,7,13,0,6,19,12,18,5,11,24,17,10,23,16,22,15,21,20,
	4,3,2,9,8,1,7,14,0,13,6,12,5,19,11,18,24,17,10,16,23,22,15,21,20,
	4,3,2,9,1,8,7,0,14,6,13,5,12,19,11,18,10,17,24,16,23,22,15,21,20,
	4,3,2,1,9,8,0,7,6,14,13,5,12,11,19,18,10,17,16,24,15,23,22,21,20,
	4,2,3,1,0,9,7,8,6,5,13,14,12,11,10,18,19,17,16,15,23,24,22,21,20,
	2,3,4,1,0,7,8,9,6,5,12,13,14,11,10,17,18,19,16,15,22,23,24,21,20,
	2,1,0,3,4,7,6,5,8,9,12,11,10,13,14,17,16,15,18,19,22,21,20,23,24,
	2,1,0,3,4,7,6,5,8,9,12,11,10,13,14,17,16,15,18,19,22,21,20,23,24,
	0,2,1,3,4,5,7,6,8,9,11,10,12,13,14,16,15,17,18,19,21,20,22,23,24,
	0,1,2,3,5,6,4,7,8,10,11,9,12,13,15,16,14,17,18,20,19,21,22,23,24,
	0,1,2,5,3,6,7,4,10,8,11,9,12,15,13,16,14,17,20,18,21,22,19,23,24,
	0,1,2,5,6,3,7,10,4,11,8,12,15,9,13,16,20,17,14,18,21,22,19,23,24,
	0,1,5,2,6,10,3,7,11,4,8,15,12,16,9,13,20,17,14,21,18,22,19,23,24,
	0,1,5,6,2,10,3,7,11,15,4,12,8,16,20,13,9,17,21,14,18,22,19,23,24,
	0,5,1,6,10,2,11,7,15,3,12,16,8,20,4,17,13,21,9,18,22,14,23,19,24,
	0,5,1,6,10,2,15,11,7,3,20,12,16,8,4,17,21,13,9,22,18,14,23,19,24,
	0,5,1,10,6,2,15,11,7,20,16,3,12,8,21,17,4,13,22,9,18,14,23,19,24,
	0,5,10,1,6,15,11,2,20,7,16,12,21,3,17,8,4,13,22,18,9,14,23,19,24,
	0,5,10,1,15,6,11,20,2,16,7,21,12,3,17,8,22,13,4,18,9,14,23,19,24,
	0,5,10,15,1,6,20,11,16,2,7,21,12,17,3,8,22,13,18,4,23,9,14,19,24,
	0,10,5,15,20,1,11,6,16,21,7,2,12,17,22,8,3,13,18,23,9,4,14,19,24,
	10,5,0,15,20,11,6,1,16,21,12,7,2,17,22,13,8,3,18,23,14,9,4,19,24,
	10,15,20,5,0,11,16,21,6,1,12,17,7,22,2,13,18,8,23,3,14,9,19,4,24,
	10,15,20,5,0,11,16,21,6,1,12,17,22,7,2,13,18,23,8,3,14,19,24,9,4,
	20,10,15,5,0,21,11,16,6,1,17,22,12,7,2,18,23,13,8,3,19,24,14,9,4,
	20,15,10,5,21,16,0,11,6,22,17,1,12,7,23,18,2,13,8,24,3,19,14,9,4,
	20,15,10,21,5,16,11,0,22,6,17,1,12,23,7,18,2,13,24,8,19,14,3,9,4,
	20,15,10,21,16,5,11,22,0,17,6,12,23,1,7,18,24,13,2,8,19,14,3,9,4,
	20,15,21,10,16,22,5,11,17,0,6,23,12,18,1,7,24,13,2,19,8,14,3,9,4,
	20,15,21,16,10,22,5,11,17,23,0,12,6,18,24,7,1,13,19,2,8,14,3,9,4,
	20,21,15,16,22,10,17,11,23,5,12,18,6,24,0,13,7,19,1,8,14,2,9,3,4,
	20,21,15,16,22,10,23,17,11,5,24,12,18,6,0,13,19,7,1,14,8,2,9,3,4,
	20,21,15,22,16,10,23,17,11,24,18,5,12,6,19,13,0,7,14,1,8,2,9,3,4,
	20,21,22,15,16,23,17,10,24,11,18,12,19,5,13,6,0,7,14,8,1,2,9,3,4,
	20,21,22,15,23,16,17,24,10,18,11,19,12,5,13,6,14,7,0,8,1,2,9,3,4,
	20,21,22,23,15,16,24,17,18,10,11,19,12,13,5,6,14,7,8,0,9,1,2,3,4,
	20,22,21,23,24,15,17,16,18,19,11,10,12,13,14,6,5,7,8,9,1,0,2,3,4,
	22,21,20,23,24,17,16,15,18,19,12,11,10,13,14,7,6,5,8,9,2,1,0,3,4};

/******** routines; no modifications req'd ********/

void InitProgram(void);		/* Called by main */

/*	The following routines are called only by InitProgram or by
	routines called only by InitProgram unless indicated otherwise */

static void InitMath(void);
static void InitData(void);
static void InitRecord(void);
static void InitWall(void);
static void InitVehic(void);

void *Alloc(size_t);		/* Alloc is called throughout the program */

static void RdFile(char *, char *);
static void GetData(char *, unsigned work);
static work NextWord(void);

void ExitProgram(work);		/* Called throughout the program */
static void ExitData(void);	/* Called only by ExitProgram */

void InitProgram(void)
{
	InitGraphics();	/* Machine specific 'inits' */
	InitPlot();
	InitMouse();
	InitInterrupt();

	InitMath();	/* Generic 'inits' */
	InitData();
	InitRecord();
	InitWall();
	InitVehic();
}

#define DCPI	3.141592653589793	/* Define used in InitMath */

static void InitMath(void)
{
	work t, *p;
	double d, i;

	p = trigdata = ALLM(FCIRC);
	d = 0.0;
	i = 2.0*DCPI/FCIRC;
	for (t = 0; t < FCIRC; t++, p++, d += i)
		*p = (work)(sin(d) * SIN90);

	p = horzdata = ALLM(FCIRC/2);
	d = DCPI/-2.0 + 2.0*DCPI/FCIRC;
	for (t = 0; t < (FCIRC/2 - 1); t++, p++, d += i)
		*p = (work)(tan(d) * dxscale) + map_mid_w;

	/* Adjust the pointer to midway in the table */
	horzdata += (FCIRC/4 - 1);
}

/*	Defines used by InitData */
#define MIDFX	(MAXFX/2+1)
#define MIDFY	(MAXFY/2+1)
#define MIDFZ	256
#define UNIT	(unsigned work)sizeof(work)	/* Shorthand */
#define FILSZ	(size_t)(0x8000/sizeof(char))	/* Buffer size */

static void InitData(void)
{
	char code, *ctemp, *membuf;
	register work d, t, *p, *q;
	work *mem, *arr, cnt, num, id, fstat, dat[3];

	p = scan_ = ALLM(SCANRSV);
	p = coor_ = ALLM(COORRSV);
	p = poly_ = ALLM(POLYRSV);

	vehtypp_ = ALLP(VTPRSV);
	vehtypd_ = ALLM(VTDRSV);
	vehicpnts = vehtypp_ + VEHICPNTS;
	vehfield = vehtypp_ + VEHFIELD;
	vehpoly = vehtypp_ + VEHPOLY;
	vehlngth = vehtypd_ + VEHLNGTH;
	vehstart = vehtypd_ + VEHSTART;
	for (t = 0; t < MVEHTYP; t++) vehicpnts[t] = 0;

	p = vehic_ = ALLM(VEHRSV);
	for (d = 1, t = MACTVEH; t; t--, p++) {
		p[VEHICA] = VCLR;
		p[VEHICT] = NOSEL;
		p[VEHICF] = NOFLD;
		p[VEHICS] = d;
		p[VEHICU] = ~d;
		p[VEHICC] = VCLR;
		d += d;
	}

	fieldp_ = ALLP(FPRSV);
	fieldlst = fieldp_ + FLST;
	addveh = fieldp_ + AVEH;
	remveh = fieldp_ + DVEH;

	p = fieldc_ = ALLM(FCRSV);
	for (t = MAXFLD; t; t--, p++) {
		p[RECFX] = MIDFX;
		p[RECFY] = MIDFY;
		p[RECFZ] = MIDFZ;
	}

	field_ = ALLM(FLDRSV);
	fieldflg = field_ + FIELDFLG;
	fieldveh = field_ + FIELDVEH;

	sndlst = Alloc((size_t)MAXSND * sizeof(char *));
	sndrnk = ALLM(MAXSND);
	for (t = 0; t < NCHAN; t++) {
		sndlst[t] = sndoff;
		sndrnk[t] = 0;
	}

	polypnts = ALLP(MPLYFIG);
	fieldloc = ALLP(MACTVEH);
	horizons = ALLP(MFLDHZ);

	if (!(membuf = (char *)malloc(FILSZ))) ExitProgram(MAERR);

/*	RdFile("zcndfil.dat", membuf);*/
	RdFile("savefil.dat", membuf);
	fstat = 1;
	while (fstat) {

		GetData(&code, sizeof(char));

		switch (code) {

			case HZCODE:
				GetData((char *)&cnt, UNIT);
				mem = Alloc((size_t)cnt);

				GetData((char *)mem, cnt);
				if (*mem >= MFLDHZ) ExitProgram(FCERR);
				p = horizons[*mem] = mem + 1;
				d = map_mid_h;

				/* Process the new horizon */
				while (*p != COLBAK) {
					p += 2;
					*p = d - SlopeCalc(*p, d, FCIRC);
					p += 2;
					*p = d - SlopeCalc(*p, d, FCIRC);
					p++;
				}
				break;

			case SDCODE:
				GetData((char *)&cnt, UNIT);
				ctemp = Alloc((size_t)cnt);

				while ((id = NextWord()) >= LMEOR) {
					if (id >= MAXSND) ExitProgram(FCERR);
					GetData((char *)&sndrnk[id], UNIT);
					GetData((char *)&num, UNIT);
					GetData(ctemp, sizeof(char) * num);
					sndlst[id] = ctemp;
					ctemp += num;
				}
				break;

			case VPCODE:
				GetData((char *)dat, UNIT * 3);
				num = dat[0] * FLYR + dat[1] * XYZ + dat[2];
				mem = ALLM(num);

				for (num = 0; num < dat[0]; num++) {
					GetData((char *)&id, UNIT);
					if (id >= MVEHTYP) ExitProgram(FCERR);
					GetData((char *)&cnt, UNIT);
					if ((*mem = cnt) >= MOBJPNT)
						ExitProgram(FCERR);

					GetData((char *)(mem + 1), UNIT * cnt * XYZ);
					vehicpnts[id] = mem;
					mem += cnt * XYZ + 1;

					GetData((char *)mem, UNIT * FLYR);
					vehfield[id] = mem;
					mem += FLYR;

					GetData((char *)&cnt, UNIT);
					GetData((char *)mem, UNIT * cnt);
					vehpoly[id] = mem;
					mem += cnt;
				}
				break;

			case VDCODE:
				while ((id = NextWord()) >= LMEOR) {
					if (id >= MVEHFIG) ExitProgram(FCERR);
					arr = vehic_ + id;

					GetData((char *)&arr[VEHICX], UNIT);
					GetData((char *)&arr[VEHICY], UNIT);
					GetData((char *)&arr[VEHICZ], UNIT);
					GetData((char *)&arr[VEHICD], UNIT);
					GetData((char *)&arr[VEHICI], UNIT);
					GetData((char *)&arr[VEHICP], UNIT);
					GetData((char *)&arr[VEHICT], UNIT);
					GetData((char *)&arr[VEHICE], UNIT);
					arr[VEHICL] = arr[VEHICH] = VEHINIT;
					arr[VEHICA] = VIDL;
				}
				break;

			case PPCODE:
				GetData((char *)&num, UNIT);
				mem = ALLM(num * XYZ);

				while ((id = NextWord()) >= LMEOR) {
					if (id >= MPLYFIG) ExitProgram(FCERR);
					GetData((char *)&num, UNIT);
					GetData((char *)mem, UNIT * num * XYZ);
					polypnts[id] = mem;
					mem += num * XYZ;
				}
				break;

			case PDCODE:
				while ((id = NextWord()) >= LMEOR) {
					if (id >= MPLYFIG) ExitProgram(FCERR);
					arr = poly_ + id;

					GetData((char *)&arr[PBTM], UNIT);
					GetData((char *)&arr[PTOP], UNIT);
					GetData((char *)&arr[PTYP], UNIT);
					GetData((char *)&arr[PCOL], UNIT);
					GetData((char *)&arr[PDEF], UNIT);
					GetData((char *)&arr[PSPC], UNIT);
					GetData((char *)&arr[PLML], UNIT);
					GetData((char *)&arr[PLMH], UNIT);
					GetData((char *)&arr[PDAT], UNIT);
				}
				break;

			case FDCODE:
				GetData((char *)&num, UNIT);
				mem = ALLM(num * (FLDLEN + 2 * MACTVEH));

				for (t = num; t > 0; t--) {
					GetData((char *)&id, UNIT);
					if (id >= MAXFLD) ExitProgram(FCERR);
					arr = fieldc_ + id;

					GetData((char *)&arr[FLDTP], UNIT);
					GetData((char *)&arr[HORZN], UNIT);
					GetData((char *)&arr[FLDSX], UNIT);
					GetData((char *)&arr[FLDSY], UNIT);
					GetData((char *)&arr[FLDSZ], UNIT);

					GetData((char *)mem, UNIT * FLDLEN);
					fieldlst[id] = mem;
					mem += FLDLEN;

					GetData((char *)mem, UNIT * MACTVEH);
					addveh[id] = mem;
					mem += MACTVEH;

					GetData((char *)mem, UNIT * MACTVEH);
					remveh[id] = mem;
					mem += MACTVEH;
				}
				break;

			case EFCODE:
				fstat = 0;
				break;

			default:
				ExitProgram(FCERR);
		}
	}

	free(membuf);
}

static void InitRecord(void)
{
	work t, **ri;

	lborder = ALLM(map_h);
	rborder = ALLM(map_h);

	FillArray(map_h, lborder, lftinit);
	FillArray(map_h, rborder, rgtinit);

	ri = recinfo = ALLP(map_h);

	/* Set up in recinfo, the address for the beginning of each
	horizontal line of the map record */

	for (t = 0; t < map_h; t++, ri++) {
		*ri = ALLM(map_w);
		FillArray(map_w, *ri, RECINIT);
	}

	current = 1;
}

static void InitWall(void)
{
	tborder = ALLM(map_w);
	FillArray(map_w, tborder, topinit);
}

static void InitVehic(void)
{
	char *sc, **vl;
	work a, c, r, s, t, u, v, x, y, *d, *rot, **vt;

	vl = vlyr = Alloc((size_t)VEHDIR * sizeof(char *));
	vt = vehtab = ALLP(VEHDIR);

	for (v = u = t = 0; t < MVEHTYP; t++) {
		if (vehicpnts[t]) {
			u += vehlngth[t] = s = *vehicpnts[t];
			vehstart[t] = v;
			v += s * XYZ;
		}
	}

	rot = vehrot = ALLM(u * XYZ * VEHDIR);

	for (sc = scanveh, t = a = 0; t < VEHDIR;
		sc += VLYR, t++, a += FCIRC/VEHDIR) {
		s = Sin(a);
		c = Cos(a);
		*vt++ = rot;
		*vl++ = sc;

		for (r = 0; r < MVEHTYP; r++) {
			if (d = vehicpnts[r]) {

				for (u = *d++; u > 0; u--) {
					x = *d++;
					y = *d++;
	
					/* Note that the order in vehrot is y, x, z */
					*rot++ = SpecMult(y, c) - SpecMult(x, s);
					*rot++ = SpecMult(x, c) + SpecMult(y, s);
					*rot++ = *d++;
				}
			}
		}
	}

	FillArray(FLDLEN, fieldveh, VEHINIT);
	SetField(NOPSPC);
}

/*	Memory allocator. ALLM and ALLP are defined as shorthand
	notations for some common allocation requirements */

void *Alloc(size_t memspc)
{
	void *ctemp;

	if (!mempnt) {
		mempnt = malloc((size_t)maxreq * sizeof(void *));
		mempnt[numreq++] = (void *)mempnt;
	}

	if (numreq >= maxreq) ExitProgram(FCERR);
	else if (mempnt[numreq++] = ctemp = malloc(memspc))
		return ctemp;
	else ExitProgram(MAERR);
}

static void RdFile(char *rdfile, char *membuf)
{
	register unsigned char c;
	register char *vp = membuf;
	char *vq;
	register work t;
	FILE *rdspec;

	if (!(rdspec = fopen(rdfile, "rb"))) ExitProgram(FOERR);

	vq = vp + FILSZ - 2 * MCHAR * sizeof(work);

	while (*vp++ = fgetc(rdspec), !feof(rdspec))
		if (vp >= vq) ExitProgram(FCERR);

/*	while (c = fgetc(rdspec), !feof(rdspec)) {

		if (c == SPSEQ) {
			*vp++ = fgetc(rdspec);
			*vp++ = 0;
		}

		else if (c >= MCHAR)
			for (t = c - MCHAR; t > 0; t--)
				*vp++ = fgetc(rdspec);

		else if (c == STSEQ) {
			for (t = fgetc(rdspec); t > 0; t--) {
				*vp++ = fgetc(rdspec);
				*vp++ = 0;
			}
		}

		else for (t = c, c = fgetc(rdspec); t > 0; t--)
				*vp++ = c;

		if (vp >= vq) ExitProgram(FCERR);
	}*/

	fclose(rdspec);
	ldtemp = membuf;
}

static void GetData(register char *vp, register unsigned work n)
{
	register char *vq = ldtemp;

	while (n--) *vp++ = *vq++;
	ldtemp = vq;
}

static work NextWord(void)
{
	work tmp;

	GetData((char *)&tmp, UNIT);
	return tmp;
}

void ExitProgram(work stat)
{
	ExitData();
	ExitInterrupt();
	ExitMessage(stat);

	if (stat != NOERR) exit(stat);
}

static void ExitData(void)
{
	while (numreq--) free(mempnt[numreq]);
}
