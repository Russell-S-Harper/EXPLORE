
/* objrtn.c */

#include <stdlib.h>
#include "define.h"
#include "declar.h"

/******** routines; no modifications req'd ********/

void RenderObj(void);	/* Called by main */
static void Vehicle(work);
static void VehCase(work *);

static void GetPPoints(work, work);
static void RenderSeg(void (*)(work, work, work, work), work, work);
static void ZPlane(work, work);

static void DrawLine(work, work, work, work);
static void WallLine(work, work, work, work);
static void PolyLine(work, work, work, work);
static void PolyUpdate(void);

static void TopAdjust(work, work);
static void LftAdjust(work, work);
static void RgtAdjust(work, work);

static void DrawTarget(work, work);

/*	Define used in RenderObj */
#define LIMSCAN	(VIEWA-1)

/*	Variables used in routines called by RenderObj */
static work xtemp, ytemp, targx, targy, flgsel, cursin, curcos;

void RenderObj(void)
{
	register work i, n, t, vt, *ps = scan_, *pv, *pp;

	cursin = Sin(playerd);
	curcos = Cos(playerd);
	flgsel = NOSEL;

	for (n = 0; n < numpoly; n++, ps++) {
		if ((t = ps[POLY]) < LMVEH) {
			ps[POLY] = (t & WRKABS);
			vt = ps[VEHIC];
			t = 1;
		}

		pp = poly_ + t;
		color = pp[PCOL];
		switch (pp[PTYP]) {

			case VEHCLE:
				for (i = 0, pv = vehic_; vt; i++, pv++) {
					if ((vt & pv[VEHICS]) && (pv[VEHICF] == curfield))
						Vehicle(i);
					vt &= pv[VEHICU];
				}

				/* Redo n and ps without any vehicle data */
				n--;
				ps--;
				break;

			case FULWAL:
				GetPPoints(n, 4);
				RenderSeg(DrawLine, 0, 1);
				RenderSeg(WallLine, 0, 3);
				RenderSeg(WallLine, 1, 2);
				RenderSeg(WallLine, 2, 3);
				break;

			case TOPBTM:
				GetPPoints(n, 4);
				RenderSeg(DrawLine, 0, 1);
				RenderSeg(WallLine, 2, 3);
				break;

			case RGTEDG:
				GetPPoints(n, 4);
				RenderSeg(DrawLine, 0, 1);
				RenderSeg(WallLine, 0, 3);
				RenderSeg(WallLine, 2, 3);
				break;

			case LFTEDG:
				GetPPoints(n, 4);
				RenderSeg(DrawLine, 0, 1);
				RenderSeg(WallLine, 1, 2);
				RenderSeg(WallLine, 2, 3);
				break;

			case PENTGN:
				GetPPoints(n, 5);
				RenderSeg(DrawLine, 0, 1);
				RenderSeg(WallLine, 0, 4);
				RenderSeg(WallLine, 1, 2);
				RenderSeg(WallLine, 3, 4);
				RenderSeg(WallLine, 2, 3);
				break;

			case WALL3E:
				GetPPoints(n, 3);
				RenderSeg(DrawLine, 0, 1);
				RenderSeg(WallLine, 1, 2);
				RenderSeg(WallLine, 0, 2);
				break;

			case SNGDRW:
				GetPPoints(n, 2);
				RenderSeg(DrawLine, 0, 1);
				break;

			case TPONLY:
				GetPPoints(n, 2);
				RenderSeg(WallLine, 0, 1);
				break;

			case POLYS4:
				GetPPoints(n, 4);
				RenderSeg(PolyLine, 0, 1);
				RenderSeg(PolyLine, 0, 3);
				RenderSeg(PolyLine, 1, 2);
				RenderSeg(PolyLine, 2, 3);
				PolyUpdate();
				break;

			case DRWMSK:
				GetPPoints(n, 4);
				RenderSeg(DrawLine, 0, 1);
				color = COLBAK;
				RenderSeg(WallLine, 2, 3);
				break;

			case SPEC00:
				GetPPoints(n, 7);
				RenderSeg(PolyLine, 0, 1);
				RenderSeg(PolyLine, 0, 3);
				RenderSeg(PolyLine, 1, 2);
				RenderSeg(PolyLine, 2, 3);
				PolyUpdate();

				RenderSeg(PolyLine, 3, 4);
				RenderSeg(PolyLine, 2, 6);
				color = COLBAK;
				RenderSeg(PolyLine, 2, 3);
				RenderSeg(PolyLine, 4, 5);
				RenderSeg(PolyLine, 6, 5);
				PolyUpdate();
				break;

			case SPEC01:
				GetPPoints(n, 6);
				RenderSeg(DrawLine, 0, 1);
				RenderSeg(DrawLine, 0, 3);
				RenderSeg(DrawLine, 1, 2);
				RenderSeg(DrawLine, 2, 3);
				RenderSeg(DrawLine, 3, 4);
				RenderSeg(DrawLine, 2, 5);
				RenderSeg(DrawLine, 2, 4);
				RenderSeg(DrawLine, 3, 5);
		}
	}

	/* Draw horizon */
	if ((vt = fieldc_[curfield + HORZN]) >= LMFLD) {
		pv = horizons[vt];
		vt = playerd;
	
		while (*pv != COLBAK) {
			color = *pv++;
			i = (((*pv++ - vt) + FCIRC/2) & (FCIRC-1)) - FCIRC/2;
			if (i < -LIMSCAN || i > LIMSCAN) {
				pv += 3;
				continue;
			}
			t = *pv++;
			n = (((*pv++ - vt) + FCIRC/2) & (FCIRC-1)) - FCIRC/2;
			if (n < -LIMSCAN || n > LIMSCAN) {
				pv++;
				continue;
			}
			DrawLine(horzdata[i], t, horzdata[n], *pv++);
		}
	}
			
	/* Draw special target */
	if (flgsel != NOSEL) {
		color = COLSPC;
		DrawTarget(targx, targy);
	}
}

