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


/// These functions receive a CMD and DATA IO Port Pair
/// indexes into the CMD and either 
///    retrieves the result from DATA or Writes the value to DATA

UINT8 io_index_byte_read_byte(
    UINT16 cmd_port,
    UINT16 data_port,
    UINT8  idx
);

UINT8 io_index_word_read_byte(
    UINT16 cmd_port,
    UINT16 data_port,
    UINT16 idx
);

UINT16 io_index_byte_read_word(
    UINT16 cmd_port,
    UINT16 data_port,
    UINT8  idx
);

UINT16 io_index_word_read_word(
    UINT16 cmd_port,
    UINT16 data_port,
    UINT16 idx
);



void io_index_byte_write_byte(
    UINT16 cmd_port,
    UINT16 data_port,
    UINT8  idx,
    UINT8  data
);

void io_index_word_write_byte(
    UINT16 cmd_port,
    UINT16 data_port,
    UINT16 idx,
    UINT8  data
);

void io_index_byte_write_word(
    UINT16 cmd_port,
    UINT16 data_port,
    UINT8  idx,
    UINT16 data
);

void io_index_word_write_word(
    UINT16 cmd_port,
    UINT16 data_port,
    UINT16 idx,
    UINT16 data
);
///////////////////////