/*
    Contents: "cx16 specific" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

/* cx16-specific.c */

#include <tgi.h>
#include <joystick.h>
#include <conio.h>
#include <time.h>
#include <limits.h>
#include <ctype.h>
#include "explore.h"
#include "vera.h"

/* Ideal frames/s - adjust as required to prevent "jerking".
   Limited by the resolution of CLOCKS_PER_SEC.

	FRAMES_PER_SEC | Actual frames/s
	------------- -+----------------
	1 - 6          | 1 - 6
	7              | 7.500
	8              | 8.571
	9, 10          | 10
	11, 12         | 12
	13 - 15        | 15
	16 - 20        | 20
	21 - 30        | 30
	31 - 60        | 60
*/
#define FRAMES_PER_SEC_LO	4	/* If HW VERA FX Line Helper 4bpp workaround is required */
#define FRAMES_PER_SEC_HI	8	/* No restrictions */

/* Keyboard defines used in GetPlayerInput */
#define JOIN_AS_NORMAL		74	/* J: join game */
#define FOCUS_ON_HUMAN		75	/* K: focus on human */
#define NO_REFRESH_AT_LAST_LVL	76	/* L: last level doesn't refresh */

#define PAUSE_PROGRAM		27	/* escape: pause/unpause program */
#define REQ_CLIMB_OR_GEAR_FWD	145	/* cursor up: climb or gear forward */
#define REQ_DIVE_OR_GEAR_REV	17	/* cursor down: dive or gear reverse */
#define TURN_RIGHT		29	/* cursor right: turn right */
#define TURN_LEFT		157	/* cursor left: turn left */
#define FIRE_MISSILE		32	/* space: fire missile */
#define CYCLE_PLAYER		67	/* C: cycle player */
#define QUIT_PROGRAM		81	/* Q: quit program */

/* To prevent cheating (e.g. by using the keyboard and joystick together)! */
#define Z_DELTA_LMT		2
#define A_DELTA_LMT		2

/* Hex digit defines */
#define HEX_DGT_HI		0xF0
#define HEX_DGT_LO		0x0F
#define HEX_DGT_SHIFT		4
#define CLEAR_BYTE		((CLR16_BLACK << HEX_DGT_SHIFT) | CLR16_BLACK)

/* Rendering shift defines */
#define VERA_SLOPE_SHIFT	9
#define WORKAROUND_SHIFT	3

/* Incrementer */
#define VEHICLE_IDX_INC		5

/* To check if we're running in the emulator */
#define EMULATOR_LO	(*(char *)(0x9FBE))
#define EMULATOR_HI	(*(char *)(0x9FBF))

/* To control screen recording in the emulator:

	FRAME_CMD = 0	- disable
	FRAME_CMD = 1	- save a single frame
	FRAME_CMD = 2	- continuous
*/
#define FRAME_CMD	(*(char *)(0x9FB5))

/* Callback routine hint */
enum {FRAME_TO_FINISH, SCREEN_TO_FINISH};

/* Messages */
#define MSG_LINES	7
#define MSG_WINDOW	(MSG_LINES - 2)
#define MSG_DURATION	20		/* Or MSG_DURATION / FRAMES_PER_SEC seconds */
#define	MSG_CYCLE	45		/* Or MSG_CYCLE * MSG_DURATION / FRAMES_PER_SEC seconds */

static void DrawLine16(void);
static void ResetMessages(void);
static void DefaultCallback(uint8_t waiting);

static bool
	f_use_workaround;

static uint8_t
	f_current_color,
	f_message_cd,
	f_message_index;

static int16_t
	f_x_from_point,
	f_y_from_point,
	f_x_to_point,
	f_y_to_point;

