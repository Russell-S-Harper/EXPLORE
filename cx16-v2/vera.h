/*
    Contents: "VERA" C header file (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#ifndef _VERA_H
#define _VERA_H

#define VERA_ADDRx_L		(*((uint8_t *)0x9F20))
#define VERA_ADDRx_M		(*((uint8_t *)0x9F21))
#define VERA_ADDRx_H		(*((uint8_t *)0x9F22))
#define VERA_DATA0		(*((volatile uint8_t *)0x9F23))
#define VERA_DATA1		(*((volatile uint8_t *)0x9F24))
#define VERA_CTRL		(*((uint8_t *)0x9F25))
#define VERA_IEN		(*((uint8_t *)0x9F26))
#define VERA_ISR		(*((uint8_t *)0x9F27))
#define VERA_IRQLINE_L		(*((uint8_t *)0x9F28))
#define VERA_SCANLINE_L		(*((const uint8_t *)0x9F28))
#define VERA_L0_CONFIG		(*((uint8_t *)0x9F2D))
#define VERA_L0_MAPBASE		(*((uint8_t *)0x9F2E))
#define VERA_L0_TILEBASE	(*((uint8_t *)0x9F2F))
#define VERA_L1_CONFIG		(*((uint8_t *)0x9F34))
#define VERA_L1_MAPBASE		(*((uint8_t *)0x9F35))
#define VERA_L1_TILEBASE	(*((uint8_t *)0x9F36))

/* DCSEL = 0 */
#define VERA_DCSEL_0		0x00
#define VERA_DC_VIDEO		(*((uint8_t *)0x9F29))
#define VERA_DC_HSCALE		(*((uint8_t *)0x9F2A))
#define VERA_DC_VSCALE		(*((uint8_t *)0x9F2B))
#define VERA_DC_BORDER		(*((uint8_t *)0x9F2C))

/* DCSEL = 1 */
#define VERA_DCSEL_1		(0x01 << 1)
#define VERA_DC_HSTART		(*((uint8_t *)0x9F29))
#define VERA_DC_HSTOP		(*((uint8_t *)0x9F2A))
#define VERA_DC_VSTART		(*((uint8_t *)0x9F2B))
#define VERA_DC_VSTOP		(*((uint8_t *)0x9F2C))

/* DCSEL = 2 */
#define VERA_DCSEL_2		(0x02 << 1)
#define VERA_FX_CTRL		(*((uint8_t *)0x9F29))
#define VERA_FX_TILEBASE	(*((uint8_t *)0x9F2A))
#define VERA_FX_MAPBASE		(*((uint8_t *)0x9F2B))
#define VERA_FX_MULT		(*((uint8_t *)0x9F2C))

/* DCSEL = 3 */
#define VERA_DCSEL_3		(0x03 << 1)
#define VERA_FX_X_INCR_L	(*((uint8_t *)0x9F29))
#define VERA_FX_X_INCR_H	(*((uint8_t *)0x9F2A))
#define VERA_FX_Y_INCR_L	(*((uint8_t *)0x9F2B))
#define VERA_FX_Y_INCR_H	(*((uint8_t *)0x9F2C))

/* DCSEL = 4 */
#define VERA_DCSEL_4		(0x04 << 1)
#define VERA_FX_X_POS_L		(*((uint8_t *)0x9F29))
#define VERA_FX_X_POS_H		(*((uint8_t *)0x9F2A))
#define VERA_FX_Y_POS_L		(*((uint8_t *)0x9F2B))
#define VERA_FX_Y_POS_H		(*((uint8_t *)0x9F2C))

/* DCSEL = 5 */
#define VERA_DCSEL_5		(0x05 << 1)
#define VERA_FX_X_POS_S		(*((uint8_t *)0x9F29))
#define VERA_FX_Y_POS_S		(*((uint8_t *)0x9F2A))
#define VERA_FX_POLY_FILL_L	(*((const uint8_t *)0x9F2B))
#define VERA_FX_POLY_FILL_H	(*((const uint8_t *)0x9F2C))

/* Various settings */
#define VERA_ADDR_0		0x00
#define VERA_ADDR_1		0x01

#define VERA_16_COLOR_MODE	0x06
#define VERA_SCANLINE_H		0x40
#define VERA_TRADITIONAL	0x00
#define VERA_FX_4_BIT_MODE	0x04
#define VERA_FX_LINE_HELPER	0x01

#define VERA_ADV_BY_160		0xD0
#define VERA_ADV_BY_0_5		0x04
#define VERA_DECR		0x08
#define VERA_INCR		0x00

/* Miscellaneous */
#define VERA_HORZ_RES		320
#define VERA_VERT_RES		240

#define VERA_SCR_1_BASE		0x00
#define VERA_SCR_1_ADDR		(VERA_SCR_1_BASE << 1)
#define VERA_SCR_2_BASE		0x4C
#define VERA_SCR_2_ADDR		(VERA_SCR_2_BASE << 1)

#define VERA_PX_PER_BYTE	2

/* Equivalent to y * VERA_HORZ_RES / VERA_PX_PER_BYTE + x / VERA_PX_PER_BYTE */
#define VERA_ADDR_PT(x, y)	(((uint32_t)(y) << 7) + ((y) << 5) + ((x) >> 1) \
					+ ((uint32_t)(VERA_L0_TILEBASE? VERA_SCR_1_ADDR: VERA_SCR_2_ADDR) << 8))

/* Equivalent to y * VERA_HORZ_RES + x */
#define VERA_ADDR_FX(x, y)	(((uint32_t)(y) << 8) + ((y) << 6) + (x) \
					+ ((uint32_t)(VERA_L0_TILEBASE? VERA_SCR_1_ADDR: VERA_SCR_2_ADDR) << 9))

#endif /* _VERA_H */
