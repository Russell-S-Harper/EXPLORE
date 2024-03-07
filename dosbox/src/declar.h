
/* declar.h */

extern void **mempnt;

extern void (*plot_pnt)(work, work), (*erase_pnt)(work, work);

extern char **vlyr, **sndlst, *ldtemp, sndoff[], scanveh[];

extern work
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
	curfield, newfield, vgear, select, playern,
	dxscale, dyscale, xmax, ymax, map_mid_w, map_mid_h,
	lftinit, rgtinit, topinit, btminit, maxreq,
	dxmouse, dymouse, lbutton, rbutton;

extern volatile work vbcounter, quit_prg;

extern unsigned work current, map_w, map_h, t_mrk_w, t_mrk_m;
