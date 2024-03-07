
/* fldrtn.c */

#include <stdlib.h>
#include "define.h"
#include "declar.h"

/******** routines; no modifications req'd ********/

void ProcessVeh(void);	/* Called by main */

/*	The following routines are called only by ProcessVeh or by
	routines called only by ProcessVeh unless indicated otherwise */

void SetField(work);	/* Called throughout the program */
static work NextPlyr(work);
static work OCDoorRtn(work *, work);

static void FldTravel(work, work, work, work, work);
static work Rfielddata(work, work, work);
static void HeightRtn(work *, work);
static void InfVehic(work, work);

static void SetVehic(work);
static void ClrVehic(work);
static void Svehicdata(work);
static void Cvehicdata(work);
static void TransVeh(work *, work *);
static work NextFVeh(void);

static void AddSound(work);
static void RepSound(work);

/*	ScanField and CheckFld are not called by ProcessVeh */

void ScanField(void);			/* Called by main */
static void CheckFld(work, work);	/* Called only by ScanField */


/*	Defines used in fldrtn.c */

#define VEHBIT	~WRKABS		/* Highest bit on == vehicle present */ 

#define FYMSK	~(FLDXDM-1)	/* 'and' mask to isolate FLDYDM */
#define LOBYTE 0x00ff		/* 'and' mask to isolate low byte */
#define HIBYTE \
	(LOBYTE << 2*DGTSHFT)	/* 'and' mask to isolate high byte */

#define InField(a, b) \
	((unsigned work)(a) <= MAXFX && (unsigned work)(b) <= MAXFY)

/*	Defines used in ProcessVeh or in routines called by ProcessVeh */

#define DAMPEN	4	/* To dampen mouse movement */

/*	Collision 'or' masks. VHCL is combined with VSTP or VSBT to give
	more information about the collision */
#define VSTP	0x01	/* Scraping top */
#define VSBT	0x02	/*    "     bottom */
#define VSLS	0x04	/*    "     left side */
#define VSRS	0x08	/*    "     right side */
#define VMCL	0x10	/* Collision when moving forward */
#define VHCL	0x20	/*     "      "   changing height */
#define VCQT	0x40	/* Close quarters */
#define VTQT	0x80	/* Tight quarters */

/*	Process every active vehicle; the player-controlled one is done first */
void ProcessVeh(void)
{
	register work cnt, *u, *v, *w;
	work tmp;

	/* lbutton != 0 means the targets should be updated */
	if (lbutton) {
		lbutton = 0;
		if (select == NOSEL || select == NXSEL) {
			for (cnt = MACTVEH, u = vehic_ + VEHICT; cnt; cnt--, u++) {
				if (*u != NOSEL) *u = DOSEL;
			}
		}
		else vehic_[select + VEHICT] = DESEL;
		select = NXSEL;
	}

	/* rbutton != 0 means the player-type vehicles should be cycled */
	if (rbutton) {
		rbutton = 0;

		/* Get the next player-type vehicle */
		playern = NextPlyr(playern + 1);

		/* Make the vehicle motionless */
		vgear = LGEAR;
		altcntr = dircntr = 0;

		/* Get the new vehicle's field */
		newfield = vehic_[playern + VEHICF];

		/* Provisional sound gimmick */
		RepSound(NCHAN);

		/* Change the field, if required. NOPSPC == no special treatment */
		if (newfield != curfield) SetField(NOPSPC);
	}

	/* Set the player's vehicle pointer */
	w = vehic_ + playern;

	/* <tmp> is the potential new z coordinate */
	tmp = GetInput(w);

	/* Move on the field */
	FldTravel(playern, tmp, vgear, dxmouse/DAMPEN, dymouse/DAMPEN);

	/* In the future, collision or scraping related sounds go here */

	/* Slow down a gear if VMCL is set (motion collision) */
	if ((w[VEHICC] & VMCL) && (vgear < LGEAR-1)) vgear++;

	/* Clear the collision indicator */
	w[VEHICC] = VCLR;

	/* Change the field, if required. NEWFLD == special treatment */
	if (newfield != curfield) {
		SetField(NEWFLD);
		w[VEHICF] = curfield;
	}

	/* Keep a record of the player's position */
	v = fieldc_ + w[VEHICF];
	playerx = v[RECFX] = w[VEHICX];
	playery = v[RECFY] = w[VEHICY];
	playerz = v[RECFZ] = w[VEHICZ];
	playerd = w[VEHICD];

	/* Now check all the other players */
	for (cnt = 0, v = vehic_; cnt < MACTVEH; cnt++, v++) {

		/* Process active vehicles */
		if (v[VEHICA] != VCLR) {

			/* VEHICP is the vehicle's procedure */
			switch (v[VEHICP]) {

				case PLYRIN: /* Player input routine */
				case ROTVEH: /* Rotation routine */
					v[VEHICD] += v[VEHICE];
					break;

				/* DOOR[PF]D are two door routines.
				In the future, the mask may be more
				complicated in that more than one vehicle
				may be masked. Also, whether the vehicles
				specified by the mask are in the same field
				as the door will be checked before calling
				OCDoorRtn */

				case DOORPD: /* Door proc (poly data is changed) */
					poly_[(*fieldloc[cnt] & ~VEHBIT) + PDEF] =
						/* w[VEHICS] is the player's mask */
						OCDoorRtn(v, w[VEHICS]);
					break;

				case DOORFD: /* Door proc (field data is changed) */
					u = fieldloc[cnt];
					*u = (*u & VEHBIT) + OCDoorRtn(v, w[VEHICS]);
					break;

				case GTPLYR: /* Provisional chase player routine */
					/* Note: use VEHICE to remember last move */
					tmp = v[VEHICD] + v[VEHICE];
					v[VEHICD] = 0;
					FldTravel(cnt, w[VEHICZ], LGEAR,
						(w[VEHICX] - v[VEHICX])/(DAMPEN*4),
						(w[VEHICY] - v[VEHICY])/(DAMPEN*4));
					v[VEHICD] = tmp;
			}
		}
	}
}

