#include "amd_spi.h"
#include "global.h"
#include "physmem.h"
#include "Util.h"

DWORD g_spi_addr;
SPI g_spi_registers;
bool bSpiInformationInitialized;
bool bNewChipset;
DWORD g_flash_id;

UINT32 AMD_FLASH_SIZE;
UINT64 AMD_FLASH_BASE;


BYTE SPI_READ_OP;
BYTE SPI_WRITE_ENABLE_OP;
BYTE SPI_WRITE_DISABLE_OP;
BYTE SPI_WRITE_BYTE_OP;
BYTE SPI_WRITE32_OP;
BYTE SPI_RDID_OP;
BYTE SPI_SECTOR_ERASE_OP; // vendor specific - not shown in the SPI Controller region
BYTE SPI_READ_STATUS_REG1_OP;

int amd_spi_check_busy(volatile SPI* spi_base) {
    amd_spi_clear_fifo_ptr(spi_base);
    spi_base->CmdCode = SPI_READ_STATUS_REG1_OP;
    spi_base->RxByteCnt = 1;
    spi_base->TxByteCnt = 0;
    amd_spi_execute_command(spi_base);
    char status = spi_base->mode32.SPI_regx80;
    return status & 1;
}

void print_SPI_Cntrl0() {
    // AMD 15h Family
    // SPIx00 SPI_Cntrl0

    /*
    SpiHostAccessRomEn. Read; Write-0-only. Reset: 1. This is a clear-once protection bit; once it is 
    cleared to 0 it cannot be set back to 1. 0=MAC cannot access BIOS ROM space (upper 512KB). 
    1=MAC can access BIOS ROM space.

    SpiAccessMacRomEn. IF (Mode == SMI) Read-write. ELSE Read; Write-0-only. ENDIF. Reset: 1. 
    This is a clear-once protection bit. 0=Software cannot access MAC�s portion of the ROM space 
    (lower 512KB). 1=Software can access MAC�s portion of the ROM space
    */
    DWORD spiCntrl0 = g_spi_registers.SPI_Cntrl0Value;
    int SpiAccessMacRomEn  = (spiCntrl0 >> 22) & 1;
    int SpiHostAccessRomEn = (spiCntrl0 >> 23) & 1;
    printf("SPIx00 - SPI_Cntrl0: %08x\n", spiCntrl0);
    printf("  -  SpiAccessMacRomEn: %d - ", SpiAccessMacRomEn);
    if (SpiAccessMacRomEn == 1) {
        print_red("FAILED\n");
    } else {
        print_green("OK\n");
    }
    printf("  - SpiHostAccessRomEn: %d - ", SpiHostAccessRomEn);
    if (SpiHostAccessRomEn == 1) {
        print_red("FAILED\n");
    } else {
        print_green("OK\n");
    }

    printf("\n");
}

