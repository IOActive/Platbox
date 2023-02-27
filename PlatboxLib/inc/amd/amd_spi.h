
#pragma once
#include "types.h"

typedef struct _ALT_SPI_CS {
	BYTE AltSpiCsEn		: 2;
	BYTE WriteBufferEn	: 1;
	BYTE SpiProtectEn0	: 1;
	BYTE SpiProtectEn1	: 1;
	BYTE SpiProtectLock : 1;
	BYTE lock_spi_cs 	: 1; // New 2022
	BYTE SpiCsDlySel	: 1;
} ALT_SPI_CS;

#define SPI_INDEX_MODE_READ_BLOCK_BITS 6
#define SPI_INDEX_MODE_READ_BLOCK_SIZE 64
#define SPI_INDEX_MODE_READ_BLOCK_MASK 0x3F
#define SPI_INDEX_MODE_WRITE_BLOCK_FIFO 8

#define SPI_INDEX_MODE_WRITE_BLOCK_4K 0x1000
#define SPI_INDEX_MODE_WRITE_BLOCK_BITS 12
#define SPI_INDEX_MODE_WRITE_BLOCK_MASK 0xFFF

typedef struct _SPI {
	// 0x00
    union {
        DWORD SPI_Cntrl0Value;
        struct {
            BYTE SpiOpCode;
            BYTE Rsvd0;
            BYTE ExecuteOpcode: 1;
            BYTE Rsvd1: 1;
            BYTE SpiReadMode_0: 1;
            BYTE SpiArbEnable: 1;
            BYTE FifoPtrClr: 1;
            BYTE IllegalAccess: 1;
            BYTE SpiAccessMacRomEn: 1;
            BYTE SpiHostAccessMacRomEn: 1;
            BYTE ArbWaitCount: 3;
            BYTE Rsvd2: 1;
            BYTE SpiClkGate: 1;
            BYTE SpiReadMode_1: 2;
            BYTE SpiBusy: 1;
        } SPI_Cntrl0 ;
    };
	
	// 0x04
	DWORD SPI_RestrictedCmd;

	// 0x08
	DWORD SPI_RestrictedCmd2;

	// 0x0C
	union {
		DWORD SPI_Cntrl1Value;
		struct {
			BYTE SpiParamters;
			BYTE FifoPtr: 3;
			BYTE TrackMacLockEn: 1;
			BYTE Rsvd: 4;
			BYTE WaitCount: 6;
			BYTE Rsvd2: 2;
			BYTE ByteCommand;
		} SPI_Cntrl1;
	};
	

	// 0x10
	DWORD SPI_CmdValue0;

	// 0x14
	DWORD SPI_CmdValue1;

	// 0x18
	DWORD SPI_CmdValue2;

	// 0x1C
	BYTE Reserved0;

	// 0x1D
	BYTE Alt_SPI_CS;

	// 0x1E
	BYTE SpiExtRegIndx;

	// 0x1F; confusing definition in spec
	BYTE SpiExtRegData;

	// 0x20
	BYTE SPI100Enable;

	// 0x22
	WORD SPI100SpeedConfig;

	// 0x24
	DWORD Reserved1;
	// 0x28
	DWORD Reserved2;

	// 0x2C 
	WORD SPI100HostPrefetchCfg;

	// 0x2E
	WORD Reserved3;
	
	// 0x30
	DWORD Reserved4[4];

	// 0x40
	BYTE DDRCmdCode;
	// 0x41
	BYTE QDRCmdCode;
	// 0x42
	BYTE DPRCmdCode;
	// 0x43
	BYTE QPRCmdCode;
	// 0x44
	BYTE ModeByte;
	// 0x45
	BYTE CmdCode;
	// 0x46
	BYTE Reserved5;
	// 0x47
	BYTE CmdTrig;
	// 0x48
	BYTE TxByteCnt;
	// 0x49 - 0x4A
	BYTE Reserved6[2];
	// 0x4B
	BYTE RxByteCnt;
	// 0x4C
	union {
		DWORD SpiStatusValue;
		struct {
			BYTE DoneByCount;
			BYTE FifoWrPtr: 7;
			BYTE Rsvd0: 1;
			BYTE FifoRdPtr: 7;
			BYTE Rsvd1: 1;
			BYTE Rsvd2: 7;
			BYTE SpiBusy: 1;
		} SpiStatus;
	};
	
	
	// 0x50
	DWORD fla_addr;
	BYTE Reserved7[0x2C];

	// 0x80
	BYTE SPI_regx80;
	BYTE SPI_regx81;
	BYTE SPI_regx82;

	BYTE FIFO[SPI_INDEX_MODE_READ_BLOCK_SIZE]; // Actually is 67

	// XXX To be extended if necessary

} SPI;

typedef struct _SPIRestrictedCmd {
	DWORD RestrictedCmd0 : 8;
	DWORD RestrictedCmd1 : 8;
	DWORD RestrictedCmd2 : 8;
	DWORD RestrictedCmd3 : 8;
} SPIRestrictedCmd;

typedef struct _SPIRestrictedCmd2 {
	DWORD RestrictedCmd4 : 8;
	DWORD RestrictedCmdWoAddr0 : 8;
	DWORD RestrictedCmdWoAddr1 : 8;
	DWORD RestrictedCmdWoAddr2 : 8;
} SPIRestrictedCmd2;

// todo: retrieve flash region size dynamically
#define FLASH_SIZE 16 * 1024 * 1024


void load_spi_information(DWORD spi_addr);
void print_spi_info();

// SPI Controller operations
void amd_spi_clear_fifo_ptr(volatile SPI *spi_base);
void amd_spi_execute_command(volatile SPI *spi_base);
void amd_spi_write_enable(volatile SPI *spi_base);
void amd_spi_print_fifo_stats(volatile SPI *spi_base);
DWORD amd_spi_read_id();

void amd_spi_erase_4k_block(volatile SPI *spi_base_arg, UINT32 address);

void read_from_flash_index_mode(volatile SPI *spi_base_arg, UINT32 start_offset, UINT32 length, BYTE *out_buff);
void amd_dump_spi_flash_index_mode(const char *output_filename);

void write_4k_block(volatile SPI *spi_base, UINT32 addr, BYTE *block);
void amd_spi_write_buffer(volatile SPI *spi_base_arg, UINT32 flash_address, BYTE *in_buff, UINT32 length);

// Exposed to other modules
extern DWORD g_spi_addr; 
extern SPI g_spi_registers;


#define Winbond_25Q128JVS 0xEF6018
#define Winbond_25Q128JVS_SECTOR_ERASE_OP 0x20