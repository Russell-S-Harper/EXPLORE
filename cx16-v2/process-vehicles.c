/*
    Contents: "process vehicles" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include "explore.h"

#define ANGLE_DELTA_SHIFT	1
#define XY_DELTA_SHIFT		4
#define Z_DELTA_SHIFT		7

/* Process every vehicle; the player-controlled one is done first */
void ProcessVehicles(void)
{
	VEHICLE *p = vehicles + PLAYER_INDEX;

	GetInput(p);
	if (p->angle_delta || (!p->sin && !p->cos)) {
		p->angle += (int16_t)p->angle_delta << ANGLE_DELTA_SHIFT;
		p->sin = Sin(p->angle);
		p->cos = Cos(p->angle);
		p->angle_delta = 0;
	}
	if (p->z_delta) {
		p->z += (int16_t)p->z_delta << Z_DELTA_SHIFT;
		p->z = Minimum(Maximum(p->z, MIN_XYZ), MAX_XYZ);
		p->z_delta = 0;
	}
	p->x += p->sin >> XY_DELTA_SHIFT;
	p->x = Minimum(Maximum(p->x, MIN_XYZ), MAX_XYZ);
	p->y += p->cos >> XY_DELTA_SHIFT;
	p->y = Minimum(Maximum(p->y, MIN_XYZ), MAX_XYZ);
}
