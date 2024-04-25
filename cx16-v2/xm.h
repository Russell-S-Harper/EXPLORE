/*
    Contents: "extended memory" C header file (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#ifndef _XM_H
#define _XM_H

#include <stdint.h>
#include <cx16.h>
#include "common.h"

#define XM_REGISTER	RAM_BANK
#define XM_ADDRESS	((size_t)BANK_RAM)
#define XM_SIZE		8192

#define ALLOC_XM_HANDLE	0
#define ALLOC_XM_BANK	1		/* The O/S uses bank 0, so the next is 1 */
#define ALLOC_XM_OFFSET	XM_ADDRESS

#define GetXM(handle, index, data)	GetOrSetXM((handle), (index), (data), XM_GET)
#define SetXM(handle, index, data)	GetOrSetXM((handle), (index), (data), XM_SET)
#define GetXMAddressInitial(handle)	GetXMAddress((handle), 0)

typedef int16_t XM_HANDLE;
typedef enum {XM_GET, XM_SET} XM_MODE;
typedef struct { uint8_t bank, shift; size_t offset, limit, size; } ALLOC_XM;

#define ALLOC_XM_SHIFT	3		/* ceil(log2(sizeof(ALLOC_XM))) */

void InitXM(void);
XM_HANDLE AllocXM(size_t limit, size_t size);
void GetOrSetXM(XM_HANDLE handle, int16_t index, void *data, XM_MODE get_or_set);
void *GetXMAddress(XM_HANDLE handle, int16_t index);
int16_t GetXMDirectSigned(XM_HANDLE handle, int16_t index);
uint16_t GetXMDirectUnsigned(XM_HANDLE handle, int16_t index);
void SetXMDirectSigned(XM_HANDLE handle, int16_t index, int16_t value);
void SetXMDirectUnsigned(XM_HANDLE handle, int16_t index, uint16_t value);

#endif /* _XM_H */
