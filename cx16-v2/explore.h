/*
    Contents: "explore" C header file (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#ifndef _EXPLORE_H
#define _EXPLORE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "xm.h"

/* Overall arena dimensions */
#define MAX_XYZ			16383
#define MIN_XYZ			0

/* How many participants */
#define PLAYER_COUNT		4
#define MISSILE_COUNT		20
#define VEHICLE_COUNT		(PLAYER_COUNT + MISSILE_COUNT)

/* Indices and limits */
#define PLAYER_INDEX		0
#define PLAYER_LIMIT		PLAYER_COUNT
#define MISSILE_INDEX		PLAYER_COUNT
#define MISSILE_LIMIT		VEHICLE_COUNT

/* The human identifier, if playing */
#define HUMAN_ID		PLAYER_INDEX

/* Initial player health */
#define PLAYER_HEALTH		60

/* How +/- close in height to be considered targetable */
#define VEHICLE_Z_TOL		191

/* Exploadable distances - these are scaled */
#define MSS_XY_TOL		114
#define MSS_DST_TOL_SQR		13224
#define TGT_NR_XY_TOL		114
#define TGT_FR_XY_TOL		171

/* Counters (in frames) for how long a missile takes to reload, missile lifespan, and hit animation */
#define MSS_LOADING		6
#define MSS_LIVE		30
#define MSS_HIT			3

/* Explosions and badges are last three "vehicles" */
#define EXP_APP_PRM_OFFSET	3
#define EXP_APP_AUX_OFFSET	2
#define HMN_BDG_AUX_OFFSET	1

/* Appearance */
enum {APP_PRM, APP_AUX, APP_MSS, APP_CNT};

/* VEHICLE combines player, NPCs, and missiles; appearance is set up as an array so we
   can iterate through them in a loop */
typedef struct {
	bool active, npc, airborne, exploding, firing;
	int8_t health, z_delta, last_z_delta, a_delta, last_a_delta, level, gear;
	uint8_t identifier, target, joy, mss_delta, damage, hit_cd, loading_cd, live_cd;
	int16_t x, y, z, angle, sin, cos;
	XM_HANDLE appearance[APP_CNT];
} VEHICLE;

typedef struct {
	bool last, airborne;
	uint8_t arena, gear, mss_delta, damage;
	XM_HANDLE player, missile;
} LEVEL;

/******** Function declarations ********/

/* Routines called by main */
void InitProgram(void);			/* Defined in initialize.c */
void InitPlayers(void);			/* Defined in initialize.c */
void ProcessVehicles(void);		/* Defined in process-vehicles.c */
void RenderObjects(void);		/* Defined in render-objects.c */
void UpdateDisplay(void);		/* Defined in cx16-specific.c */

/* Routine defined in cx16-specific.c and called by InitProgram */
void InitSpecific(void);

/* Math routines defined in cx16-specific.c and called throughout the program */
int16_t MulDiv16(int16_t num1, int16_t num2, int16_t denom);
#define SpcMul16(num1, num2)	MulDiv16((num1), (num2), SCALE_1_0)
#define SpcDiv16(num, denom)	MulDiv16((num), SCALE_1_0, (denom))

/* Drawing routines */
void DrawLineFromTo16(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
void DrawLineJustTo16(int16_t x, int16_t y, uint8_t color);
void PlotPoint16(int16_t x, int16_t y, uint8_t color);
void ErasePoint16(int16_t x, int16_t y);

/* Routines defined in cx16-specific.c and called by ProcessVehicle */
void GetPlayerInput(VEHICLE *player);
void AddSound(uint8_t type);
void StopSounds(void);
void QueueNewMessages(XM_HANDLE messages);

/* Defined in initialize.c, called by InitVehicles and other places */
bool AdvancePlayer(VEHICLE *player);

/* AI */
typedef enum {
	EVT_HUMAN_JOINED,	/* human has joined the game */
	EVT_NEW_ARENA,		/* players are playing in a new arena */
	EVT_SWITCHED_FOCUS,	/* focus was switched to a new player (to cut short mourning) */
	EVT_DAMAGED_PLAYER,	/* a player has been damaged */
	EVT_PLAYER_CORNERED,	/* a player is cornered */
	EVT_PLAYER_IMPEDED,	/* a player or missile is stuck, or a player is brushing up against a wall, or had an unsuccessful pursuit */
	EVT_REACHED_TOP,	/* a player has reached the top of the arena */
	EVT_REACHED_BOTTOM,	/* a player has reached the bottom of the arena */
	EVT_ADVANCED_PLAYER,	/* a player has advanced */
	EVT_ELIMINATED_PLAYER,	/* a player has been eliminated */
	EVT_WINNING_PLAYER	/* a player has won */
} AI_EVENT;

/* Game modes */
enum {MD_JOIN_AS_NORMAL, MD_FOCUS_ON_HUMAN, MD_NO_REFRESH_AT_LAST_LEVEL};

/* Impediment types and severities  */
enum {IMP_LOW = -3, IMP_MEDIUM = -5, IMP_HIGH = -8, IMP_EXTREME = -13};

void InitAI(void);
void NPCAI(VEHICLE *player);
void MissileAI(VEHICLE *missile);
void ReportToAI(VEHICLE *player, AI_EVENT event, int16_t extra);

/* Other routines called throughout the program */
int16_t Min16(int16_t a, int16_t b);
int16_t Max16(int16_t a, int16_t b);
int8_t Min8(int8_t a, int8_t b);
int8_t Max8(int8_t a, int8_t b);
int16_t Sin(int16_t angle);
int16_t Cos(int16_t angle);

/* Variable declarations */
extern XM_HANDLE
	g_error_messages,
	g_current_messages,
	g_attract_messages,
	g_summary_messages,
	g_trig_data,
	g_crc_data,
	g_squares,
	g_arena_data,
	g_exploding_prm,
	g_exploding_aux,
	g_human_id_aux,
	*g_vehicle_data;

extern VEHICLE
	*g_vehicles;

extern LEVEL
	*g_levels;

extern bool
	g_human_joined,
	g_focus_on_human,
	g_no_refresh_at_last_level,
	g_exit_program;

extern uint8_t
	g_arena_index,
	g_vehicle_index,
	g_active_players,
	*g_psg_settings,
	*g_psg_volumes;

extern uint16_t
	g_display_width,
	g_display_height,
	g_frame_counter;

extern int16_t
	g_max_arena_segments,
	g_max_arena_vertices,
	g_max_vehicle_segments,
	g_max_vehicle_vertices;

extern const char
	*g_read_mode,
	*g_write_mode;

#endif /* _EXPLORE_H */