/*	Change the field to <newfield> from <curfield>.
	<pstat> indicates the required treatment */
void SetField(work pstat)
{
	register work q, t, *p, *v = vehic_ + playern;

	/* Remove any current field dependant vehicles */
	if (curfield >= LMFLD) {
		p = remveh[curfield];
		for (t = MACTVEH; t; t--) {
			/* LMEOR means the list has ended */
			if ((q = *p++) < LMEOR) break;
			else ClrVehic(q);
		}
	}

	/* Get the new field */
	fieldpnt = fieldlst[curfield = newfield];
	p = addveh[curfield];

	/* Add any new field dependant vehicles */
	for (t = MACTVEH; t; t--) {
		if ((q = *p++) < LMEOR) break;
		else SetVehic(q);
	}

	/* pstat != NOPSPC means that special treatment is required */
	if (pstat != NOPSPC) {

		/* Modify the player's coordinates */
		p = fieldc_ + curfield;
		v[VEHICX] = p[FLDSX] + (v[VEHICX] & LOBYTE);
		v[VEHICY] = p[FLDSY] + (v[VEHICY] & LOBYTE);
		v[VEHICZ] = p[FLDSZ] + (v[VEHICZ] & LOBYTE);

		/* In the future, INFVEH will deal with any sounds */
		AddSound(NCHAN);
		AddSound(NCHAN+1);
		AddSound(NCHAN+2);
	}
}

/*	Find the next available player-type vehicle from <n> onward */
static work NextPlyr(register work n)
{
	register work *v;

	/* Starting at n, go up to MACTVEH - 1 */
	for (v = vehic_ + n; n < MACTVEH; v++, n++)
		/* Return the vehicle number of the 1st player-type active vehicle */
		if (v[VEHICP] == PLYRIN && v[VEHICA] != VCLR)
			return n;

	/* There weren't any from <n> onward, so start again from 0 */
	return NextPlyr(0);
}

/*	Door constants used in OCDoorRtn */
#define CLOSED	VEHINIT
#define NOTCLS	(VEHINIT+1)	/* Closing, but not fully closed */
#define OPENED	(VEHINIT+2)
#define NOTOPN	(VEHINIT+3)	/* Opening, but not fully opened */

/*	Generic door opening/closing routine. <v> door pointer <msk>
	vehicle mask, usually specifies the player only, but can also
	specify multiple vehicles */