void print_SPI_RestrictedCmds() {

    SPIRestrictedCmd* spiRestrictedCmd = (SPIRestrictedCmd *) &g_spi_registers.SPI_RestrictedCmd;
    SPIRestrictedCmd2* spiRestrictedCmd2 = (SPIRestrictedCmd2 *) &g_spi_registers.SPI_RestrictedCmd2;

    printf(" RestrictedCmd: %02x %02x %02x %02x\n",
        spiRestrictedCmd->RestrictedCmd0,
        spiRestrictedCmd->RestrictedCmd1,
        spiRestrictedCmd->RestrictedCmd2,
        spiRestrictedCmd->RestrictedCmd3);

    printf("RestrictedCmd2: %02x %02x %02x %02x\n",
        spiRestrictedCmd2->RestrictedCmd4,
        spiRestrictedCmd2->RestrictedCmdWoAddr0,
        spiRestrictedCmd2->RestrictedCmdWoAddr1,
        spiRestrictedCmd2->RestrictedCmdWoAddr2);

    
    // Write Status Register instruction (0x01)
    // Write Enable (0x06)
    // Chip Erase (0xC7)

    BOOL bWriteStatusRegBlocked = false;
    BOOL bWriteEnableBlocked     = false;
    BOOL bChipEraseBlocked       = false;
    
    UINT64 restricted_ops = (*(UINT64 *)spiRestrictedCmd2 << 32) | (*(UINT64 *) spiRestrictedCmd);
    for (int i = 0; i < 8; i++) {
        BYTE op = (restricted_ops >> i*8) & 0xFF;
        if (op == 0xC7) {
            bChipEraseBlocked = true;
        }
        if (op == 0x06) {
            bWriteEnableBlocked = true;
        }
        if (op == 0x01) {
            bWriteStatusRegBlocked = true;
        }
    }

    // printf("  - Write Status Register Op (0x01): ");
    // if (bWriteStatusRegBlocked == false) {
    //     print_red("FAILED (Not blocked)\n");
    // } else {
    //     print_green("OK\n");
    // }

    printf("  - Write Enable Op (0x06): ");
    if (bWriteEnableBlocked == false) {
        print_red("FAILED (Not blocked)\n");
    } else {
        print_green("OK\n");
    }

    // printf("  - Chip Erase Op (0xC7): ");
    // if (bChipEraseBlocked == false) {
    //     print_red("FAILED (Not blocked)\n");
    // } else {
    //     print_green("OK\n");
    // }


    printf("\n");
}

void print_SPI_Cntrl1() {
    BYTE ByteCommand = g_spi_registers.SPI_Cntrl1Value >> 24;
    printf("SPIx0C [SPI_Cntrl1] - ByteCommand: %02x\n", ByteCommand);
}

void print_SPI_CmdValue0() {
    BYTE MacLockCmd0 = g_spi_registers.SPI_CmdValue0 & 0xFF;
    BYTE MacLockCmd1 = (g_spi_registers.SPI_CmdValue0 >> 8 ) & 0xFF;
    BYTE MacUnlockCmd0 = (g_spi_registers.SPI_CmdValue0 >> 16 ) & 0xFF;
    BYTE MacUnlockCmd1 = (g_spi_registers.SPI_CmdValue0 >> 24 ) & 0xFF;

    printf("SPIx10 - CmdVal0\n");
    printf("  - MacLockCmd0: %02x\n", MacLockCmd0);
    printf("  - MacLockCmd1: %02x\n", MacLockCmd1);
    printf("  - MacUnlockCmd0: %02x\n", MacUnlockCmd0);
    printf("  - MacUnlockCmd1: %02x\n", MacUnlockCmd1);
}

void print_SPI_CmdValue1() {
    BYTE WREN = g_spi_registers.SPI_CmdValue1 & 0xFF;
    BYTE WRDI = (g_spi_registers.SPI_CmdValue1 >> 8 ) & 0xFF;
    BYTE RDID = (g_spi_registers.SPI_CmdValue1 >> 16 ) & 0xFF;
    BYTE RDSR = (g_spi_registers.SPI_CmdValue1 >> 24 ) & 0xFF;

    printf("SPIx14 - CmdVal1\n");
    printf("  - WREN: %02x\n", WREN);
    printf("  - WRDI: %02x\n", WRDI);
    printf("  - RDID: %02x\n", RDID);
    printf("  - RDSR: %02x\n", RDSR);
}

void print_SPI_CmdValue2() {
    BYTE Read = g_spi_registers.SPI_CmdValue2 & 0xFF;
    BYTE FRead = (g_spi_registers.SPI_CmdValue2 >> 8 ) & 0xFF;
    BYTE PAGEWR = (g_spi_registers.SPI_CmdValue2 >> 16 ) & 0xFF;
    BYTE BYTEWR = (g_spi_registers.SPI_CmdValue2 >> 24 ) & 0xFF;

    printf("SPIx18 - CmdVal2\n");
    printf("  - Read: %02x\n", Read);
    printf("  - FRead: %02x\n", FRead);
    printf("  - PAGEWR: %02x\n", PAGEWR);
    printf("  - BYTEWR: %02x\n", BYTEWR);
}

