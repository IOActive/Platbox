#pragma once
#include "global.h"
#include "types.h"

/* MSR */
BOOL do_read_msr(UINT msr, UINT64 *res);
BOOL do_write_msr(UINT msr, UINT64 value);
BOOL do_read_msr_for_core(UINT32 core, UINT msr, UINT64 *res);
BOOL do_write_msr_for_core(UINT32 core, UINT msr, UINT64 value);