/* Initialize the hardware, etc. */
void InitSpecific(void)
{
	/* Set up */
	tgi_install(tgi_static_stddrv);
	if (tgi_geterror() != TGI_ERR_OK)
		ExitProgram(ERR_GI);

	/* Initialize */
	tgi_init();
	tgi_clear();

	/* Save the resolution */
	g_display_width = VERA_HORZ_RES;
	g_display_height = VERA_VERT_RES;

	/* Set the bitmap to 16-color mode */
	VERA_L0_CONFIG = VERA_16_COLOR_MODE;

	/* Set the offset */
	VERA_L0_TILEBASE = VERA_SCR_2_BASE;

	/* Use HW VERA FX Line Helper 4bpp workaround? */
	f_use_workaround = (EMULATOR_LO != '1' || EMULATOR_HI != '6');

	/* Clear the remainder of screen 2 */
	UpdateDisplay();

	/* Cursor off */
	cursor(0);

	/* Set up the joystick */
	joy_install(joy_static_stddrv);

	/* Set up current messages */
	g_current_messages = g_attract_messages;
}

/* Switch to the other screen, clear the previous screen, and perform other functions */
void UpdateDisplay(void)
{
	static clock_t s_frame_clock, s_clocks_per_frame;
	static bool s_initialized;
	clock_t current_clock;
	uint8_t base, address, i, j;

	/* Wait for the end of the frame based on clock cycles */
	if (!s_initialized) {
		s_frame_clock = clock();
		s_clocks_per_frame = CLOCKS_PER_SEC / (f_use_workaround? FRAMES_PER_SEC_LO: FRAMES_PER_SEC_HI);
		s_initialized = true;
	} else {
		do {
			/* While waiting for the end of the frame, do some work */
			DefaultCallback(FRAME_TO_FINISH);
			/* Check if the frame is done */
			current_clock = clock();
			/* Check for rollover - not likely to happen but JIC! */
			if (current_clock < s_frame_clock)
				break;
		}
		while (current_clock - s_frame_clock < s_clocks_per_frame);
		s_frame_clock = current_clock;
	}

	/* Wait for the end of the current screen based on scan lines */
	do
		/* While waiting for the end of the current screen, do some work */
		DefaultCallback(SCREEN_TO_FINISH);
	while (CURRENT_SCAN_LINE < MAXIMUM_SCAN_LINE);

	/* Set up the next screen */
	switch (VERA_L0_TILEBASE) {
		case VERA_SCR_1_BASE:
			base = VERA_SCR_2_BASE;
			address = VERA_SCR_1_ADDR;
			break;
		case VERA_SCR_2_BASE:
			base = VERA_SCR_1_BASE;
			address = VERA_SCR_2_ADDR;
			break;
		default:
			ExitProgram(ERR_DC);
	}

	/* Switch to the other screen */
	VERA_L0_TILEBASE = base;

	/* Set the address to start clearing */
	VERA_ADDRx_L = 0x00;
	VERA_ADDRx_M = address;
	VERA_ADDRx_H = VERA_INC_1;

	/* Clear the previous screen */
	for (i = 0; i < VERA_VERT_RES; ++i) {
		for (j = 0; j < VERA_HORZ_RES / VERA_PX_PER_BYTE / 8; ++j)
			/* The optimizer handles this perfectly! */
			VERA_DATA0 =
			VERA_DATA0 =
			VERA_DATA0 =
			VERA_DATA0 =
			VERA_DATA0 =
			VERA_DATA0 =
			VERA_DATA0 =
			VERA_DATA0 = CLEAR_BYTE;
	}
}

/* To provide a little bit of feedback when cycling the player */
#define ID_SCR_CD	6
#define	ID_SCR_X	21
#define	ID_SCR_Y	21

