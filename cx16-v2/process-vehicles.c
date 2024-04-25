/*
    Contents: "process vehicles" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include "explore.h"

/* Process every active vehicle; the player-controlled one is done first */
PLAYER_STATUS *ProcessVehicles(void)
{
	PLAYER_STATUS *p;

	p = GetInput(0);
	if (p->angle_delta || (!p->sin && !p->cos)) {
		p->angle += p->angle_delta;
		p->sin = Sin(p->angle);
		p->cos = Cos(p->angle);
		p->angle_delta = 0;
	}
	if (p->z_delta) {
		p->z += p->z_delta;
		p->z_delta = 0;
	}
	p->x += p->sin >> 10;
	p->y += p->cos >> 10;

	return p;
}