/*	Defines used in Vehicle and in routines called by Vehicle */
#define LFTC	0
#define RGTC	1
#define TOPC	2
#define BTMC	3
#define LRTB	4

/*	MINCOOR is a very important constant related to	the
	map resolution, the ceiling of the field, and the
	precision of the integer type <work> */
#define MINCOOR 23

#define DIRSHFT	(FCSHFT - VDSHFT)

/*	Data used in Vehicle */
static char arctan[]=
	{0x03, 0x08, 0x0D, 0x12, 0x16, 0x1B, 0x1F, 0x24,
	0x28, 0x2C, 0x2F, 0x33, 0x36, 0x39, 0x3C, 0x3F,
	0x41, 0x44, 0x46, 0x48, 0x4A, 0x4C, 0x4E, 0x4F,
	0x51, 0x52, 0x54, 0x55, 0x56, 0x58, 0x59, 0x5A,
	0x5B, 0x5C, 0x5D, 0x5D, 0x5E, 0x5F, 0x60, 0x61,
	0x61, 0x62, 0x63, 0x63, 0x64, 0x64, 0x65, 0x66,
	0x66, 0x67, 0x67, 0x67, 0x68, 0x68, 0x69, 0x69,
	0x6A, 0x6A, 0x6A, 0x6B, 0x6B, 0x6B, 0x6C, 0x6C, 0x6C};

/*	Arctan-related defines */
#define ATNLIM	(work)(sizeof arctan/sizeof(char)-1)
#define ATNADJ \
	(work)(1 << DGTSHFT)	/* To improve accuracy of arctan */

