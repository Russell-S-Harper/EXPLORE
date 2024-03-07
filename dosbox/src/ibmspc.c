
/*	ibmspc.c */

#include <graphics.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <dos.h>

#include "define.h"
#include "declar.h"

/*	The following must be defined; also check
	define.h for other required modifications */

/*	Routine called by main */
void UpdateDisplay(void);


/*	Math routines called throughout the program */
work SlopeCalc(work, work, work);
work SpecMult(work, work);
work SpecDiv(work, work);


/*	Routines called by InitProgram */
void InitPlot(void);
void InitGraphics(void);
void InitMouse(void);
void InitInterrupt(void);


/*	Keyboard routine called by ProcessVeh */
work GetInput(work *);


/*	Fill routine called throughout the program */
work *FillArray(unsigned, work *, work);


/*	Routines called by ExitProgram */
void ExitInterrupt(void);
void ExitMessage(work);


/******** These routines are accessed by routines in ibmspc.c ********/

/*	Routine used by InitMouse and GetInput */
static void CenterMouse(void);

/*	Program-defined interrupt routines */
static void interrupt ProgramTimer(void);
static int ControlBreak(void);

/*	Mouse interface */
static void CallMouse(void *);

/*	Pixel routines */
static void Color16Plot(work, work);
static void Color16Erase(work, work);
static void Color4Plot(work, work);
static void Color4Erase(work, work);
static void MonoPlot(work, work);
static void MonoErase(work, work);

/*	Compare routine used by InitPlot */
static unsigned CompareBlocks(char *, char *);


/******** 80X86 specific material; modify as req'd ********/

/*	The program insures that the values passed to and returned
	from SlopeCalc, SpecMult and SpecDiv never exceed |32767|.
	Since these three routines are very short and are used a great
	deal, it is well worth the effort to optimize them */

/*	Actually (work)((prec)num1 * num2 / denom) */
work SlopeCalc(work num1, work num2, work denom)
{
	asm	mov	ax, word ptr [bp+4]
	asm	imul	word ptr [bp+6]
	asm	idiv	word ptr [bp+8]
}

/*	Actually (work)(((prec)num1 * num2)>>(DGTSHFT * 3)) */
work SpecMult(work num1, work num2)
{
	asm	mov	ax, word ptr [bp+4]
	asm	imul	word ptr [bp+6]
	asm	shl	ax, 1
	asm	rcl	dx, 1
	asm	shl	ax, 1
	asm	rcl	dx, 1
	asm	shl	ax, 1
	asm	rcl	dx, 1
	asm	shl	ax, 1
	asm	rcl	dx, 1
	asm	mov	ax, dx
}

/*	Actually (work)(((prec)num<<(DGTSHFT * 3)) / denom) */
work SpecDiv(work num, work denom)
{
	asm	mov	dx, word ptr [bp+4]
	asm	mov	ax, 0
	asm	sar	dx, 1
	asm	rcr	ax, 1
	asm	sar	dx, 1
	asm	rcr	ax, 1
	asm	sar	dx, 1
	asm	rcr	ax, 1
	asm	sar	dx, 1
	asm	rcr	ax, 1
	asm	idiv	word ptr [bp+6]
}


/*	Fills an array at <p> with <num> quantity of <data> */
work *FillArray(unsigned num, work *p, work data)
{
	asm	les	di, dword ptr [bp + 6]
	asm	mov	cx, word ptr [bp + 4]
	asm	mov	ax, word ptr [bp + 10]
	asm	cld
	asm	rep	stosw

	return p + num;
}

/*	Accesses the mouse via int 51 */
static void CallMouse(void *regp)
{
	asm	les	di, dword ptr [bp + 4]
	asm	mov	ax, word ptr [di]
	asm	mov	bx, word ptr [di + 2]
	asm	mov	cx, word ptr [di + 4]
	asm	mov	dx, word ptr [di + 6]
	asm	int	51
	asm	mov	word ptr [di], ax
	asm	mov	word ptr [di + 2], bx
	asm	mov	word ptr [di + 4], cx
	asm	mov	word ptr [di + 6], dx
}


/******** IBM specific material; modify as req'd ********/

#define MSSHFT	3	/* log2(8) 8 == resolution of mouse input */

#define SYS_TMR	8	/* 8259 Channel 0 system timer vector */
#define TMP_TMR	103	/* SYS_TMR is put here */