static work OCDoorRtn(register work *v, work msk)
{
	/* u points to the specific door data */
	register work *u = vehic_ + v[VEHICE], sound = NOACT;

	/* Check if any important vehicles are on the door step */
	if (v[VEHICL] & msk) {

		/* There are, so start opening the door */
		if (u[VEHCNT] < u[VEHINC]) {
			v[VEHICH] = NOTOPN;

			/* Specify the sound effect */
			sound = u[OPNSND] + ++u[VEHCNT];
		}
		/* Door is open */
		else v[VEHICH] = OPENED;
	}

	/* Otherwise, there aren't any on the door step */
	else {
		/* Start closing the door */
		if (u[VEHCNT]) {
			v[VEHICH] = NOTCLS;

			/* Specify the sound effect */
			sound = u[CLSSND] + --u[VEHCNT];
		}
		/* Door is closed */
		else v[VEHICH] = CLOSED;
	}

	/* Check if the door is in the current field */
	if (v[VEHICF] != curfield) {

		/* It's not, so specify that the sound be turned off */
		sound = u[CUTSND];

		/* If the door isn't being used anymore, it can be removed
		from the field. Note that any 'stranded' vehicles will have
		to find their own ways out */
		if (v[VEHICH] == CLOSED) ClrVehic(v[VEHICO]);
	}

	/* Do the sound effect, if specified */
	if (sound >= LMACT) RepSound(sound);

	/* Clear the door's location data */
	v[VEHICL] = VEHINIT;

	/* Return any modified door data, whether poly- or field-based */
	return u[DOORDT] + u[VEHCNT];
}

/*	Defines used in FldTravel */
#define DTRVL	FCIRC/32	/* Tolerance for 'scraping' */
#define VRSHFT	1		/* Velocity reducer upon collision (/2) */

/*	Move the vehicle <id> on the current field. <zt> z coordinate
	<gear> velocity <f[xy]inc> 'shuffle' */
