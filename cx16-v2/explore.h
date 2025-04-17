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
	int8_t health, z_delta, a_delta, level, gear;
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
void ProcessVehicles(void);		/* Defined in process-vehicles.c */
void RenderObjects(void);		/* Defined in render-objects.c */
void UpdateDisplay(void);		/* Defined in cx16-specific.c */

/* Routine defined in cx16-specific.c and called by InitProgram */
void InitSpecific(void);

/* Math routines defined in cx16-specific.c and called throughout the program */
int16_t MultiplyThenDivide(int16_t num1, int16_t num2, int16_t denom);
#define SpecialMultiply(num1, num2)	MultiplyThenDivide((num1), (num2), SCALE_1_0)
#define SpecialDivide(num, denom)	MultiplyThenDivide((num), SCALE_1_0, (denom))

/* Drawing routines */
void DrawLineFromTo16(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
void DrawLineJustTo16(int16_t x, int16_t y, uint8_t color);
void PlotPoint16(int16_t x, int16_t y, uint8_t color);
void ErasePoint16(int16_t x, int16_t y);

/* Routines defined in cx16-specific.c and called by ProcessVehicle */
void GetPlayerInput(VEHICLE *player);
void AddSound(uint8_t type);
void StopSounds(void);

/* Routine defined in initialize.c and called by InitVehicles and other places */
bool AdvancePlayer(VEHICLE *player);

/* AI */
typedef enum {
	AIE_NEW_ARENA,
	AIE_SWITCHED_FOCUS,
	AIE_STUCK_PLAYER,
	AIE_STUCK_MISSILE,
	AIE_REACHED_TOP,
	AIE_REACHED_BOTTOM,
	AIE_DAMAGED_PLAYER,
	AIE_ADVANCED_PLAYER,
	AIE_ELIMINATED_PLAYER,
	AIE_WINNING_PLAYER
} AI_EVENT;

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

/******** Variable declarations ********/
extern XM_HANDLE
	g_string_data,
	g_trig_data,
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
	g_exit_program;

extern uint8_t
	*g_psg_settings,
	*g_psg_volumes;

extern uint16_t
	g_display_width,
	g_display_height,
	g_frame_counter,
	*g_squares;

extern int16_t
	g_arena_index,
	g_max_arena_segments,
	g_max_arena_vertices,
	g_vehicle_index,
	g_max_vehicle_segments,
	g_max_vehicle_vertices;

#endif /* _EXPLORE_H */