static void Vehicle(work t)
{
	register work va, vb, vc, vd, *pv, *pc;
	work dd, vi, pt[LRTB], pstat, ta, td;
	register char *vs;

	pc = vehic_ + t;
	if ((vi = pc[VEHICI]) < LMVEH) return;
	vc = pc[VEHICX] - playerx;
	vd = pc[VEHICY] - playery;
	dd = pc[VEHICD] - playerd;

	pv = vehtab[(dd & (FCIRC-1)) >> DIRSHFT] + vehstart[vi];
	va = SpecMult(vc, curcos) - SpecMult(vd, cursin);
	vb = SpecMult(vd, curcos) + SpecMult(vc, cursin);
	vd = pc[VEHICZ] - playerz;

	if (select != NOSEL && pc[VEHICT] == DOSEL && vb > MINCOOR) {
		if (select == NXSEL) select = t;
		if (select == t) {
			flgsel = DOSEL;
			targx = SlopeCalc(va, dxscale, vb) + map_mid_w;
			targy = SlopeCalc(vd, dyscale, vb) + map_mid_h;
		}
		else {
			color = COLNRM;
			DrawTarget(SlopeCalc(va, dxscale, vb) + map_mid_w,
				SlopeCalc(vd, dyscale, vb) + map_mid_h);
		}
	}

	pc = coor_;
	pt[LFTC] = lftinit;
	pt[RGTC] = rgtinit;
	pt[TOPC] = topinit;
	pt[BTMC] = btminit;

	for (t = vehlngth[vi]; t; t--, pc++) {

		/* Note that the vehicle data is ordered y, x, z */
		vc = vb + *pv++;
		if (vc > MINCOOR) {
			pc[MAP_X] = SlopeCalc(va + *pv++, dxscale, vc) +
				map_mid_w;
			pc[MAP_Y] = SlopeCalc(vd + *pv++, dyscale, vc) +
				map_mid_h;
			pc[YCOOR] = 1;

			if (pt[LFTC] > pc[MAP_X]) pt[LFTC] = pc[MAP_X];
			if (pt[RGTC] < pc[MAP_X]) pt[RGTC] = pc[MAP_X];
			if (pt[TOPC] > pc[MAP_Y]) pt[TOPC] = pc[MAP_Y];
			if (pt[BTMC] < pc[MAP_Y]) pt[BTMC] = pc[MAP_Y];
		}
		else {
			pc[YCOOR] = 0;
			pv += 2;
		}
	}

	ta = va;	/* l/r adj: save for later */
	td = vd;	/* Delta z: save for later */

	if (pt[BTMC] == btminit || (va = pt[LFTC]) == lftinit ||
		(vc = pt[RGTC]) == rgtinit || (vd = pt[TOPC]) == topinit)
		return;

	if (va < XMIN) va = XMIN;
	if (vc > xmax) vc = xmax;
	if (vd < YMIN) vd = YMIN;

	for (pv = tborder + va, pstat = 1; va <= vc; va++, pv++) {
		if (vd < *pv) {
			pstat = 0;
			break;
		}
	}
	if (pstat) return;

	if (vb > MINCOOR/2) va = SlopeCalc(ATNADJ, ta, vb);
	else va = ta;
	if (va < 0) {
		va = -va;
		vb = 1;
	}
	else vb = 0;
	if (va > ATNLIM) va = ATNLIM;
	va = arctan[va];
	if (vb) va = -va;

	pv = vehpoly[vi];
	vs = vlyr[((dd - va) >> DIRSHFT) & (VEHDIR-1)];
	pc = vehfield[vi];

	if (td < 0) {	/* ie. player is higher than target */
		va = 0;
		vb = VLYR*2;
	}
	else {
		va = VLYR*2;
		vb = 0;
	}

	for (t = VLYR; t; t--) {
		vc = *vs++;
		vd = pc[vc + va];
		if (vd >= LMVEH) VehCase(pv + vd);
		vd = pc[vc + VLYR];
		if (vd >= LMVEH) VehCase(pv + vd);
		vd = pc[vc + vb];
		if (vd >= LMVEH) VehCase(pv + vd);
	}
}