void print_SPI_x1D() {
    ALT_SPI_CS *pAltSpiCs = (ALT_SPI_CS *) & g_spi_registers.Alt_SPI_CS;
    printf("SPIx1D - Alt_SPI_CS\n");
    printf("  - lock_spi_cs: %d - ", pAltSpiCs->lock_spi_cs);
    if (pAltSpiCs->lock_spi_cs == 0) {
        print_red("FAILED\n");
    } else {
        print_green("OK\n");
    }
    printf("  - SpiProtectEn0: %d - ", pAltSpiCs->SpiProtectEn0);
    if (pAltSpiCs->SpiProtectEn0 == 0) {
        print_red("FAILED\n");
    } else {
        print_green("OK\n");
    }
    printf("  - SpiProtectEn1: %d - ", pAltSpiCs->SpiProtectEn1);
    if (pAltSpiCs->SpiProtectEn1 == 0) {
        print_red("FAILED\n");
    } else {
        print_green("OK\n");
    }
    printf("  - SpiProtectLock: %d - ", pAltSpiCs->SpiProtectLock);
    if (pAltSpiCs->SpiProtectLock == 0) {
        print_red("FAILED\n");
    } else {
        print_green("OK\n");
    }
    printf("  - AltSpiCsEn: %d\n", pAltSpiCs->AltSpiCsEn);
}

void load_spi_information(DWORD spi_addr, bool new_chipset) {
    g_spi_addr = spi_addr;
    bNewChipset = new_chipset;

    read_physical_memory(g_spi_addr, sizeof(SPI), &g_spi_registers, false);

//    SPI_WRITE_ENABLE_OP  = g_spi_registers.SPI_CmdValue1 & 0xFF;
    SPI_WRITE_ENABLE_OP  = 0x06;
    SPI_WRITE_DISABLE_OP = (g_spi_registers.SPI_CmdValue1 >> 8 ) & 0xFF;
    SPI_RDID_OP          = (g_spi_registers.SPI_CmdValue1 >> 16 ) & 0xFF;
    SPI_WRITE_BYTE_OP    = (g_spi_registers.SPI_CmdValue2 >> 24 ) & 0xFF;
    SPI_READ_OP          = g_spi_registers.SPI_CmdValue2 & 0xFF;
    SPI_WRITE32_OP       = 0x12;
    SPI_RDID_OP          = 0x13;
    SPI_READ_STATUS_REG1_OP = 0x05;
    
    g_flash_id = amd_spi_read_id();

    debug_print("Flash ID: %08x\n", g_flash_id);

    if (bNewChipset) {
        AMD_FLASH_SIZE = 64 * 1024 * 1024;
        AMD_FLASH_BASE = AMD_DEFAULT_NEW_MAPPED_FLASH_ADDRESS;
    } else {
        AMD_FLASH_SIZE = 16 * 1024 * 1024;
        AMD_FLASH_BASE = AMD_DEFAULT_OLD_MAPPED_FLASH_ADDRESS;
    }

    // @todo Redefine FLASH Size based on FlashID
    switch(g_flash_id) {
        case Winbond_25Q128JVS:
        case Winbond_W25Q64FW:
            SPI_SECTOR_ERASE_OP = Winbond_25Q128JVS_SECTOR_ERASE_OP;
            break;
        default:
            break;
    }

    if (SPI_SECTOR_ERASE_OP == 0) {
        printf("warning: SPI_ERASE could not be found!\n");
    }
    //printf("SPI_SECTOR_ERASE_OP: %02x\n", SPI_SECTOR_ERASE_OP);
    bSpiInformationInitialized = 1;
}