#define OPTIMUM_AREA	128000.0	/* in screen pixels */


static char **map_info, *map_data;

static work
	int_stat, mouse_stat, mousemidx, mousemidy,
	dev_x_ul, dev_y_ul, dev_x_br, dev_y_br;

static unsigned work rec_limit;

static unsigned map_br, map_ul, map_dv;

static struct {unsigned ax, bx, cx, dx;} regs;

static void interrupt (*pc_timer)(void);


/*	Initialize the graphics display to the best available mode */
void InitGraphics(void)
{
	int	driver = DETECT, c_mode, l_mode, t_mode,
		t_color, c_color = 0, t_maxx, t_maxy;

	double	t_area, c_area = 0.0, o_area = log(OPTIMUM_AREA);

	/* Use auto detect to load the device driver */
	initgraph(&driver, &c_mode, "");

	if (graphresult() != grOk) ExitProgram(GIERR);

	/* Get the range of graphics modes */
	getmoderange(driver, &l_mode, &t_mode);

	/* Check each graphics mode to determine the best one */
	for (; t_mode >= l_mode; t_mode--) {

		initgraph(&driver, &t_mode, "");

		/* Find out how many colors are in this mode */
		t_color = getmaxcolor() + 1;

		/* The preferred number of colors is 16, then 4, then 2 */
		if (t_color > c_color && t_color <= NUMCOL)
			c_color = t_color;

		/* If the new mode is suitable then check the screen size */
		if (c_color == t_color) {

			/* Get the resolutions */
			t_maxx = getmaxx() + 1; t_maxy = getmaxy() + 1;

			/* Get the log of the area for a better comparison */
			t_area = log((double)t_maxx * t_maxy);

			/* Use t_mode if the screen size is better */
			if (fabs(t_area - o_area) < fabs(c_area - o_area)) {
				c_mode = t_mode;
				c_area = t_area;
			}
		}
	}

	/* Set the screen to the best mode, and clear the screen */
	initgraph(&driver, &c_mode, "");
	cleardevice();
}