/* Processes keyboard and joystick input */
void GetPlayerInput(VEHICLE *player)
{
	static uint8_t s_identifier_cd;

	bool active_human = !player->npc && player->active;
	uint8_t c, i, j, joy, identifier;

	if (s_identifier_cd && !--s_identifier_cd)
		cputcxy(ID_SCR_X, ID_SCR_Y, ' ');

	/* Process keyboard input */
	while (kbhit()) {
		c = (uint8_t)cgetc();
		switch (c) {
			case PAUSE_PROGRAM:
				StopSounds();
				/* Wait until it's pressed again */
				while (!kbhit() || cgetc() != PAUSE_PROGRAM);
				break;

			case CYCLE_PLAYER:
				/* Find the next active player or NPC, if any */
				for (i = 1, j = g_vehicle_index + 1; i < PLAYER_COUNT; ++i, ++j) {
					if (j >= PLAYER_LIMIT)
						j = PLAYER_INDEX;
					if (g_vehicles[j].active) {
						g_vehicle_index = j;
						ReportToAI(g_vehicles + j, EVT_SWITCHED_FOCUS, (int16_t)&identifier);
						textcolor(CLR16_WHITE);
						cputcxy(ID_SCR_X, ID_SCR_Y, identifier);
						s_identifier_cd = ID_SCR_CD;
						break;
					}
				}
				break;

			case JOIN_AS_NORMAL:
				ReportToAI(player, EVT_HUMAN_JOINED, MD_JOIN_AS_NORMAL);
				ResetMessages();
				break;

			case FOCUS_ON_HUMAN:
				ReportToAI(player, EVT_HUMAN_JOINED, MD_FOCUS_ON_HUMAN);
				ResetMessages();
				break;

			case NO_REFRESH_AT_LAST_LVL:
				ReportToAI(player, EVT_HUMAN_JOINED, MD_NO_REFRESH_AT_LAST_LEVEL);
				ResetMessages();
				break;

			case QUIT_PROGRAM:
				tgi_done();
				g_exit_program = true;
				break;
		}
		if (active_human) {
			switch (c) {
				case REQ_CLIMB_OR_GEAR_FWD:
					if (player->airborne)
						++player->z_delta;
					else
						++player->gear;
					break;

				case REQ_DIVE_OR_GEAR_REV:
					if (player->airborne)
						--player->z_delta;
					else
						--player->gear;
					break;

				case TURN_RIGHT:
					++player->a_delta;
					break;

				case TURN_LEFT:
					--player->a_delta;
					break;

				case FIRE_MISSILE:
					if (!player->loading_cd)
						player->firing = true;
					break;
			}
		}
	}

	if (active_human) {
		/* Process joystick input - JOY_2 is the first physical joystick */
		if (joy = joy_read(JOY_2)) {
			if (JOY_UP(joy)) {		/* Joystick forward - dive or gear up */
				if (player->airborne)
					player->z_delta -= JOY_UP(player->joy)? 2: 1;
				else
					++player->gear;
			} else if (JOY_DOWN(joy)) {	/* Joystick back - climb or gear down */
				if (player->airborne)
					player->z_delta += JOY_DOWN(player->joy)? 2: 1;
				else
					--player->gear;
			}
			if (JOY_RIGHT(joy))
				player->a_delta += JOY_RIGHT(player->joy)? 2: 1;
			else if (JOY_LEFT(joy))
				player->a_delta -= JOY_LEFT(player->joy)? 2: 1;
			if (JOY_BTN_1(joy)) {
				if (!player->loading_cd)
					player->firing = true;
			}
		}
		/* Save */
		player->joy = joy;

		/* Keep player deltas within limits! */
		player->z_delta = Min8(Max8(player->z_delta, -Z_DELTA_LMT), Z_DELTA_LMT);
		player->a_delta = Min8(Max8(player->a_delta, -A_DELTA_LMT), A_DELTA_LMT);
	}
}

void QueueNewMessages(XM_HANDLE messages)
{
	g_current_messages = messages;
	ResetMessages();
}

static void ResetMessages(void)
{
	f_message_cd = 0;
	f_message_index = 0;
	clrscr();
}

