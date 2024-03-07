/*
    Contents: "extended memory" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "xm.h"

XM_HANDLE AllocXM(size_t limit, size_t size) {
	static XM_HANDLE current_handle = ALLOC_XM_HANDLE + 1;
	static unsigned char current_bank = ALLOC_XM_BANK + 1;
	static size_t current_offset;
	static bool initialized;
	ALLOC_XM working;
	size_t total;

	if (!initialized) {
		/* Clear the space */
		XM_REGISTER = ALLOC_XM_BANK;
		memset((void *)XM_ADDRESS, 0, XM_SIZE);

		/* Set up to record XM allocations */
		working.bank = ALLOC_XM_BANK;
		working.offset = ALLOC_XM_OFFSET;
		working.limit = XM_SIZE >> ALLOC_XM_SHIFT;
		working.size = sizeof(ALLOC_XM);
		working.shift = ALLOC_XM_SHIFT;
		memcpy((void *)XM_ADDRESS, &working, sizeof(ALLOC_XM));
		initialized = true;
	}

	if ((total = limit * size) > XM_SIZE)
		ExitProgram(XMERR);

	/* Get a new bank if required */
	if (XM_SIZE - current_offset < total) {
		++current_bank;
		current_offset = 0;
	}

	/* Set up a new allocation */
	working.bank = current_bank;
	working.offset = XM_ADDRESS + current_offset;
	working.limit = limit;
	working.size = size;
	working.shift = 0;
	while ((1 << working.shift) < working.size)
		++working.shift;
	SetXM(ALLOC_XM_HANDLE, current_handle, &working);

	/* Update the offset */
	current_offset += total;

	/* Return the allocation handle */
	return current_handle++;
}

void GetOrSetXM(XM_HANDLE handle, int16_t index, void *data, XM_MODE get_or_set) {
	ALLOC_XM working;
	void *bm_address;

	/* Save the bank */
	unsigned char bank = XM_REGISTER;

	/* Get the ALLOC_XM record */
	XM_REGISTER = ALLOC_XM_BANK;
	memcpy(&working, (void *)(XM_ADDRESS + (handle << ALLOC_XM_SHIFT)), sizeof(ALLOC_XM));

	/* Check for out of bounds */
	if (index < 0 || index >= working.limit)
		ExitProgram(OBERR);

	/* Switch to the correct bank */
	XM_REGISTER = working.bank;

	/* Copy the data */
	bm_address = (void *)(working.offset + (index << working.shift));

	switch (get_or_set) {
		case XM_GET:
			memcpy(data, bm_address, working.size);
			break;
		case XM_SET:
			memcpy(bm_address, data, working.size);
			break;
	}

	/* Restore the bank */
	XM_REGISTER = bank;
}

/* Provides a pointer to the banked memory for direct access. Note
	that this function will change the active bank and not
	restore it. If you're alternating among banked memory
	allocations, it's best to refresh the address before each
	access, or keep track of the banks, or use a copy with GetXM. */
void *GetXMAddress(XM_HANDLE handle, int16_t index) {
	ALLOC_XM working;
	void *bm_address;

	/* Get the ALLOC_XM record */
	XM_REGISTER = ALLOC_XM_BANK;
	memcpy(&working, (void *)(XM_ADDRESS + (handle << ALLOC_XM_SHIFT)), sizeof(ALLOC_XM));

	/* Check for out of bounds */
	if (index < 0 || index >= working.limit)
		ExitProgram(OBERR);

	/* Switch to the correct bank */
	XM_REGISTER = working.bank;

	/* Calculate the address */
	bm_address = (void *)(working.offset + (index << working.shift));

	/* Return the address */
	return bm_address;
}

/* For the direct XM functions, just call the standard procedure using the address of the value. */
int16_t GetXMDirectSigned(XM_HANDLE handle, int16_t index) {
	int16_t value;
	GetOrSetXM(handle, index, &value, XM_GET);
	return value;
}

uint16_t GetXMDirectUnsigned(XM_HANDLE handle, int16_t index) {
	uint16_t value;
	GetOrSetXM(handle, index, &value, XM_GET);
	return value;
}

void SetXMDirectSigned(XM_HANDLE handle, int16_t index, int16_t value) {
	GetOrSetXM(handle, index, &value, XM_SET);
}

void SetXMDirectUnsigned(XM_HANDLE handle, int16_t index, uint16_t value) {
	GetOrSetXM(handle, index, &value, XM_SET);
}