void print_spi_ctnl_info() {

    if (bSpiInformationInitialized == 0) {
        printf("err: amd_spi - SPI_ADDR not set!\n");
        exit(-1);
    }

    read_physical_memory(g_spi_addr, sizeof(SPI), &g_spi_registers, false);

    printf("SPI BASE: %lx\n", g_spi_addr);

    // SPIx00 SPI_Cntrl0
    print_SPI_Cntrl0();

    // SPIx04, SPIx08 RestrictedCmd
    print_SPI_RestrictedCmds();

    print_SPI_Cntrl1();

    print_SPI_CmdValue0();
    print_SPI_CmdValue1();
    print_SPI_CmdValue2();

    // SPIx1D
    print_SPI_x1D();

}

void print_flash_info() {
    /*
    LPC_ROM_ADDRESS lpc_rom_addr_range1 = {0};
    lpc_rom_addr_range1.StartAddr = (DWORD)g_lpc_isa_bridge_registers.LPC_ROM_Address_Range_1_StartAddress << 16;
    lpc_rom_addr_range1.EndAddr = ((DWORD)g_lpc_isa_bridge_registers.LPC_ROM_Address_Range_1_EndAddress << 16) | 0xffff;

    LPC_ROM_ADDRESS lpc_rom_addr_range2 = {0};
    lpc_rom_addr_range2.StartAddr = (DWORD)g_lpc_isa_bridge_registers.LPC_ROM_Address_Range_2_StartAddress << 16;
    lpc_rom_addr_range2.EndAddr = ((DWORD)g_lpc_isa_bridge_registers.LPC_ROM_Address_Range_2_EndAddress << 16) | 0xffff;

    printf("LPC ROM Address Range1 Start: %lx\n", lpc_rom_addr_range1.StartAddr);
    printf("LPC ROM Address Range1   End: %lx\n", lpc_rom_addr_range1.EndAddr);
    printf("LPC ROM Address Range2 Start: %lx\n", lpc_rom_addr_range2.StartAddr);
    printf("LPC ROM Address Range2   End: %lx\n", lpc_rom_addr_range2.EndAddr);
    */
    printf("Flash Base: %016lx\n", AMD_FLASH_BASE);
    printf("Flash Size: %08x\n", AMD_FLASH_SIZE);
}


void amd_spi_clear_fifo_ptr(volatile SPI *spi_base) {
    // DWORD SPI_Cntrl0  = spi_base->SPI_Cntrl0Value;
	// SPI_Cntrl0 |= 1<<20;
	// spi_base->SPI_Cntrl0Value = SPI_Cntrl0;
    if (bNewChipset == false) {
        spi_base->SPI_Cntrl0.FifoPtrClr = 1;
        while (spi_base->SPI_Cntrl1.FifoPtr != 0);
    }
}

void amd_spi_execute_command(volatile SPI *spi_base) {
    spi_base->CmdTrig = 0xff;
	while((spi_base->CmdTrig & 0x80) != 0);
	while(spi_base->SpiStatus.SpiBusy);
    while(spi_base->SPI_Cntrl0.SpiBusy);
}

void amd_spi_write_enable(volatile SPI* spi_base) {
    amd_spi_clear_fifo_ptr(spi_base);
    
    spi_base->CmdCode = SPI_WRITE_ENABLE_OP; // WREN
	// Set RX Byte Count
	spi_base->RxByteCnt = 0;
	spi_base->TxByteCnt = 0;
	amd_spi_execute_command(spi_base);
}

void amd_spi_print_fifo_stats(volatile SPI *spi_base) {
    printf("    FifoRdPtr: %08x\n", spi_base->SpiStatus.FifoRdPtr);
	printf("    FifoWrPtr: %08x\n", spi_base->SpiStatus.FifoWrPtr);
	printf("  DoneByCount: %08x\n", spi_base->SpiStatus.DoneByCount);
    printf("IllegalAccess: %02x\n", spi_base->SPI_Cntrl0.IllegalAccess);
}

