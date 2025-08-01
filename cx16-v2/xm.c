/*
    Contents: "extended memory" C source code (c) 2024
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

#include <stdlib.h>
#include <string.h>
#include "xm.h"

void InitXM(void)
{
	ALLOC_XM working;

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
}

XM_HANDLE AllocXM(size_t limit, size_t size)
{
	static XM_HANDLE s_current_handle = ALLOC_XM_HANDLE + 1;
	static uint8_t s_current_bank = ALLOC_XM_BANK + 1;
	static size_t s_current_offset;
	ALLOC_XM working;
	uint8_t shift;
	size_t total;

	// Find the smallest power of two to accommodate
	shift = 0;
	while ((1 << shift) < size)
		++shift;

	/* Check the allocation */
	if ((total = limit * (1 << shift)) > XM_SIZE)
		ExitProgram(ERR_XM);

	/* Get a new bank if required */
	if (XM_SIZE - s_current_offset < total) {
		++s_current_bank;
		s_current_offset = 0;
	}

	/* Set up a new allocation */
	working.bank = s_current_bank;
	working.offset = XM_ADDRESS + s_current_offset;
	working.limit = limit;
	working.size = size;
	working.shift = shift;
	SetXM(ALLOC_XM_HANDLE, s_current_handle, &working);

	/* Update the offset */
	s_current_offset += total;

	/* Return the allocation handle */
	return s_current_handle++;
}

bool IndexExistsForXM(XM_HANDLE handle, int16_t index)
{
	ALLOC_XM *working;
	uint8_t bank;
	bool result;

	/* Confirm index is non-negative */
	if (result = (index >= 0)) {
		/* Retrieve the allocation details and check against the limit */
		bank = XM_REGISTER;
		XM_REGISTER = ALLOC_XM_BANK;
		working = (ALLOC_XM *)(XM_ADDRESS + (handle << ALLOC_XM_SHIFT));
		result = (index < working->limit);
		XM_REGISTER = bank;
	}

	return result;
}

void GetOrSetXM(XM_HANDLE handle, int16_t index, void *data, XM_MODE get_or_set)
{
	ALLOC_XM *working;
	void *address;
	size_t size;

	/* Save the bank */
	uint8_t bank = XM_REGISTER;

	/* Get the ALLOC_XM record */
	XM_REGISTER = ALLOC_XM_BANK;
	working = (ALLOC_XM *)(XM_ADDRESS + (handle << ALLOC_XM_SHIFT));

	/* Check for out of bounds */
	if (index < 0 || index >= working->limit)
		ExitProgram(ERR_OB);

	/* Get the address and size */
	address = (void *)(working->offset + (index << working->shift));
	size = working->size;

	/* Switch to the correct bank */
	XM_REGISTER = working->bank;

	/* Copy */
	switch (get_or_set) {
		case XM_GET:
			memcpy(data, address, size);
			break;
		case XM_SET:
			memcpy(address, data, size);
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
void *GetXMAddress(XM_HANDLE handle, int16_t index)
{
	ALLOC_XM *working;
	void *address;

	/* Get the ALLOC_XM record */
	XM_REGISTER = ALLOC_XM_BANK;
	working = (ALLOC_XM *)(XM_ADDRESS + (handle << ALLOC_XM_SHIFT));

	/* Check for out of bounds */
	if (index < 0 || index >= working->limit)
		ExitProgram(ERR_OB);

	/* Calculate the address */
	address = (void *)(working->offset + (index << working->shift));

	/* Switch to the correct bank */
	XM_REGISTER = working->bank;

	/* Return the address */
	return address;
}

/* For the direct XM functions, just call the standard procedure using the address of the value. */
int16_t GetXMDirectSigned(XM_HANDLE handle, int16_t index)
{
	int16_t value;
	GetOrSetXM(handle, index, &value, XM_GET);
	return value;
}

uint16_t GetXMDirectUnsigned(XM_HANDLE handle, int16_t index)
{
	uint16_t value;
	GetOrSetXM(handle, index, &value, XM_GET);
	return value;
}