/* Default callback to do useful work while waiting */
static void DefaultCallback(uint8_t waiting)
{
	static uint16_t s_frame_counter;
	static uint8_t s_vehicle_cd, s_vehicle_index, s_width, s_height;
	static bool s_initialized, s_messages_checked;
	uint32_t a;
	char *p;
	uint8_t i, b, v;

	/* Save the width and height of the text overlay */
	if (!s_initialized) {
		screensize(&s_width, &s_height);
		s_initialized = true;
	}

	/* Perform tasks dependent on what we're waiting for */
	switch (waiting) {
		case FRAME_TO_FINISH:
			/* Once a frame, process any active sounds and prepare for vehicles and messages */
			if (s_frame_counter != g_frame_counter) {
				s_frame_counter = g_frame_counter;
				for (i = 0, a = VERA_PSG_BASE + VERA_PSG_RLV_OFFSET; i < VERA_PSG_VOICES; ++i, a += VERA_PSG_BLOCK_SIZE) {
					b = vpeek(a);
					v = b & VERA_PSG_VOLUME_MASK;
					if (v) {
						/* Follow the sequence for each frame */
						b = (b & VERA_PSG_RL_MASK) | g_psg_volumes[v];
						vpoke(b, a);
					}
					s_vehicle_cd = VEHICLE_COUNT;
					s_messages_checked = false;
				}
			} else {
				/* Do some NPC and Missile AI */
				if (s_vehicle_cd) {
#if PLAYER_INDEX == 0
					if (s_vehicle_index < PLAYER_LIMIT)
#else
					if (s_vehicle_index >= PLAYER_INDEX)
#endif
						NPCAI(g_vehicles + s_vehicle_index);
					else
						MissileAI(g_vehicles + s_vehicle_index);
					/* Skip to the next vehicle */
					s_vehicle_index += VEHICLE_IDX_INC;
					if (s_vehicle_index >= VEHICLE_COUNT)
						s_vehicle_index -= VEHICLE_COUNT;
					--s_vehicle_cd;
				/* Update the text */
				} else if (!g_human_joined && !s_messages_checked) {
					s_messages_checked = true;
					if (!f_message_cd) {
						if (IndexExistsForXM(g_current_messages, f_message_index - MSG_WINDOW))
							cclearxy(0, (((f_message_index - MSG_WINDOW) % MSG_LINES) << 1) + 1, s_width);
						if (IndexExistsForXM(g_current_messages, f_message_index)) {
							p = GetXMAddress(g_current_messages, f_message_index);
							gotoxy(0, ((f_message_index % MSG_LINES) << 1) + 1);
							for (i = 0; p[i]; ) {
								if (p[i] == '\e') {
									if (p[i + 1] == 'T') {
										++i;
										if (p[i + 1]) {
											++i;
											if (isdigit(p[i]))
												textcolor(p[i] - '0');
											else if (isupper(p[i]))
												textcolor(p[i] + 10 - 'A');
											++i;
										}
									}
								} else {
									cputc(p[i]);
									++i;
								}
							}
						}
						f_message_cd = MSG_DURATION;
						++f_message_index;
						if (f_message_index >= MSG_CYCLE) {
							f_message_index = 0;
							g_current_messages = g_attract_messages;
						}
					} else
						--f_message_cd;
				}
			}
			break;
		case SCREEN_TO_FINISH:
			/* Nothing to do here yet! */
			break;
	}
}

/* Sound routines */
void AddSound(uint8_t type)
{
	static uint8_t s_index;
	uint8_t *p;
	uint32_t a;

	a = VERA_PSG_BASE + (s_index << VERA_PSG_BLOCK_SHIFT);
	if (++s_index >= VERA_PSG_VOICES)
		s_index = 0;

	p = g_psg_settings + (type << VERA_PSG_BLOCK_SHIFT);

	vpoke(*p, a);		/* Frequency low byte */
	vpoke(*++p, ++a);	/* Frequency high byte */
	vpoke(*++p, ++a);	/* Initial volume */
	vpoke(*++p, ++a);	/* Waveform */
}