DWORD amd_spi_read_id() {
    volatile SPI *spi_base = (volatile SPI *) map_physical_memory(g_spi_addr, PAGE_SIZE);
    amd_spi_clear_fifo_ptr(spi_base);
    spi_base->CmdCode = 0x9F; // TODO, FIX THIS
	// Set RX Byte Count
	spi_base->RxByteCnt = 3;
	spi_base->TxByteCnt = 0;
    amd_spi_execute_command(spi_base);

    DWORD flash_id = (DWORD)spi_base->mode24.SPI_regx80 << 16;
    flash_id |= (DWORD)spi_base->mode24.SPI_regx81 << 8;
    flash_id |= (DWORD) spi_base->mode24.SPI_regx82;


    unmap_physical_memory((void *) spi_base, PAGE_SIZE);

    return flash_id;
}

void amd_spi_erase_4k_block(volatile SPI *spi_base_arg, UINT32 address) {
    if (address > AMD_FLASH_SIZE) 
    {
        printf("invalid parameters for read_from_flash_index_mode\n");
        return;
    }

    debug_print("Erasing flash address: 0x%lx\n", address);

    volatile SPI *spi_base;
    
    if (spi_base_arg) {
        spi_base = spi_base_arg;
    } else {
        spi_base = (volatile SPI *) map_physical_memory(g_spi_addr, PAGE_SIZE);
    }

    amd_spi_write_enable(spi_base);

    amd_spi_clear_fifo_ptr(spi_base);
    
    if (bNewChipset) {
        spi_base->mode32.SPI_regx80 = (address >> 24) & 0xFF;
        spi_base->mode32.SPI_regx81 = (address >> 16) & 0xFF;
        spi_base->mode32.SPI_regx82 = (address >> 8) & 0xFF;
        spi_base->mode32.SPI_regx83 = (address >> 0) & 0xFF;

        spi_base->CmdCode = SPI_SECTOR_ERASE_OP;

        // Set RX Byte Count
        spi_base->RxByteCnt = 0;

        // Set Tx Byte Count
        spi_base->TxByteCnt = 4;
    } else {
        // Write address to read from into FIFO
        spi_base->SPI_Cntrl1.SpiParamters = (address >> 16) & 0xFF;
        spi_base->SPI_Cntrl1.SpiParamters = (address >> 8) & 0xFF;
        spi_base->SPI_Cntrl1.SpiParamters = address & 0xFF;

        spi_base->CmdCode = SPI_SECTOR_ERASE_OP;

        // Set RX Byte Count
        spi_base->RxByteCnt = 0;

        // Set Tx Byte Count
        spi_base->TxByteCnt = 3;
    }  
    

    amd_spi_execute_command(spi_base);

    if (!spi_base_arg) {
        unmap_physical_memory((void *) spi_base, PAGE_SIZE);
    }
}


void read_block_index_mode24(volatile SPI *spi_base, UINT32 addr, BYTE *block) {
    /*
    * Reads 64 byte block from flash
    */

    amd_spi_clear_fifo_ptr(spi_base);

    // Write address to read from into FIFO
    spi_base->SPI_Cntrl1.SpiParamters = (addr >> 16) & 0xFF;
	spi_base->SPI_Cntrl1.SpiParamters = (addr >> 8) & 0xFF;
	spi_base->SPI_Cntrl1.SpiParamters = addr & 0xFF;

    // Set Read Opcode
	spi_base->CmdCode = SPI_READ_OP;

	// Set TX Byte Count (address is 24 bits)
	spi_base->TxByteCnt = 3;

    // Set RX for the block
    spi_base->RxByteCnt = SPI_INDEX_MODE_READ_BLOCK_SIZE;
	
	amd_spi_execute_command(spi_base);

    memcpy(block, (void *) &spi_base->mode24.FIFO[0], SPI_INDEX_MODE_READ_BLOCK_SIZE);
}