/*	Initialize the plotting variables */
void InitPlot(void)
{
	char **mp, *st, *map_temp;
	int xaspect, yaspect, cnt;
	unsigned dev_w, dev_h, dev_c, map_size, map_bpl;
	double area_adj;

	/* Get the device specifics */
	dev_w = getmaxx() + 1;
	dev_h = getmaxy() + 1;
	dev_c = getmaxcolor() + 1;

	/* Set up the pixel routines */
	switch (dev_c) {
		case 2:
			plot_pnt = MonoPlot;
			erase_pnt = MonoErase;
			break;
		case 4:
			plot_pnt = Color4Plot;
			erase_pnt = Color4Erase;
			break;
		case 16:
			plot_pnt = Color16Plot;
			erase_pnt = Color16Erase;
			break;
		default:
			ExitProgram(DTERR);

	}

	/* Allow room for borders */
	map_w = dev_w - 2; map_h = dev_h - 2;

	/* See if the map area is too large */
	area_adj = sqrt(OPTIMUM_AREA / (double)map_w / (double)map_h);

	/* If it is, adjust map_w and map_h */
	if (area_adj < 1.0) {
		map_w = (work)(area_adj * (double)map_w);
		map_h = (work)(area_adj * (double)map_h);
	}

	/* <map_mid_w, map_mid_h> == the center of the map */
	map_mid_w = map_w/2; map_mid_h = map_h/2;

	/* Set up the screen boundaries */
	dev_x_ul = (dev_w - map_w) / 2;
	dev_y_ul = (dev_h - map_h) / 2;
	dev_x_br = dev_x_ul + map_w - 1;
	dev_y_br = dev_y_ul + map_h - 1;

 	/* Target marker sizes */
	t_mrk_w = (map_w/45 & ~1) + 1; t_mrk_m = t_mrk_w/2;

	/* Indicate the maximum number of allocation requests.
	More than maxreq means the data file is not correct */
	maxreq = map_h + 80;

	/* Set up map boundaries */
	xmax = map_w - 1; ymax = map_h - 1;

	/* Set up scaling factors */
	getaspectratio(&xaspect, &yaspect);
	dxscale = (xmax - XMIN + 1)/2;
	dyscale = -(work)((double)xaspect * dxscale / (double)yaspect + 0.5);

	/* Set the center of the screen in mouse coordinates */
	mousemidx = dev_w << (MSSHFT - 1);
	mousemidy = dev_h << (MSSHFT - 1);

	/* Set up init values */
	lftinit = xmax + 1; rgtinit = XMIN - 1;
	topinit = ymax + 1; btminit = YMIN - 1;

	/* Set up rec_limit */
	rec_limit = (unsigned work)~0 - map_h + 1;

	/* Find out how much memory to reserve */
	map_size = imagesize(dev_x_ul, dev_y_ul, dev_x_br, dev_y_br);

	/* <map_data> is permanent, <map_temp> is a working copy */
	map_data = Alloc(map_size);
	if (!(map_temp = malloc(map_size))) ExitProgram(MAERR);

	/* The screen should be clear, so the image should be blank */
	getimage(dev_x_ul, dev_y_ul, dev_x_br, dev_y_br, map_temp);

	/* Determine the start of the image memory */
	putpixel(dev_x_ul, dev_y_ul, dev_c - 1);
	getimage(dev_x_ul, dev_y_ul, dev_x_br, dev_y_br, map_data);
	map_ul = CompareBlocks(map_data, map_temp);
	putimage(dev_x_ul, dev_y_ul, map_temp, COPY_PUT);

	/* Determine the end of the image memory */
	putpixel(dev_x_br, dev_y_br, 1);
	getimage(dev_x_ul, dev_y_ul, dev_x_br, dev_y_br, map_data);
	map_br = CompareBlocks(map_data, map_temp);
	putimage(dev_x_ul, dev_y_ul, map_temp, COPY_PUT);

	/* Determine how many bytes per line (image-wise) */
	putpixel(dev_x_ul, dev_y_ul + 1, dev_c - 1);
	getimage(dev_x_ul, dev_y_ul, dev_x_br, dev_y_br, map_data);
	map_bpl = CompareBlocks(map_data, map_temp) - map_ul;
	putimage(dev_x_ul, dev_y_ul, map_temp, COPY_PUT);

	free(map_temp);

        /* Set an EGA/VGA constant */
	map_dv = map_bpl >> 2;

	/* Set up map_info */
	map_info = Alloc((size_t)map_h * sizeof(char *));
	for (mp = map_info, st = map_data + map_ul, cnt = map_h;
		cnt; mp++, st += map_bpl, cnt--) *mp = st;

	/* Draw the border on the screen */
	setcolor(dev_c - 1); setwritemode(COPY_PUT);
	rectangle(dev_x_ul - 1, dev_y_ul - 1, dev_x_br + 1, dev_y_br + 1);
}

/*	Compares two blocks of memory. Returns the index where
	the blocks differ. Note that no error checking is done */
static unsigned CompareBlocks(register char *block1, register char *block2)
{
	register unsigned index = 0;

	while (*block1++ == *block2++) index++;

	return index;
}

/*	Pixel routines */