static void VehCase(register work *p)
{
	register work p0, p1, p2, p3, *cp = coor_, *cq;

	/* For a recursive Vehicle call, add an offset to coor_ of
	COORSEG, call Vehicle, then restore coor_ */

	switch (*p++) {
		case PLY3:
			color = *p++;
			p0 = *p++;
			p1 = *p++;
			p2 = *p;
			if (cp[p0 + YCOOR] && cp[p1 + YCOOR] && cp[p2 + YCOOR]) {
				cq = cp + MAP_Y;
				cp += MAP_X;
				PolyLine(cp[p0], cq[p0], cp[p1], cq[p1]);
				PolyLine(cp[p1], cq[p1], cp[p2], cq[p2]);
				PolyLine(cp[p2], cq[p2], cp[p0], cq[p0]);
				PolyUpdate();
			}
			break;

		case PLY4:
			color = *p++;
			p0 = *p++;
			p1 = *p++;
			p2 = *p++;
			p3 = *p;
			if (cp[p0 + YCOOR] && cp[p1 + YCOOR] &&
				cp[p2 + YCOOR] && cp[p3 + YCOOR]) {
				cq = cp + MAP_Y;
				cp += MAP_X;
				PolyLine(cp[p0], cq[p0], cp[p1], cq[p1]);
				PolyLine(cp[p1], cq[p1], cp[p2], cq[p2]);
				PolyLine(cp[p2], cq[p2], cp[p3], cq[p3]);
				PolyLine(cp[p3], cq[p3], cp[p0], cq[p0]);
				PolyUpdate();
			}
			break;

		case WAL3:
			color = *p++;
			p0 = *p++;
			p1 = *p++;
			p2 = *p;
			if (cp[p0 + YCOOR] && cp[p1 + YCOOR] && cp[p2 + YCOOR]) {
				cq = cp + MAP_Y;
				cp += MAP_X;
				DrawLine(cp[p0], cq[p0], cp[p1], cq[p1]);
				WallLine(cp[p0], cq[p0], cp[p2], cq[p2]);
				WallLine(cp[p1], cq[p1], cp[p2], cq[p2]);
			}
			break;

		case WAL4:
			color = *p++;
			p0 = *p++;
			p1 = *p++;
			p2 = *p++;
			p3 = *p;
			if (cp[p0 + YCOOR] && cp[p1 + YCOOR] &&
				cp[p2 + YCOOR] && cp[p3 + YCOOR]) {
				cq = cp + MAP_Y;
				cp += MAP_X;
				DrawLine(cp[p0], cq[p0], cp[p1], cq[p1]);
				WallLine(cp[p0], cq[p0], cp[p3], cq[p3]);
				WallLine(cp[p1], cq[p1], cp[p2], cq[p2]);
				WallLine(cp[p2], cq[p2], cp[p3], cq[p3]);
			}
			break;

		case DRW1:
			color = *p++;
			p0 = *p++;
			p1 = *p;
			if (cp[p0 + YCOOR] && cp[p1 + YCOOR]) {
				cq = cp + MAP_Y;
				cp += MAP_X;
				DrawLine(cp[p0], cq[p0], cp[p1], cq[p1]);
			}
			break;

		case DRW4:
			color = *p++;
			p0 = *p++;
			p1 = *p++;
			p2 = *p++;
			p3 = *p;
			if (cp[p0 + YCOOR] && cp[p1 + YCOOR] &&
				cp[p2 + YCOOR] && cp[p3 + YCOOR]) {
				cq = cp + MAP_Y;
				cp += MAP_X;
				DrawLine(cp[p0], cq[p0], cp[p1], cq[p1]);
				DrawLine(cp[p0], cq[p0], cp[p3], cq[p3]);
				DrawLine(cp[p1], cq[p1], cp[p2], cq[p2]);
				DrawLine(cp[p2], cq[p2], cp[p3], cq[p3]);
			}
			break;

		case SPC0:
			color = *p++;
			p0 = *p++;
			p1 = *p++;
			p2 = *p++;
			p3 = *p;
			if (cp[p0 + YCOOR] && cp[p1 + YCOOR] &&
				cp[p2 + YCOOR] && cp[p3 + YCOOR]) {
				cq = cp + MAP_Y;
				cp += MAP_X;
				DrawLine(cp[p0], cq[p0], cp[p3], cq[p3]);
				DrawLine(cp[p1], cq[p1], cp[p2], cq[p2]);
				DrawLine(cp[p2], cq[p2], cp[p3], cq[p3]);
			}
			break;

		case SPC1:
			color = *p++;
			p0 = *p++;
			p1 = *p++;
			p2 = *p++;
			p3 = *p;
			if (cp[p0 + YCOOR] && cp[p1 + YCOOR] &&
				cp[p2 + YCOOR] && cp[p3 + YCOOR]) {
				cq = cp + MAP_Y;
				cp += MAP_X;
				PolyLine(cp[p0], cq[p0], cp[p3], cq[p3]);
				PolyLine(cp[p1], cq[p1], cp[p2], cq[p2]);
				PolyLine(cp[p2], cq[p2], cp[p3], cq[p3]);
				color = COLBAK;
				PolyLine(cp[p0], cq[p0], cp[p1], cq[p1]);
				PolyUpdate();

			}
	}
}

static void GetPPoints(work id, register work n)
{
	register work xt, yt, yc, *pp, *pc;
	work rotx, roty;

	pc = scan_ + id;
	rotx = pc[SCANX] - playerx;
	roty = pc[SCANY] - playery;

	pp = polypnts[poly_[pc[POLY] + PDEF]];
	pc = coor_;

	for (; n; n--, pc++) {
		xt = rotx + *pp++;
		yt = roty + *pp++;

		pc[XCOOR] = SpecMult(curcos, xt) - SpecMult(cursin, yt);
		pc[YCOOR] = yc = SpecMult(curcos, yt) + SpecMult(cursin, xt);
		pc[ZCOOR] = *pp++ - playerz;

		if (yc > MINCOOR) {
			pc[MAP_X] = SlopeCalc(pc[XCOOR], dxscale, yc) + map_mid_w;
			pc[MAP_Y] = SlopeCalc(pc[ZCOOR], dyscale, yc) + map_mid_h;
		}
	}
}

