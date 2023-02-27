#include "x86Bios.h"

NTSTATUS Init(VOID)
{
	/*  Get "real mode" memory for an E820_DESCRIPTOR.  */

	ULONG cb = sizeof(E820_DESCRIPTOR);
	NTSTATUS status = x86BiosAllocateBuffer(&cb, &m_Segment, &m_Offset);
	if (NOT NT_SUCCESS(status)) return status;

	/*  Initialisation isn't necessary, but has the merit that we can
		check that the buffer is usable.  */

	E820_DESCRIPTOR desc = { 0 };
	return x86BiosWriteMemory(m_Segment, m_Offset, &desc, sizeof(desc));
};