void Color16Plot(work x, work y)
{
	static char plot_data[] =
		{0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

	register char *map = map_info[y] + (x >> 3),
		dat = plot_data[x & 0x07];

	register work col = color;

	if (col & 0x08) *map |= dat;
	map += map_dv;
	if (col & 0x04) *map |= dat;
	map += map_dv;
	if (col & 0x02) *map |= dat;
	map += map_dv;
	if (col & 0x01) *map |= dat;
}

void Color16Erase(work x, work y)
{
	static char erase_data[] =
		{0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};

	register char *map = map_info[y] + (x >> 3),
		dat = erase_data[x & 0x07];

	*map &= dat;
	*(map += map_dv) &= dat;
	*(map += map_dv) &= dat;
	*(map += map_dv) &= dat;
}

void Color4Plot(register work x, register work y)
{
	static char plot_data[] =
		{0x00,0x00,0x00,0x00,0x40,0x10,0x04,0x01,
		0x80,0x20,0x08,0x02,0xc0,0x30,0x0c,0x03,
		0x40,0x10,0x04,0x01,0x80,0x20,0x08,0x02,
		0xc0,0x30,0x0c,0x03,0x40,0x10,0x04,0x01,
		0x80,0x20,0x08,0x02,0xc0,0x30,0x0c,0x03,
		0x40,0x10,0x04,0x01,0x80,0x20,0x08,0x02,
		0xc0,0x30,0x0c,0x03,0x40,0x10,0x04,0x01,
		0x80,0x20,0x08,0x02,0xc0,0x30,0x0c,0x03};

	*(map_info[y] + (x >> 2)) |=
		plot_data[(color << 2) + (x & 0x03)];
}

void Color4Erase(register work x, register work y)
{
	static char erase_data[] = {0x3f,0xcf,0xf3,0xfc};

	*(map_info[y] + (x >> 2)) &= erase_data[x & 0x03];
}

void MonoPlot(register work x, register work y)
{
	static char plot_data[] =
		{0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

	if (color)
		*(map_info[y] + (x >> 3)) |= plot_data[x & 0x07];
}

void MonoErase(register work x, register work y)
{
	static char erase_data[] =
		{0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};


	*(map_info[y] + (x >> 3)) &= erase_data[x & 0x07];
}


/*	Update the display and perform other functions */
void UpdateDisplay(void)
{
	char *map_bgn = map_data + map_ul, *map_end = map_data + map_br;
	static work **clrrec;

	/* Update the screen image */
	putimage(dev_x_ul, dev_y_ul, map_data, COPY_PUT);

	/* Clear the bitmap data */
	if ((ptrdiff_t)map_bgn & 1) *map_bgn++ = 0;
	if (!((ptrdiff_t)map_end & 1)) *map_end-- = 0;
	FillArray((map_end - map_bgn + 1) >> 1, (work *)map_bgn, 0);

	/* Reset tborder */
	FillArray(map_w, tborder, topinit);

	/* Update the current record value and
	clear the map record a line at a time */
	if (++current) {
		if (current >= rec_limit) {
			if (current == rec_limit)
				clrrec = recinfo;
			FillArray(map_w, *clrrec, RECINIT);
			clrrec++;
		}
	}
	else current++;
}


/*	Set up the mouse for the program */
void InitMouse(void)
{
	if (!mouse_stat) {
		/* Check if the mouse is installed */
		regs.ax = 0;
		CallMouse(&regs);
		if (regs.ax) mouse_stat = 1;
	}

	if (mouse_stat) {
		/* Set up the x boundary */
		regs.ax = 7;
		regs.cx = XMIN << MSSHFT;
		regs.dx = getmaxx() << MSSHFT;
		CallMouse(&regs);

		/* Set up the y boundary */
		regs.ax = 8;
		regs.cx = YMIN << MSSHFT;
		regs.dx = getmaxy() << MSSHFT;
		CallMouse(&regs);

		/* Initialize the mouse position */
		CenterMouse();
	}
}


/*	Routine used by InitMouse and GetInput */
static void CenterMouse(void)
{
	regs.ax = 4;
	regs.cx = mousemidx;
	regs.dx = mousemidy;
	CallMouse(&regs);
}


/*	Reconfigure the interrupt */
void InitInterrupt(void)
{
	if (!int_stat) {

		int_stat = 1;

		/* Set the timer vector to the new routine */
		pc_timer = getvect(SYS_TMR);
		setvect(TMP_TMR, pc_timer);
		setvect(SYS_TMR, ProgramTimer);

		/* Set the timer rate to 145.65 Hz */
		outportb(0x43, 0x36);
		outportb(0x40, 0x00);
		outportb(0x40, 0x20);

		/* Initialize the ctrl-break routine */
		ctrlbrk(ControlBreak);
		setcbrk(0);
	}
}


/*	Reset interrupts */
void ExitInterrupt(void)
{
	if (int_stat) {

		/* Reset the timer rate to 18.21 Hz */
		outportb(0x43, 0x36);
		outportb(0x40, 0x00);
		outportb(0x40, 0x00);

		/* Restore the timer vector */
		setvect(SYS_TMR, pc_timer);
	}
}


/*	Interrupt routine. Called at 145.65 Hz */
static void interrupt ProgramTimer(void)
{
	static int
		tmcontrol = 8,		/* corresponds to 18.21 Hz */
        	vbcontrol = 2;		/* corresponds to 72.82 Hz */

	if (!--vbcontrol) {		/* frame counter at 72.82 Hz */
		vbcontrol = 2;
		vbcounter++;
	}

	if (!--tmcontrol) {		/* call original interrupt at 18.21 Hz */
		tmcontrol = 8;
		geninterrupt(TMP_TMR);
	}
	else outportb(0x20, 0x20);	/* indicate that the interrupt is done */
}


/*	Program Control-Break handler */
static int ControlBreak(void)
{
	return quit_prg = 1;
}


/*	ExitMessage indicates any errors */
void ExitMessage(work stat)
{
	closegraph();

	switch (stat) {

		case MAERR:
			fputs("\nThere is not enough memory to run the program.\n", stdout);
			fputs("The program can require up to 450 K.\n", stdout);
			break;
		case FOERR:
			fputs("\nThe data file has been erased or renamed.\n", stdout);
			fputs("Restore the data file from the back-up.\n", stdout);
			break;
		case FCERR:
			fputs("\nThe data file has been corrupted.\n", stdout);
			fputs("Restore the data file from the back-up.\n", stdout);
			break;
		case GIERR:
			fputs("\nThere is a problem involving the graphics initialization.\n", stdout);
			fputs("Make sure the BGI files are located in the current directory.\n", stdout);
			break;
		case DTERR:
			fputs("\nThe display driver doesn't have a supportable graphics mode.\n", stdout);
			break;
	}
}


/*	GetInput processes keyboard and mouse input.
	When translating this routine, note that kbhit returns nonzero
	if a character is ready on the keyboard queue and that getch
	returns the next character on the queue, or waits until one is
	available */

/*	keyboard defines used in GetInput */
#define ESCAP	27	/* escape key: pause program */
#define CURUP	72	/* cursor up: climb faster */
#define CURLF	75	/* cursor left: turn left */
#define CURRT	77	/* cursor right: turn right */
#define CURDN	80	/* cursor down: dive faster */
#define NPAD1	79	/* numeric pad '1': accelerate */
#define NPAD0	82	/* numeric pad '0': deaccelerate */
#define CTRLQ	17	/* <CTRL> Q: to quit */

#define ZISHFT	2	/* improves the climbing response (* 4) */
#define MAXCNTR	20	/* limit for spinning and climbing rate */

work GetInput(work *vp)
{
	work d, q, altstat = 0, dirstat = 0, zt;

	/* Process mouse input */
	if (mouse_stat) {

		/* Read its position */
		regs.ax = 3;
		CallMouse(&regs);

		/* Update the mouse position and button status */
		dxmouse = ((work)regs.cx - mousemidx) >> MSSHFT;
		dymouse = (mousemidy - (work)regs.dx) >> MSSHFT;
		lbutton |= regs.bx & 1;
		rbutton |= regs.bx & 2;

		/* Recenter the mouse */
		CenterMouse();
	}

	/* Process keyboard input */
	while (kbhit()) {

		switch (getch()) {

			case ESCAP:
				d = vbcounter;
				while (getch() == ESCAP);
				vbcounter = d;
				lbutton = rbutton = 0;
				if (mouse_stat) CenterMouse();
				break;

			case CURUP:
				if (altcntr < 0) altcntr = 0;
				else if (altcntr < MAXCNTR) altcntr++;
				altstat = 1;
				break;

			case CURLF:
				if (dircntr > 0) dircntr = 0;
				else if (dircntr > -MAXCNTR) dircntr--;
				dirstat = 1;
				break;

			case CURRT:
				if (dircntr < 0) dircntr = 0;
				else if (dircntr < MAXCNTR) dircntr++;
				dirstat = 1;
				break;

			case CURDN:
				if (altcntr > 0) altcntr = 0;
				else if (altcntr > -MAXCNTR) altcntr--;
				altstat = 1;
				break;

			case NPAD1:	/* accelerate */
				if (vgear > HGEAR) vgear--;
				break;

			case NPAD0:	/* deaccelerate */
				if (vgear < LGEAR) vgear++;
				break;

			case CTRLQ:
				quit_prg = 1;
		}
	}

	vp[VEHICD] += dircntr;
	zt = vp[VEHICZ] + (altcntr << ZISHFT);

	if (!altstat) {
		if (q = ((d = altcntr) < 0)) d = -d;
		d >>= 1;
		altcntr = (q)? -d: d;
	}

	if (!dirstat) {
		if (q = ((d = dircntr) < 0)) d = -d;
		d >>= 1;
		dircntr = (q)? -d: d;
	}

	return zt;
}