static void RenderSeg(void (*render)(work, work, work, work), work a, work b)
{
	register work ya, yb, yl = MINCOOR, *ca, *cb = coor_;

	ca = cb + a;
	cb += b;
	ya = ca[YCOOR];
	yb = cb[YCOOR];
	if (ya > yl && yb > yl) {
		(*render)(ca[MAP_X], ca[MAP_Y], cb[MAP_X], cb[MAP_Y]);
		return;
	}
	if (ya <= yl && yb <= yl) return;
	if (ya <= yl) {
		ZPlane(a, b);
		(*render)(xtemp, ytemp, cb[MAP_X], cb[MAP_Y]);
	}
	else {
		ZPlane(b, a);
		(*render)(ca[MAP_X], ca[MAP_Y], xtemp, ytemp);
	}
}

static void ZPlane(work a, work b)
{
	register work yb, yd, *ca, *cb = coor_;

	ca = cb + a;
	cb += b;
	yd = ca[YCOOR] - cb[YCOOR];
	yb = MINCOOR - cb[YCOOR];

	xtemp = SlopeCalc(cb[XCOOR] + SlopeCalc(yb,
		ca[XCOOR] - cb[XCOOR], yd), dxscale, MINCOOR) + map_mid_w;

	ytemp = SlopeCalc(cb[ZCOOR] + SlopeCalc(yb,
		ca[ZCOOR] - cb[ZCOOR], yd), dyscale, MINCOOR) + map_mid_h;
}

/*	Defines used in Draw/Wall/PolyLine */
#define SpecPrec(a)	((prec)(a)<<DGTSHFT)
#define HALFCN \
	(work)((1L << 4*DGTSHFT)/2) /* == 0.5 in certain contexts */

static void DrawLine(work xa, work ya, work xb, work yb)
{
	register work *at, *tb, **ri, xt, yt, cnt, cur;
	register void (*pp)(work, work);
	work xdelta, ydelta, xsign, xabs, counter, temp;
	prec xstart, ystart, xinc, yinc;

	if (xa > xb) {
		temp = xa; xa = xb; xb = temp;
		temp = ya; ya = yb; yb = temp;
	}
	if (xb < XMIN) return;
	if (xa > xmax) return;
	if (xa < XMIN) {
		ya = ya + SlopeCalc((XMIN - xa), (yb - ya), (xb - xa));
		xa = XMIN;
	}
	if (xb > xmax) {
		yb = yb + SlopeCalc((xmax - xb), (yb - ya), (xb - xa));
		xb = xmax;
	}
	if (ya > yb) {
		temp = xa; xa = xb; xb = temp;
		temp = ya; ya = yb; yb = temp;
	}
	if (yb < YMIN) return;
	if (ya > ymax) return;
	if (ya < YMIN) {
		xa = xa + SlopeCalc((YMIN - ya), (xb - xa), (yb - ya));
		ya = YMIN;
	}
	if (yb > ymax) {
		xb = xb + SlopeCalc((ymax - yb), (xb - xa), (yb - ya));
		yb = ymax;
	}
	/* Points are now within boundaries and yb > ya */

	xdelta = xb - xa;
	ydelta = yb - ya;
	if (ydelta || xdelta) {
		xabs = (xsign = (xdelta < 0))? -xdelta: xdelta;
		if (ydelta > xabs) {
			counter = ydelta;
			xdelta = SpecDiv(xdelta, ydelta);
			ydelta = SIN90;
		}
		else {
			counter = xabs;
			ydelta = SpecDiv(ydelta, xabs);
			xdelta = (xsign)? -SIN90: SIN90;
		}

		PackPrec(xstart, xa, HALFCN);
		PackPrec(ystart, ya, HALFCN);
		xinc = SpecPrec(xdelta);
		yinc = SpecPrec(ydelta);

		tb = tborder;
		ri = recinfo;
		pp = plot_pnt;
		cur = current;

		for (cnt = counter; cnt >= 0; cnt--) {
			xt = Prec2nd(xstart);
			yt = Prec2nd(ystart);
			at = ri[yt] + xt;
			if (*at != cur) {
				*at = cur;
				if (tb[xt] > yt) (*pp)(xt, yt);
			}
			xstart += xinc;
			ystart += yinc;
		}
	}
}