void read_block_index_mode32(volatile SPI *spi_base, UINT32 addr, BYTE *block) {
    /*
    * Reads 64 byte block from flash
    */

    amd_spi_clear_fifo_ptr(spi_base);

    // Write address to read from into SPI_Regx
    spi_base->mode32.SPI_regx80 = (addr >> 24) & 0xFF;
	spi_base->mode32.SPI_regx81 = (addr >> 16) & 0xFF;
	spi_base->mode32.SPI_regx82 = (addr >> 8) & 0xFF;
	spi_base->mode32.SPI_regx83 = (addr >> 0) & 0xFF;


    // Set Read Opcode
	spi_base->CmdCode = SPI_READ_OP;

	// Set TX Byte Count (address is 32 bits)
	spi_base->TxByteCnt = 4;

    // Set RX for the block
    spi_base->RxByteCnt = SPI_INDEX_MODE_READ_BLOCK_SIZE;
	
	amd_spi_execute_command(spi_base);

    memcpy(block, (void *) &spi_base->mode32.FIFO[0], SPI_INDEX_MODE_READ_BLOCK_SIZE);
}

void read_from_flash_index_mode(volatile SPI *spi_base_arg, UINT32 start_offset, 
    UINT32 length, BYTE *out_buff) 
{

    if (bSpiInformationInitialized == 0) {
        debug_print("SPI Information not retrieved: cannot read from SPI flash - index mode\n");
        return;
    }

    if (start_offset > AMD_FLASH_SIZE
     || (start_offset + length) > AMD_FLASH_SIZE
     || (start_offset + length) < start_offset
     || out_buff == NULL) 
    {
        printf("invalid parameters for read_from_flash_index_mode\n");
        return;
    }

    volatile SPI *spi_base;
    if (spi_base_arg) {
        spi_base = spi_base_arg;
    } else {
	    spi_base = (volatile SPI *) map_physical_memory(g_spi_addr, PAGE_SIZE);
	}

    int num_blocks = length >> SPI_INDEX_MODE_READ_BLOCK_BITS;
    num_blocks += (length & SPI_INDEX_MODE_READ_BLOCK_MASK) ? 1 : 0;

    // align start address
    UINT32 aligned_addr = start_offset & ~SPI_INDEX_MODE_READ_BLOCK_MASK;
    UINT32 offset       = start_offset & SPI_INDEX_MODE_READ_BLOCK_MASK;

    BYTE block[SPI_INDEX_MODE_READ_BLOCK_SIZE];
    
    BYTE *pBuff = out_buff;

    // Remaining length
    UINT32 rlength = length;

    //debug_print("Processing %d blocks\n", num_blocks);
    for (int i = 0 ; i < num_blocks; i++) {
        UINT32 addr = aligned_addr + i * SPI_INDEX_MODE_READ_BLOCK_SIZE;
        //memset(block, 0x00, SPI_INDEX_MODE_READ_BLOCK_SIZE);
        if (bNewChipset) {
            read_block_index_mode32(spi_base, addr, block);
        } else {
            read_block_index_mode24(spi_base, addr, block);
        }
        
        debug_print("reading block:[%d] - addr: %08x\n", i, addr);

        // Handle first block
        if (i == 0) {
            memcpy(pBuff, &block[offset], SPI_INDEX_MODE_READ_BLOCK_SIZE - offset);        
            pBuff += SPI_INDEX_MODE_READ_BLOCK_SIZE - offset;
            rlength -= SPI_INDEX_MODE_READ_BLOCK_SIZE - offset;
            continue;
        }

        // Handle last block
        if (i == num_blocks - 1) {
            memcpy(pBuff, block, rlength);
            pBuff += rlength;
            continue;
        }

        // Handle middle blocks
        memcpy(pBuff, block, SPI_INDEX_MODE_READ_BLOCK_SIZE);
        pBuff += SPI_INDEX_MODE_READ_BLOCK_SIZE;
        rlength -= SPI_INDEX_MODE_READ_BLOCK_SIZE;
    }

    if ( (UINT32 )(pBuff - out_buff) != length) {
        printf("Error during SPI block copy\n");
    }

    if (!spi_base_arg) {
	    unmap_physical_memory((void *) spi_base, PAGE_SIZE);
    }
}

