/*
    Contents: "process vehicles" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include "explore.h"

#define ANGLE_DELTA_SHIFT	1
#define XY_DELTA_SHIFT		10
#define UPPER_Z			(SCALE_1_0 * 7 - 1)
#define LOWER_Z			0
#define Z_DELTA_SHIFT		7

/* Process every active vehicle; the player-controlled one is done first */
PLAYER_STATUS *ProcessVehicles(void)
{
	PLAYER_STATUS *p;

	p = GetInput(0);
	if (p->angle_delta || (!p->sin && !p->cos)) {
		p->angle += (int16_t)p->angle_delta << ANGLE_DELTA_SHIFT;
		p->sin = Sin(p->angle);
		p->cos = Cos(p->angle);
		p->angle_delta = 0;
	}
	if (p->z_delta) {
		p->z += (int16_t)p->z_delta << Z_DELTA_SHIFT;
		if (p->z < LOWER_Z)
			p->z = LOWER_Z;
		else if (p->z > UPPER_Z)
			p->z = UPPER_Z;
		p->z_delta = 0;
	}
	p->x += p->sin >> XY_DELTA_SHIFT;
	p->y += p->cos >> XY_DELTA_SHIFT;

	return p;
}