static void WallLine(work xa, work ya, work xb, work yb)
{
	register work *at, *bt, *tb, xt, yt, cnt, cur;
	register void (*pp)(work, work);
	work xdelta, ydelta, xsign, xabs, counter, temp;
	prec xstart, ystart, xinc, yinc;

	if (xa > xb) {
		temp = xa; xa = xb; xb = temp;
		temp = ya; ya = yb; yb = temp;
	}
	if (xb < XMIN) return;
	if (xa > xmax) return;
	if (xa < XMIN) {
		ya = ya + SlopeCalc((XMIN - xa), (yb - ya), (xb - xa));
		xa = XMIN;
	}
	if (xb > xmax) {
		yb = yb + SlopeCalc((xmax - xb), (yb - ya), (xb - xa));
		xb = xmax;
	}
	if (ya > yb) {
		temp = xa; xa = xb; xb = temp;
		temp = ya; ya = yb; yb = temp;
	}
	if (yb < YMIN) {
		TopAdjust(xa, xb);
		return;
	}
	if (ya > ymax) return;
	if (ya < YMIN) {
		temp = xa + SlopeCalc((YMIN - ya), (xb - xa), (yb - ya));
		if (temp != xa) {
			if (temp < xa) TopAdjust(xa, temp+1);
			else TopAdjust(xa, temp-1);
		}
		xa = temp;
		ya = YMIN;
	}
	if (yb > ymax) {
		xb = xb + SlopeCalc((ymax - yb), (xb - xa), (yb - ya));
		yb = ymax;
	}
	/* Points are now within boundaries and yb > ya */

	xdelta = xb - xa;
	ydelta = yb - ya;
	if (ydelta || xdelta) {
		xabs = (xsign = (xdelta < 0))? -xdelta: xdelta;
		if (ydelta > xabs) {
			counter = ydelta;
			xdelta = SpecDiv(xdelta, ydelta);
			ydelta = SIN90;
		}
		else {
			counter = xabs;
			ydelta = SpecDiv(ydelta, xabs);
			xdelta = (xsign)? -SIN90: SIN90;
		}

		PackPrec(xstart, xb, HALFCN);
		PackPrec(ystart, yb, HALFCN);
		xinc = SpecPrec(-xdelta);
		yinc = SpecPrec(-ydelta);

		tb = tborder;
		pp = plot_pnt;
		cur = current;

		for (cnt = counter; cnt >= 0; cnt--) {
			xt = Prec2nd(xstart);
			yt = Prec2nd(ystart);
			at = recinfo[yt];
			bt = tb + xt;
			if (*bt > yt) {
				*bt = yt;
				if (at[xt] != cur) (*pp)(xt, yt);
			}
			xstart += xinc;
			ystart += yinc;
		}
	}
	else {
		if (xa < XMIN || xa > xmax) return;
		if (ya < YMIN) ya = YMIN;
		if (tborder[xa] > ya) tborder[xa] = ya;
	}
}