static void FldTravel(work id, register work zt, work gear, work fxinc, work fyinc)
{
	register work *vp, *vq, *p, xt, yt, d;
	work q;

	/* Remove the vehicle from the field */
	Cvehicdata(id);

	vp = vehic_ + id;

	/* Check if the vehicle intends to move */
	if (gear < LGEAR) {

		/* It does, so set (xt, yt) to the desired positions */
 		xt = vp[VEHICX] + (Sin(vp[VEHICD]) >> gear);
		yt = vp[VEHICY] + (Cos(vp[VEHICD]) >> gear);

		/* Keep (xt, yt) in the field */
		if (xt < 0) xt = 0;
		else if (xt > MAXFX) xt = MAXFX;
		if (yt < 0) yt = 0;
		else if (yt > MAXFY) yt = MAXFY;

		/* Get the block the vehicle is on */
		p = poly_ + Rfielddata(vp[VEHICF], xt, yt);

		/* Get high and low limits */
		d = p[PTOP];
		q = p[PBTM];

		/* Try to move unaltered */
		if (q < zt && d > zt) {
			vp[VEHICX] = xt;
			vp[VEHICY] = yt;
			vp[VEHICZ] = zt;
		}
		/* Try staying up */
		else if (q < vp[VEHICZ] && q > zt) {
			vp[VEHICX] = xt;
			vp[VEHICY] = yt;
			vp[VEHICZ] = q + 1;
			vp[VEHICC] |= VSBT;	/* Scraping bottom */
		}
		/* Try staying down */
		else if (d > vp[VEHICZ] && d < zt) {
			vp[VEHICX] = xt;
			vp[VEHICY] = yt;
			vp[VEHICZ] = d - 1;
			vp[VEHICC] |= VSTP;	/* Scraping top */
		}
		else {
			/* Otherwise, try a bit to the right */			xt = vp[VEHICX] + (Sin(vp[VEHICD] + DTRVL) >> gear);
			yt = vp[VEHICY] + (Cos(vp[VEHICD] + DTRVL) >> gear);

			if (xt < 0) xt = 0;
			else if (xt > MAXFX) xt = MAXFX;
			if (yt < 0) yt = 0;
			else if (yt > MAXFY) yt = MAXFY;

			p = poly_ + Rfielddata(vp[VEHICF], xt, yt);

			if (p[PBTM] < zt && p[PTOP] > zt) {
				vp[VEHICX] = xt;
				vp[VEHICY] = yt;
				vp[VEHICZ] = zt;
				vp[VEHICC] |= VSLS;	/* Scraping left side */
			}

			/* Otherwise, try a bit to the left */
			else {
				xt = vp[VEHICX] + (Sin(vp[VEHICD] - DTRVL) >> gear);
				yt = vp[VEHICY] + (Cos(vp[VEHICD] - DTRVL) >> gear);

				if (xt < 0) xt = 0;
				else if (xt > MAXFX) xt = MAXFX;
				if (yt < 0) yt = 0;
				else if (yt > MAXFY) yt = MAXFY;

				p = poly_ + Rfielddata(vp[VEHICF], xt, yt);

				if (p[PBTM] < zt && p[PTOP] > zt) {
					vp[VEHICX] = xt;
					vp[VEHICY] = yt;
					vp[VEHICZ] = zt;
					vp[VEHICC] |= VSRS;	/* Scraping right side */
				}

				/* Otherwise, nothing works; just process the height request */
				else {
					vp[VEHICC] |= VMCL;	/* Motion collision */
					HeightRtn(vp, zt);
				}
			}
		}
	}

	/* No motion requested; just process the height request */
	else HeightRtn(vp, zt);

	/* Process the shuffle request */
	if (fxinc || fyinc) {
		/* Set (xt, yt) to the desired increment */
		xt = SpecMult(Cos(vp[VEHICD]), fxinc) +
			SpecMult(Sin(vp[VEHICD]), fyinc);

		yt = SpecMult(Cos(vp[VEHICD]), fyinc) -
			SpecMult(Sin(vp[VEHICD]), fxinc);

		p = poly_ + Rfielddata(vp[VEHICF], vp[VEHICX] + xt,
			vp[VEHICY] + yt);

		/* Try to move unaltered */
		if (p[PBTM] < vp[VEHICZ] && p[PTOP] > vp[VEHICZ]) {
			vp[VEHICX] += xt;
			vp[VEHICY] += yt;
		}
		else {
			/* Cut it down by VRSHFT */
			xt >>= VRSHFT;
			yt >>= VRSHFT;
			p = poly_ + Rfielddata(vp[VEHICF], vp[VEHICX] + xt,
				vp[VEHICY] + yt);

			if (p[PBTM] < vp[VEHICZ] && p[PTOP] > vp[VEHICZ]) {
				vp[VEHICX] += xt;
				vp[VEHICY] += yt;
				vp[VEHICC] |= VCQT; /* Close quarters */
			}

			/* Even cutting it by VRSHFT doesn't work */
			else vp[VEHICC] |= VTQT; /* Tight quarters */
		}

		/* Keep the vehicle in bounds */
		if (vp[VEHICX] < 0) vp[VEHICX] = 0;
		else if (vp[VEHICX] > MAXFX) vp[VEHICX] = MAXFX;
		if (vp[VEHICY] < 0) vp[VEHICY] = 0;
		else if (vp[VEHICY] > MAXFY) vp[VEHICY] = MAXFY;
	}

	/* Get the block the vehicle is located on */
	p = poly_ + Rfielddata(vp[VEHICF], vp[VEHICX], vp[VEHICY]);

	/* If the block is a special block, and the vehicle is in the
	active region of the block, then do the block's special procedure */
	if (p[PSPC] && p[PLML] < vp[VEHICZ] && p[PLMH] > vp[VEHICZ]) {

		switch (p[PSPC]) {

			case NEWFLD: /* Change the field of the vehicle */

				/* The calling procedure will handle playern */
				if (playern == id) newfield = p[PDAT];

				/* Move anything else to the new field without any special effects */
				else {
					vq = fieldc_ + (vp[VEHICF] = p[PDAT]);
					vp[VEHICX] = vq[FLDSX] + (vp[VEHICX] & LOBYTE);
					vp[VEHICY] = vq[FLDSY] + (vp[VEHICY] & LOBYTE);
					vp[VEHICZ] = vq[FLDSZ] + (vp[VEHICZ] & LOBYTE);
				}
				break;

			case NEWVEH:
				/* Activate some new vehicles. The
				high and low bytes of p[PDAT] can each
				specify a vehicle */
				if (p[PDAT] & LOBYTE)
					SetVehic(p[PDAT] & LOBYTE);
				if (p[PDAT] & HIBYTE)
					SetVehic((p[PDAT] >> 2*DGTSHFT) & LOBYTE);
				break;

			case INFVEH:
				/* Inform some vehicles that a block
				of concern is being stepped on. The
				high and low bytes of p[PDAT] can each
				specify a vehicle. VEHICS is the
				vehicle's unique mask */
				if (p[PDAT] & LOBYTE)
					InfVehic(vp[VEHICS], p[PDAT] & LOBYTE);
				if (p[PDAT] & HIBYTE)
					InfVehic(vp[VEHICS],
						(p[PDAT] >> 2*DGTSHFT) & LOBYTE);
		}
	}

	/* Put the vehicle back on its field */
	Svehicdata(id);
}