void StopSounds(void)
{
	uint8_t i;
	uint32_t a;

	for (i = 0, a = VERA_PSG_BASE + VERA_PSG_RLV_OFFSET; i < VERA_PSG_VOICES; ++i, a += VERA_PSG_BLOCK_SIZE)
		vpoke(VERA_PSG_VOLUME_OFF, a);	/* No volume */
}

/* Drawing routines */
void DrawLineFromTo16(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color)
{
	f_x_from_point = x1;
	f_y_from_point = y1;
	f_x_to_point = x2;
	f_y_to_point = y2;
	f_current_color = color;
	DrawLine16();
}

void DrawLineJustTo16(int16_t x, int16_t y, uint8_t color)
{
	f_x_from_point = f_x_to_point;
	f_y_from_point = f_y_to_point;
	f_x_to_point = x;
	f_y_to_point = y;
	f_current_color = color;
	DrawLine16();
}

static void DrawLine16(void)
{
	bool use_workaround;
	uint8_t d0, d1;
	int16_t x1, y1, x2, y2, xmin, xmax, ymin, ymax, xt, yt, dx, dy, slope;
	int32_t l;
	uint32_t address;
	register uint8_t c;
	register int16_t i;

	/* Copy because points may be changed and/or swapped after clipping */
	x1 = f_x_from_point;
	y1 = f_y_from_point;
	x2 = f_x_to_point;
	y2 = f_y_to_point;

	/* Will need these */
	ymax = Max16(y1, y2);
	ymin = Min16(y1, y2);
	xmax = Max16(x1, x2);
	xmin = Min16(x1, x2);

	/* Check if clipping is required */
	if (ymax > (VERA_VERT_RES - 1) || ymin < 0 || xmax > (VERA_HORZ_RES - 1) || xmin < 0) {
	    	/* If there's no chance of intersection, just return */
		if (ymax < 0 || ymin > (VERA_VERT_RES - 1) || xmax < 0 || xmin > (VERA_HORZ_RES - 1))
			return;
		/* Coordinates can change, so discarding xmin, xmax, ymin, and ymax */
		if (Max16(y1, y2) > (VERA_VERT_RES - 1) && y2 != y1) {
			xt = MulDiv16((VERA_VERT_RES - 1) - y1, x2 - x1, y2 - y1) + x1;
			if (y2 > y1) {
				y2 = VERA_VERT_RES - 1;
				x2 = xt;
			} else {
				y1 = VERA_VERT_RES - 1;
				x1 = xt;
			}
		}
		if (Min16(y1, y2) < 0 && y2 != y1) {
			xt = MulDiv16(-y1, x2 - x1, y2 - y1) + x1;
			if (y2 < y1) {
				y2 = 0;
				x2 = xt;
			} else {
				y1 = 0;
				x1 = xt;
			}
		}
		if (Max16(x1, x2) > (VERA_HORZ_RES - 1) && x2 != x1) {
			yt = MulDiv16((VERA_HORZ_RES - 1) - x1, y2 - y1, x2 - x1) + y1;
			if (x2 > x1) {
				x2 = VERA_HORZ_RES - 1;
				y2 = yt;
			} else {
				x1 = VERA_HORZ_RES - 1;
				y1 = yt;
			}
		}
		if (Min16(x1, x2) < 0 && x2 != x1) {
			yt = MulDiv16(-x1, y2 - y1, x2 - x1) + y1;
			if (x2 < x1) {
				x2 = 0;
				y2 = yt;
			} else {
				x1 = 0;
				y1 = yt;
			}
		}
		/* Final check! */
		if (y1 < 0 || y2 < 0 || y1 > (VERA_VERT_RES - 1) || y2 > (VERA_VERT_RES - 1)
		    || x1 < 0 || x2 < 0 || x1 > (VERA_HORZ_RES - 1) || x2 > (VERA_HORZ_RES - 1))
			return;
	}

	/* Render "right to left" to account for 0.5 pixel bias */
	if (x1 < x2) {
		xt = x1;
		x1 = x2;
		x2 = xt;
		yt = y1;
		y1 = y2;
		y2 = yt;
	}

	/* Will need these */
	dx = x2 - x1;
	dy = y2 - y1;

	/* Add 1 to i because we want to render all points up to and including the endpoints */
	if (abs(dy) > abs(dx)) {
		slope = abs(((int32_t)dx << VERA_SLOPE_SHIFT) / dy);
		d0 = VERA_ADV_BY_0_5 | (dx < 0? VERA_DECR: VERA_INCR);
		d1 = VERA_ADV_BY_160 | (dy < 0? VERA_DECR: VERA_INCR);
		i = abs(dy) + 1;
		use_workaround = f_use_workaround;
	} else {
		slope = abs(((int32_t)dy << VERA_SLOPE_SHIFT) / dx);
		d0 = VERA_ADV_BY_160 | (dy < 0? VERA_DECR: VERA_INCR);
		d1 = VERA_ADV_BY_0_5 | (dx < 0? VERA_DECR: VERA_INCR);
		i = abs(dx) + 1;
		use_workaround = false;
	}

	if (use_workaround) {
		dx = (dx > 0? slope: -slope) << WORKAROUND_SHIFT;
		dy = dy > 0? 1: -1;
		for (yt = y1, l = ((int32_t)x1 << SHIFT_1_0) + (SCALE_1_0 >> 1); i > 0; --i, yt += dy, l += dx)
			PlotPoint16(l >> SHIFT_1_0, yt, f_current_color);
	} else {
		/* Get the address */
		address = VERA_ADDR_FX(x1, y1);

		/* DCSEL = 2 */
		VERA_CTRL = VERA_DCSEL_2;

		/* Turn on FX 4-bit mode and line helper */
		VERA_FX_CTRL = VERA_FX_4_BIT_MODE | VERA_FX_LINE_HELPER;

		/* Select ADDR1 */
		VERA_CTRL = VERA_DCSEL_2 | VERA_ADDR_1;

		/* Always increment setting */
		VERA_ADDRx_H = d1;

		/* Set ADDR1 - address format is a bit wonky, just deal with it! */
		VERA_ADDRx_H |= (address & 0x01) << 1;
		address >>= 1;
		VERA_ADDRx_L = address & UINT8_MAX;
		address >>= CHAR_BIT;
		VERA_ADDRx_M = address & UINT8_MAX;
		address >>= CHAR_BIT;
		VERA_ADDRx_H |= address & 0x01;

		/* Select ADDR0 */
		VERA_CTRL = VERA_DCSEL_2 | VERA_ADDR_0;

		/* Occasionally increment setting */
		VERA_ADDRx_H = d0;

		/* DCSEL = 3 */
		VERA_CTRL = VERA_DCSEL_3;

		/* Set slope */
		VERA_FX_X_INCR_L = slope & UINT8_MAX;
		slope >>= CHAR_BIT;
		VERA_FX_X_INCR_H = slope & 0x03;

		/* Draw the line */
		//p = &VERA_DATA1;
		c = (f_current_color << HEX_DGT_SHIFT) | f_current_color;
		while (i >= 8) {
			/* The optimizer handles this perfectly! */
			VERA_DATA1 =
			VERA_DATA1 =
			VERA_DATA1 =
			VERA_DATA1 =
			VERA_DATA1 =
			VERA_DATA1 =
			VERA_DATA1 =
			VERA_DATA1 = c;
			i -= 8;
		}
		for (; i > 0; --i)
			VERA_DATA1 = c;

		/* DCSEL = 2 */
		VERA_CTRL = VERA_DCSEL_2;

		/* Turn off 4-bit and line helper mode */
		VERA_FX_CTRL = VERA_TRADITIONAL;
	}
}