static void PolyLine(work xa, work ya, work xb, work yb)
{
	register work *tb, *lb, *rb, xt, yt, cnt, cur;
	register void (*pp)(work, work);
	work *at, xdelta, ydelta, ysign, yabs, counter, temp;
	prec xstart, ystart, xinc, yinc;

	if (ya > yb) {
		temp = xa; xa = xb; xb = temp;
		temp = ya; ya = yb; yb = temp;
	}
	if (yb < YMIN) return;
	if (ya > ymax) return;
	if (ya < YMIN) {
		xa = xa + SlopeCalc((YMIN - ya), (xb - xa), (yb - ya));
		ya = YMIN;
	}
	if (yb > ymax) {
		xb = xb + SlopeCalc((ymax - yb), (xb - xa), (yb - ya));
		yb = ymax;
	}
	if (xa > xb) {
		temp = xa; xa = xb; xb = temp;
		temp = ya; ya = yb; yb = temp;
	}
	if (xb < XMIN) {
		LftAdjust(ya, yb);
		return;
	}
	if (xa > xmax) {
		RgtAdjust(ya, yb);
		return;
	}
	if (xa < XMIN) {
		temp = ya + SlopeCalc((XMIN - xa), (yb - ya), (xb - xa));
		LftAdjust(ya, temp);
		ya = temp;
		xa = XMIN;
	}
	if (xb > xmax) {
		temp = yb + SlopeCalc((xmax - xb), (yb - ya), (xb - xa));
		RgtAdjust(yb, temp);
		yb = temp;
		xb = xmax;
	}
	/* Points are now within boundaries and xb > xa */

	xdelta = xb - xa;
	ydelta = yb - ya;
	if (ydelta || xdelta) {
		yabs = (ysign = (ydelta < 0))? -ydelta: ydelta;
		if (xdelta > yabs) {
			counter = xdelta;
			ydelta = SpecDiv(ydelta, xdelta);
			xdelta = SIN90;
		}
		else {
			counter = yabs;
			xdelta = SpecDiv(xdelta, yabs);
			ydelta = (ysign)? -SIN90: SIN90;
		}

		PackPrec(xstart, xa, HALFCN);
		PackPrec(ystart, ya, HALFCN);
		xinc = SpecPrec(xdelta);
		yinc = SpecPrec(ydelta);

		tb = tborder;
		lb = lborder;
		rb = rborder;
		pp = plot_pnt;
		cur = current;

		if (color == COLBAK) {
			for (cnt = counter; cnt >= 0; cnt --) {
				xt = Prec2nd(xstart);
				yt = Prec2nd(ystart);
				if (lb[yt] > xt) lb[yt] = xt;
				if (rb[yt] < xt) rb[yt] = xt;
				xstart += xinc;
				ystart += yinc;
			}
		}
		else {
			for (cnt = counter; cnt >= 0; cnt --) {
				xt = Prec2nd(xstart);
				yt = Prec2nd(ystart);
				at = recinfo[yt];
				if ((at[xt] != cur) && (tb[xt] > yt))
					(*pp)(xt, yt);
				if (lb[yt] > xt) lb[yt] = xt;
				if (rb[yt] < xt) rb[yt] = xt;
				xstart += xinc;
				ystart += yinc;
			}
		}
	}
}

static void PolyUpdate(void)
{
	register work *lb = lborder, *rb = rborder, **ri = recinfo,
		cnt, lside, rside;

	for (cnt = map_h; cnt; cnt--, ri++) {
		lside = *lb;
		*lb++ = lftinit;
		rside = *rb;
		*rb++ = rgtinit;

		if (lside <= rside)
			FillArray(rside - lside + 1, *ri + lside, current);
	}
}

static void TopAdjust(register work x1, register work x2)
{
	register work *tb = tborder, temp;

	if (x1 > x2) {
		temp = x1; x1 = x2; x2 = temp;
	}

	FillArray(x2 - x1 + 1, tb + x1, YMIN);
}

static void LftAdjust(register work y1, register work y2)
{
	register work *lb = lborder, temp;

	if (y1 > y2) {
		temp = y1; y1 = y2; y2 = temp;
	}

	FillArray(y2 - y1 + 1, lb + y1, XMIN);
}

static void RgtAdjust(register work y1, register work y2)
{
	register work *rb = rborder, temp;

	if (y1 > y2) {
		temp = y1; y1 = y2; y2 = temp;
	}

	FillArray(y2 - y1 + 1, rb + y1, xmax);
}

/*	Define used in DrawTarget */
#define InBounds(a, b) \
	((a) <= xmax && (a) >= XMIN && (b) <= ymax && (b) >= YMIN)

static void DrawTarget(work x, work y)
{
	register work c = current, t, u, v, *p, **ri = recinfo;
	register void (*pp)(work, work) = plot_pnt,
		(*ep)(work, work) = erase_pnt;

	u = x - t_mrk_m;
	v = y - t_mrk_m;
	for (t = t_mrk_w; t; t--, u++, v++) {
		if (InBounds(u, v)) {
			if (*(p = ri[v] + u) == c) (*ep)(u, v);
			else *p = c;
			(*pp)(u, v);
		}
	}

	u = x - t_mrk_m;
	v = y + t_mrk_m;
	for (t = t_mrk_w; t; t--, u++, v--) {
		if (InBounds(u, v)) {
			if (*(p = ri[v] + u) == c) (*ep)(u, v);
			else *p = c;
			(*pp)(u, v);
		}
	}
}
