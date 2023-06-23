#pragma once
#include "global.h"
#include "types.h"

#define IO_SIZE_BYTE  0
#define IO_SIZE_WORD  1
#define IO_SIZE_DWORD 2

/* IO port operations */
UINT8 io_inb(UINT16 port);
UINT16 io_inw(UINT16 port);
UINT32 io_ind(UINT16 port);

void io_outb(UINT16 port, UINT8 val);
void io_outw(UINT16 port, UINT16 val);
void io_outd(UINT16 port, UINT32 val);

void io_index_data(UINT16 port_idx, UINT32 idx_val, UINT16 port_data, UINT32 data_val);