/* Pixel routines */
void PlotPoint16(int16_t x, int16_t y, uint8_t color)
{
	uint32_t address;

	if (y >= 0 && y < VERA_VERT_RES && x >= 0 && x < VERA_HORZ_RES) {
		address = VERA_ADDR_PT(x, y);
		VERA_ADDRx_L = (uint8_t)(address & UINT8_MAX);
		address >>= CHAR_BIT;
		VERA_ADDRx_M = (uint8_t)(address & UINT8_MAX);
		address >>= CHAR_BIT;
		VERA_ADDRx_H = (uint8_t)(address & UINT8_MAX);

		if (x & 1) {
			VERA_DATA0 &= HEX_DGT_HI;
			VERA_DATA0 |= (color & HEX_DGT_LO);
		} else {
			VERA_DATA0 &= HEX_DGT_LO;
			VERA_DATA0 |= (color << HEX_DGT_SHIFT);
		}
	}

	/* Set as the current "to-point" */
	f_x_to_point = x;
	f_y_to_point = y;
}

void ErasePoint16(int16_t x, int16_t y)
{
	uint32_t address;

	if (y >= 0 && y < VERA_VERT_RES && x >= 0 && x < VERA_HORZ_RES) {
		address = VERA_ADDR_PT(x, y);
		VERA_ADDRx_L = (uint8_t)(address & UINT8_MAX);
		address >>= CHAR_BIT;
		VERA_ADDRx_M = (uint8_t)(address & UINT8_MAX);
		address >>= CHAR_BIT;
		VERA_ADDRx_H = (uint8_t)(address & UINT8_MAX);

		if (x & 1)
			VERA_DATA0 &= HEX_DGT_HI;
		else
			VERA_DATA0 &= HEX_DGT_LO;
	}
}