/*	Return the field <fd> data at (xt, yt) */
static work Rfielddata(work fd, work xt, work yt)
{
	register work t;

	/* If the vehicle is in bounds and on
	a non-zero block, then return its data */
	if (InField(xt, yt) &&
		(t = *(fieldlst[fd] + ((yt >> DGTSHFT) & FYMSK) +
		(xt >> 2*DGTSHFT)) & ~VEHBIT)) return t;

	/* Otherwise return some generic data */
	else return fieldc_[fd + FLDTP];
}

/*	Process the height request <zt> of a vehicle <vp>. This routine is
	called when the vehicle isn't moving for some reason */
static void HeightRtn(register work *vp, work zt)
{
	register work *p;

	/* Get the block the vehicle sits on */
	p = poly_ + Rfielddata(vp[VEHICF], vp[VEHICX], vp[VEHICY]);

	/* Bottom constraints? */
	if (p[PBTM] > zt) {
		vp[VEHICZ] = p[PBTM] + 1;
		vp[VEHICC] |= (VHCL | VSBT); /* Collision underneath */
	}

	/* Top constraints? */
	else if (p[PTOP] < zt) {
		vp[VEHICZ] = p[PTOP] - 1;
		vp[VEHICC] |= (VHCL | VSTP); /* Collision overhead */
	}

	/* Otherwise, no constraints */
	else vp[VEHICZ] = zt;
}

/*	Inform the vehicle <d> that a vehicle defined
	by the mask <s> is on a block of concern */
static void InfVehic(work s, work d)
{
	register work *vq = vehic_ + d;

	/* Make sure <d> is active */
	if (vq[VEHICA] >= LMACT) {

		/* Get the actual pointer to <d> */
		vq = vehic_ + vq[VEHICA];

		/* Set the location status */
		vq[VEHICL] |= s;
	}
}

/*	Activate the vehicle <sd> */
static void SetVehic(work sd)
{
	register work id, *vp, *vq = vehic_ + sd;

	/* Check that it's inactive */
	if (vq[VEHICA] < LMACT) {

		/* Get the first free active position, if possible */
		if ((id = NextFVeh()) < MACTVEH) {

			/* Set the pointer */
			vp = vehic_ + id;

			/* Transfer the data to the active position */
			TransVeh(vp, vq);
			vp[VEHICT] = vq[VEHICT];

			vp[VEHICF] = curfield;	/* set to the current field */
			vp[VEHICA] = VSET;	/* the position is active */
			vp[VEHICC] = VCLR;	/* clear the collision status */
			vp[VEHICO] = sd;	/* index to its original position */ 

			vq[VEHICA] = id;	/* index to its active position */

			/* Put the vehicle on its field */
			Svehicdata(id);
		}
	}
}

/*	Deactivate the vehicle <sd> */
static void ClrVehic(work sd)
{
	register work id, *vp, *vq = vehic_ + sd;

	/* Make sure it's active */
	if ((id = vq[VEHICA]) >= LMACT) {

		/* Remove it from the field */
		Cvehicdata(id);

		/* Get the pointer to the active position */
		vp = vehic_ + id;

		/* Transfer the data to the inactive position */
		TransVeh(vq, vp);
		vq[VEHICA] = VIDL;	/* Indicate the vehicle isn't active */

		vp[VEHICA] = VCLR;	/* Free the active position */
		vp[VEHICT] = NOSEL;
	}
}

/*	Put the vehicle <d> on its field */
static void Svehicdata(work d)
{
	register work *p = vehic_ + d, i;

	if (InField(p[VEHICX], p[VEHICY])) {
		p[VEHICQ] = i = ((p[VEHICY] >> DGTSHFT) & FYMSK) +
			(p[VEHICX] >> 2*DGTSHFT);
		*(fieldloc[d] = fieldlst[p[VEHICF]] + i) |= VEHBIT;
		*(fieldveh + i) |= p[VEHICS];
	}
}

/*	Remove the vehicle <d> from its field */
static void Cvehicdata(work d)
{
	register work *p = vehic_ + d, i;

	if (InField(p[VEHICX], p[VEHICY])) {
		if (!(*(fieldveh + p[VEHICQ]) &= p[VEHICU]))
			*fieldloc[d] &= ~VEHBIT;
	}
}