void amd_dump_spi_flash_index_mode(const char *output_filename) {

    char *rom_data  = (char *) calloc(1, AMD_FLASH_SIZE);
    read_from_flash_index_mode(NULL, 0, AMD_FLASH_SIZE, (BYTE *) rom_data);

    FILE *f = fopen(output_filename, "wb");
    fwrite(rom_data, AMD_FLASH_SIZE, 1, f);
    fclose(f);

    free(rom_data);
}


void write_block_index_mode24(volatile SPI *spi_base, UINT32 addr, BYTE *block) {
    /*
    * Writes a 8 byte block into flash
    */

    //doSleep(200);
    while(amd_spi_check_busy(spi_base)) {
        doSleep(1);
    }

    amd_spi_write_enable(spi_base);

    amd_spi_clear_fifo_ptr(spi_base);

    // Write address to read from into FIFO
    spi_base->SPI_Cntrl1.SpiParamters = (addr >> 16) & 0xFF;
	spi_base->SPI_Cntrl1.SpiParamters = (addr >> 8) & 0xFF;
	spi_base->SPI_Cntrl1.SpiParamters = addr & 0xFF;
    

    for (int i = 0; i < SPI_INDEX_MODE_WRITE_BLOCK_FIFO; i++) {
        spi_base->SPI_Cntrl1.SpiParamters = block[i];
    }

    // Set Write Opcode
	spi_base->CmdCode = SPI_WRITE_BYTE_OP;

	// Set TX Byte Count (address is 24 bits)
	spi_base->TxByteCnt = 3 + SPI_INDEX_MODE_WRITE_BLOCK_FIFO;

    // Set RX for the block
    spi_base->RxByteCnt = 0;
	
	amd_spi_execute_command(spi_base);

    debug_print("Writing %d bytes into address: %08x\n", SPI_INDEX_MODE_WRITE_BLOCK_FIFO, addr);
    
}

void write_block_index_mode32(volatile SPI *spi_base, UINT32 addr, BYTE *block) {
    /*
    * Writes a 8 byte block into flash
    */

    //doSleep(125);
    while(amd_spi_check_busy(spi_base)) {
        doSleep(1);
    }

    amd_spi_write_enable(spi_base);

    amd_spi_clear_fifo_ptr(spi_base);

    // Write address to read from into FIFO
    spi_base->mode32.SPI_regx80 = (addr >> 24) & 0xFF;
	spi_base->mode32.SPI_regx81 = (addr >> 16) & 0xFF;
	spi_base->mode32.SPI_regx82 = (addr >> 8) & 0xFF;
	spi_base->mode32.SPI_regx83 = (addr >> 0) & 0xFF;

    for (int i = 0; i < SPI_INDEX_MODE_WRITE_BLOCK_FIFO; i++) {
        spi_base->mode32.FIFO[i] = block[i];
    }

    // Set Write Opcode
	spi_base->CmdCode = SPI_WRITE32_OP;

	// Set TX Byte Count (address is 32 bits)
	spi_base->TxByteCnt = 4 + SPI_INDEX_MODE_WRITE_BLOCK_FIFO;

    // Set RX for the block
    spi_base->RxByteCnt = 0;
	
	amd_spi_execute_command(spi_base);

    debug_print("Writing %d bytes into address: %08x\n", SPI_INDEX_MODE_WRITE_BLOCK_FIFO, addr);
    
}


void write_4k_block(volatile SPI *spi_base, UINT32 addr, BYTE *block) {
    // This function simply calls write_block_index_mode in blocks of 8 bytes
    // 4K / 8 => 512 calls
    UINT32 iterations = SPI_INDEX_MODE_WRITE_BLOCK_4K / SPI_INDEX_MODE_WRITE_BLOCK_FIFO;
    for (int i = 0; i < iterations; i++) {
        UINT32 cur_addr = addr + i * SPI_INDEX_MODE_WRITE_BLOCK_FIFO;
        if (bNewChipset) {
            write_block_index_mode32(
                spi_base,
                cur_addr,
                &block[i * SPI_INDEX_MODE_WRITE_BLOCK_FIFO]
            );
        } else {
            write_block_index_mode24(
                spi_base,
                cur_addr,
                &block[i * SPI_INDEX_MODE_WRITE_BLOCK_FIFO]
            );
        }
    }
}