/* Convenience functions usually implemented as macros, but using functions to avoid side effects! */

int16_t Min16(int16_t a, int16_t b) { return a < b? a: b; }

int16_t Max16(int16_t a, int16_t b) { return a > b? a: b; }

int8_t Min8(int8_t a, int8_t b) { return a < b? a: b; }

int8_t Max8(int8_t a, int8_t b) { return a > b? a: b; }

/* The program insures that the values passed to and returned
   from MulDiv16 never exceed |32767|. Since this routine
   is used frequently, it is well worth the effort to optimize it. */

/* Equivalent to (int16_t)((int32_t)num1 * num2 / denom) */
int16_t MulDiv16(int16_t num1, int16_t num2, int16_t denom)
{
	int32_t c;

	/* Set up the multiplier */
	VERA_CTRL = VERA_DCSEL_2;
	VERA_FX_MULT = VERA_FX_MULTIPLIER;

	/* Set the operands */
	VERA_CTRL = VERA_DCSEL_6;
	VERA_FX_CACHE_L = ((uint8_t *)&num1)[0];
	VERA_FX_CACHE_M = ((uint8_t *)&num1)[1];
	VERA_FX_CACHE_H = ((uint8_t *)&num2)[0];
	VERA_FX_CACHE_U = ((uint8_t *)&num2)[1];

	/* Set to write in unused space between the screens */
	VERA_CTRL = VERA_DCSEL_2 | VERA_ADDR_0;
	VERA_FX_CTRL = VERA_FX_CACHE_WRITE;
	VERA_ADDRx_L = 0x00;
	VERA_ADDRx_M = VERA_UNUSED_ADDR;
	VERA_ADDRx_H = 0x00;

	/* Trigger the multiply */
	VERA_DATA0 = 0x00;

	/* Save the result */
	VERA_ADDRx_H = VERA_INC_1;
	/* VOLATILE */ ((uint8_t *)&c)[0] = VERA_DATA0;
	VOLATILE ((uint8_t *)&c)[1] = VERA_DATA0;
	VOLATILE ((uint8_t *)&c)[2] = VERA_DATA0;
	VOLATILE ((uint8_t *)&c)[3] = VERA_DATA0;

	/* Restore */
	VERA_FX_CTRL = VERA_TRADITIONAL;

	/* Unfortunately, no VERA divider (yet!) */
	return (int16_t)(c / denom);
}

