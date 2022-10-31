#pragma once
#include "global.h"
#include "types.h"

/* MSR */
BOOL do_read_msr(UINT msr, UINT64 *res);
BOOL do_write_msr(UINT msr, UINT64 value);
void read_msr(int argc, char **argv);
void write_msr(int argc, char **argv);