void amd_spi_write_buffer(volatile SPI *spi_base_arg, UINT32 flash_address, BYTE *in_buff, UINT32 in_length) 
{
    if (bSpiInformationInitialized == 0) {
        debug_print("SPI Information not retrieved: cannot write to SPI flash - index mode\n");
        return;
    }

    if (flash_address > AMD_FLASH_SIZE
     || (flash_address + in_length) > AMD_FLASH_SIZE
     || (flash_address + in_length) < flash_address
     || in_buff == NULL) 
    {
        printf("invalid parameters for amd_spi_write_buffer\n");
        return;
    }

    volatile SPI *spi_base;
    if (spi_base_arg) {
        spi_base = spi_base_arg;
    } else {
	    spi_base = (volatile SPI *) map_physical_memory(g_spi_addr, PAGE_SIZE);
	}

    // Write happens in blocks of 4KB
    int num_blocks = in_length >> SPI_INDEX_MODE_WRITE_BLOCK_BITS;
    num_blocks += (in_length & SPI_INDEX_MODE_WRITE_BLOCK_MASK) ? 1 : 0;

    // align start address
    UINT32 aligned_addr = flash_address & ~SPI_INDEX_MODE_WRITE_BLOCK_MASK;
    UINT32 offset       = flash_address & SPI_INDEX_MODE_WRITE_BLOCK_MASK;

    BYTE *block = (BYTE *) malloc(SPI_INDEX_MODE_WRITE_BLOCK_4K);

    BYTE *pBuff = in_buff;

    // Remaining length
    UINT32 rlength = in_length;

    // Read the content of the block first
    for (int i = 0; i < num_blocks; i++) {
        UINT32 addr = aligned_addr + i * SPI_INDEX_MODE_WRITE_BLOCK_4K;
        read_from_flash_index_mode(spi_base, addr, SPI_INDEX_MODE_WRITE_BLOCK_4K, block);

        // Apply changes in the block

        // Handle first block
        if (i == 0) {
            if (in_length < (SPI_INDEX_MODE_WRITE_BLOCK_4K - offset)) {
                memcpy(&block[offset], pBuff, in_length);
                pBuff += in_length;
                rlength -= in_length;
            } else {
                memcpy(&block[offset], pBuff, SPI_INDEX_MODE_WRITE_BLOCK_4K - offset);
                pBuff += SPI_INDEX_MODE_WRITE_BLOCK_4K - offset;
                rlength -= SPI_INDEX_MODE_WRITE_BLOCK_4K - offset;
            }
        } 
        // Handle last block
        else if (i == num_blocks - 1) {  
            memcpy(block, pBuff, rlength);
            pBuff += rlength;
        } 
        // Handle middle blocks
        else { 
            memcpy(block, pBuff, SPI_INDEX_MODE_WRITE_BLOCK_4K);
            pBuff += SPI_INDEX_MODE_WRITE_BLOCK_4K;
            rlength -= SPI_INDEX_MODE_WRITE_BLOCK_4K;
        }

        // At this point, the block was modified, we now need to write it

        // Erase first
        amd_spi_erase_4k_block(spi_base, addr);
        
        // Write
        write_4k_block(spi_base, addr, block);
        //print_memory(0, (char *) block, SPI_INDEX_MODE_WRITE_BLOCK_4K);
        
    }

    if ( (UINT32 )(pBuff - in_buff) != in_length) {
        printf("Error during SPI block copy\n");
    }

    //////

    free(block);

    if (!spi_base_arg) {
	    unmap_physical_memory((void *) spi_base, PAGE_SIZE);
    }
}