/*	Transfer the vehicle <src> data to the vehicle <dst> */
static void TransVeh(register work *dst, register work *src)
{

	dst[VEHICX] = src[VEHICX];	/* (x, y, z) */
	dst[VEHICY] = src[VEHICY];
	dst[VEHICZ] = src[VEHICZ];
	dst[VEHICD] = src[VEHICD];	/* direction */
	dst[VEHICI] = src[VEHICI];	/* physical identity */
	dst[VEHICP] = src[VEHICP];	/* procedure it follows */
	dst[VEHICE] = src[VEHICE];	/* any extra data */
	dst[VEHICL] = src[VEHICL];	/* its location status */
	dst[VEHICH] = src[VEHICH];	/* its 'health' */
}

/*	Find the first free active vehicle position */
static work NextFVeh(void)
{
	register work t, *p;

	for (p = vehic_ + VEHICA, t = 0; t < MACTVEH && *p != VCLR; p++, t++);
	return t;
}

/*	Add a sound <n> to the active region */
static void AddSound(work n)
{
	register work i, m, r, t, *p = sndrnk;

	/* Check if it's already there and exit if it is */
	m = r = p[n];
	for (t = 0; t < NCHAN; t++, p++) {
		if (*p == r) return;
		if (*p < m) {
			m = *p;
			i = t;
		}
	}

	/* Otherwise, replace the lowest ranked sound (if possible) */
	if (m != r) {
		sndlst[i] = sndlst[n];
		sndrnk[i] = r;
	}
}

/*	Replace a sound, preferably with itself <n> */
static void RepSound(work n)
{
	register work i, m, r, t, *p = sndrnk;

	/* Check if it's already there and directly replace if it is */
	m = r = p[n];
	for (t = 0; t < NCHAN; t++, p++) {
		if (*p == r) {
			sndlst[t] = sndlst[n];
			return;
		}
		if (*p < m) {
			m = *p;
			i = t;
		}
	}

	/* Otherwise, replace the lowest ranked sound (if possible) */
	if (m != r) {
		sndlst[i] = sndlst[n];
		sndrnk[i] = r;
	}
}

/*	Defines used in ScanField */

#define SCNUM	(work)771		/* Numeric constants */
#define SCDENOM	(work)17446
	/* SCNUM/SCDENOM = 1/((1 << DGTSHFT) * SQRT(2)) */

#define FHALF	(FPREC/2)
#define FFLAG	1

/*	Scan the current field */
void ScanField(void)
{
	register work va, vb, vc, vd;
	work t, x1, y1, x2, y2, fx, fy, dscan, cnt, ds;

	x1 = SlopeCalc(Sin(va = playerd + FCIRC/4), SCNUM, SCDENOM);
	y1 = SlopeCalc(Cos(va), SCNUM, SCDENOM);
	x2 = SlopeCalc(va = Sin(playerd), SCNUM, SCDENOM);
	y2 = SlopeCalc(vb = Cos(playerd), SCNUM, SCDENOM);
	fx = playerx - ((va + va/2)>>DGTSHFT);
	fy = playery - ((vb + vb/2)>>DGTSHFT);

	FillArray(FLDLEN, fieldflg, FFLAG);

	numpoly = dscan = 0;

	CheckFld(vc = fx, vd = fy);

	for (cnt = MAXFL; cnt >= 0; cnt--) {
		CheckFld(fx = va = vc += x2, fy = vb = vd += y2);

		for (ds = dscan; ds >= 0; ds--) {
			CheckFld(vc += x1, vd += y1);
			CheckFld(va -= x1, vb -= y1);
		}
		dscan++;
		vc = fx;
		vd = fy;
	}
}

/*	Check the current field at (xt, yt) and note any inhabitants */
static void CheckFld(register work xt, register work yt)
{
	register work *q, *s, t;

	if (InField(xt ,yt)) {
		s = field_ +
			(t = (xt >> (2*DGTSHFT)) +
			((yt & HIBYTE) >> (2*DGTSHFT-FLDSHFT)));

		if (s[FIELDFLG]) {
			s[FIELDFLG] = 0;
			if (t = fieldpnt[t]) {
				q = scan_ + numpoly++;
				if ((q[POLY] = t) < LMVEH) q[VEHIC] = s[FIELDVEH];
				q[SCANX] = (xt & HIBYTE) + FHALF;
				q[SCANY] = (yt & HIBYTE) + FHALF;
			}
		}
	}
}
