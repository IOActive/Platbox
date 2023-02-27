
#include "amd_chipset.h"
#include "global.h"
#include "pci.h"
#include "Util.h"
#include "physmem.h"
#include "msr.h"

LPC_ISA_Bridge g_lpc_isa_bridge_registers;
SPI g_spi_registers;
DWORD64 g_MmioCfgBaseAddr;


void _read_lpc_isa_bridge() {	
	memset(&g_lpc_isa_bridge_registers, 0x00, sizeof(g_lpc_isa_bridge_registers));
    DWORD *pw = (DWORD *) &g_lpc_isa_bridge_registers;
    for (int i = 0 ; i < sizeof(LPC_ISA_Bridge); i++) {
        *pw = read_pci_dword(0, 0x14, 3, i);
        pw++;
    }

    //print_memory(0, (char *) &g_lpc_isa_bridge_registers, sizeof(g_lpc_isa_bridge_registers));
    //getchar();
}

void _read_mmio_cfg_base_addr() {
    do_read_msr(AMD_MSR_MMIO_CONFIGURATION_BASE_ADDRRESS, &g_MmioCfgBaseAddr);
    g_pci_ecam.physaddr = g_MmioCfgBaseAddr & 0xfff0000000;
    g_pci_ecam.vaddr    = 0;

    UINT bus_range = ((g_MmioCfgBaseAddr >> 2) & 0b1111);
    switch(bus_range) {
        case 0:
            g_pci_ecam.size = 1024 * 1024;
            break;
        case 1:
            g_pci_ecam.size = 1024 * 1024 * 2;
            break;
        case 2:
            g_pci_ecam.size = 1024 * 1024 * 4;
            break;
        case 3:
            g_pci_ecam.size = 1024 * 1024 * 8;
            break;
        case 4:
            g_pci_ecam.size = 1024 * 1024 * 16;
            break;
        case 5:
            g_pci_ecam.size = 1024 * 1024 * 32;
            break;
        case 6:
            g_pci_ecam.size = 1024 * 1024 * 64;
            break;
        case 7:
            g_pci_ecam.size = 1024 * 1024 * 128;
            break;
        case 8:
            g_pci_ecam.size = 1024 * 1024 * 256;
            break;
    }
    
}



void amd_print_smm_base() {
    DWORD64 smm_base = 0;
    do_read_msr(AMD_MSR_SMM_BASE_ADDRESS, &smm_base);

    printf("MSR C001_0111 SMM Base Address (SMM_BASE)\n");
	printf(" => Base: %lx\n", smm_base);
	printf("   -> SMI-Handler Entry Point: %lx\n", smm_base + AMD_SMI_HANDLER_ENTRY_POINT);
    printf("   -> SMM Save-State Area    : %lx\n", smm_base + AMD_SMM_STATE_SAVE_AREA);
    printf("\n");
}

void amd_print_smm_tseg_addr() {    
    DWORD64 smm_tseg_base = 0;
    UINT32  tseg_size = 0;

    get_tseg_info(&smm_tseg_base, &tseg_size);

    printf("MSR C001_0112 SMM TSeg Base Address (SMMAddr)\n");
	printf(" => Base : %08lx\n", smm_tseg_base);
    printf(" => Limit: %08lx\n", smm_tseg_base + tseg_size);
    printf("\n");	
}

void amd_print_smm_tseg_mask() {    
    DWORD64 smm_tseg_mask = 0;
    do_read_msr(AMD_MSR_SMM_TSEG_MASK, &smm_tseg_mask);

    printf("MSR C001_0113 SMM TSeg Mask (SMMMask)\n");
	printf(" => Value: %016llx\n", smm_tseg_mask);	
    printf("   -> TSegMask: %016llx\n", smm_tseg_mask & 0xfffffe0000); // TSegMask[39:17]
    printf("   -> TMTypeDram: %d\n", (smm_tseg_mask >> 12) & 0b111);
    printf("   -> AMTypeDram: %d\n", (smm_tseg_mask >> 8) & 0b111);
    printf("   -> TMTypeIoWc: %d\n", (smm_tseg_mask >> 5) & 1);
    printf("   -> AMTypeIoWc: %d\n", (smm_tseg_mask >> 4) & 1);
    printf("   -> TClose: %d\n", (smm_tseg_mask >> 3) & 1);
    printf("   -> AClose: %d\n", (smm_tseg_mask >> 2) & 1);
    printf("   -> TValid: %d\n", (smm_tseg_mask >> 1) & 1);
    printf("   -> AValid: %d\n", (smm_tseg_mask >> 0) & 1);
    printf("\n");
}

void get_tseg_info(UINT64 *base, UINT32 *size) {
    DWORD64 smm_tseg_base = 0;
    DWORD64 smm_tseg_mask = 0;

    do_read_msr(AMD_MSR_SMM_TSEG_BASE_ADDRESS, &smm_tseg_base);
    do_read_msr(AMD_MSR_SMM_TSEG_MASK, &smm_tseg_mask);

    *base = smm_tseg_base; 
    UINT32 tseg_mask = (UINT32) (smm_tseg_mask & 0xfffffe0000);
    *size = ~tseg_mask; 
}


void amd_print_apic_bar() {
    DWORD64 apic_bar = 0;
    do_read_msr(AMD_MSR_APIC_BAR, &apic_bar);

    printf("MSR 0000_001B APIC Base Address (APIC_BAR)\n");
	printf(" => Value: %lx\n", apic_bar);
	printf("   -> BA: %lx\n", apic_bar & 0xFFFFFFFFFFFFF000);
    printf("   -> Enabled: %d\n", (apic_bar >> 11) & 1);
    printf("   -> BSC: %d\n", (apic_bar >> 8) & 1);
    printf("\n");
}

void amd_print_hwcr_smmlock() {
    DWORD64 hwcr = 0;
    do_read_msr(AMD_MSR_HARDWARE_CONFIGURATION, &hwcr);

    int smm_lock = hwcr & BIT_MSR_HWCR_SMMLOCK;
    printf("MSR C001_0015 Hardware Configuration (HWCR)\n");
	printf(" => Value: %lx\n", hwcr);
	printf("   -> SMMLock: %d - ", smm_lock != 0 ? 1 : 0);
    if (smm_lock == 0) {
        print_red("FAILED\n");
    } else {
        print_green("OK\n");
    }
    printf("\n");
}

void amd_print_smi_on_io_trap_configuration() {
   
    DWORD64 smi_io_trap = 0;
    do_read_msr(AMD_MSR_SMI_ON_IO_TRAP_0, &smi_io_trap);

    printf("MSR C001_0050 IO Trap (SMI_ON_IO_TRAP_0])\n");
	printf(" => Value: %lx\n", smi_io_trap);
	printf("   -> SmiOnRdEn: %d\n", (smi_io_trap >> 63) & 1);
    printf("   -> SmiOnWrEn: %d\n", (smi_io_trap >> 62) & 1);
    printf("   -> ConfigSmi: %d\n", (smi_io_trap >> 61) & 1);
    printf("   -> SmiMask: %08x\n", (smi_io_trap >> 32) & 0xffffff);
    printf("   -> SmiAddr: %08x\n", (smi_io_trap) & 0xFFFFFFFF);

    smi_io_trap = 0;
    do_read_msr(AMD_MSR_SMI_ON_IO_TRAP_1, &smi_io_trap);

    printf("MSR C001_0051 IO Trap (SMI_ON_IO_TRAP_1])\n");
	printf(" => Value: %lx\n", smi_io_trap);
	printf("   -> SmiOnRdEn: %d\n", (smi_io_trap >> 63) & 1);
    printf("   -> SmiOnWrEn: %d\n", (smi_io_trap >> 62) & 1);
    printf("   -> ConfigSmi: %d\n", (smi_io_trap >> 61) & 1);
    printf("   -> SmiMask: %08x\n", (smi_io_trap >> 32) & 0xffffff);
    printf("   -> SmiAddr: %08x\n", (smi_io_trap) & 0xFFFFFFFF);

    smi_io_trap = 0;
    do_read_msr(AMD_MSR_SMI_ON_IO_TRAP_2, &smi_io_trap);

    printf("MSR C001_0052 IO Trap (SMI_ON_IO_TRAP_2])\n");
	printf(" => Value: %lx\n", smi_io_trap);
	printf("   -> SmiOnRdEn: %d\n", (smi_io_trap >> 63) & 1);
    printf("   -> SmiOnWrEn: %d\n", (smi_io_trap >> 62) & 1);
    printf("   -> ConfigSmi: %d\n", (smi_io_trap >> 61) & 1);
    printf("   -> SmiMask: %08x\n", (smi_io_trap >> 32) & 0xffffff);
    printf("   -> SmiAddr: %08x\n", (smi_io_trap) & 0xFFFFFFFF);

    smi_io_trap = 0;
    do_read_msr(AMD_MSR_SMI_ON_IO_TRAP_3, &smi_io_trap);

    printf("MSR C001_0053 IO Trap (SMI_ON_IO_TRAP_3])\n");
	printf(" => Value: %lx\n", smi_io_trap);
	printf("   -> SmiOnRdEn: %d\n", (smi_io_trap >> 63) & 1);
    printf("   -> SmiOnWrEn: %d\n", (smi_io_trap >> 62) & 1);
    printf("   -> ConfigSmi: %d\n", (smi_io_trap >> 61) & 1);
    printf("   -> SmiMask: %08x\n", (smi_io_trap >> 32) & 0xffffff);
    printf("   -> SmiAddr: %08x\n", (smi_io_trap) & 0xFFFFFFFF);

    printf("\n"); 
}

void amd_print_io_trap_control_status() {
    
    DWORD64 io_trap_ctl_sts = 0;
    do_read_msr(AMD_MSR_IO_TRAP_CTL_STS, &io_trap_ctl_sts);

    printf("MSR C001_0054 IO Trap Control (SMI_ON_IO_TRAP_CTL_STS)\n");
	printf(" => Value: %016lx\n", io_trap_ctl_sts);
	printf("   -> IoTrapEn: %d\n", (io_trap_ctl_sts >> 15) & 1);
    printf("   -> SmiEn3 (MSR C001_0053): %d\n", (io_trap_ctl_sts >> 7) & 1);
    printf("   -> SmiEn2 (MSR C001_0052): %d\n", (io_trap_ctl_sts >> 5) & 1);
    printf("   -> SmiEn1 (MSR C001_0051): %d\n", (io_trap_ctl_sts >> 3) & 1);
    printf("   -> SmiEn0 (MSR C001_0050): %d\n", (io_trap_ctl_sts >> 1) & 1);
    printf("\n");
}

void amd_print_mmio_ba() {
    DWORD64 mmio_ba = 0;
    do_read_msr(AMD_MSR_MMIO_CONFIGURATION_BASE_ADDRRESS, &mmio_ba);

    printf("MSR C001_0058 MMIO Configuration Base Address\n");
	printf(" => Value: %016lx\n", mmio_ba);
	printf("   -> MmioCfgBaseAddr: %016lx\n", (mmio_ba & 0xfff0000000));
    printf("   -> BusRange: %d\n", (mmio_ba >> 2) & 0b1111);
    printf("   -> Enable: %d\n", (mmio_ba >> 0) & 1);
    printf("\n");
}



void amd_print_root_complex_pci_control() {
	// D0F0x4C PCI Control
	DWORD pci_control = read_pci_dword(0, 0, 0, ROOT_COMPLEX_PCI_CONTROL_OFFSET);
    int mmio_enable = (pci_control >> 23) & 1;
    /*
        This range is used to
        create an MMIO hole in the DRAM address range used for DMA decoding. DMA writes that fall into
        the MMIO range are treated as potential p2p requests. 
    */
    printf("D0F0x4C PCI Control.MMIOEnable: %d\n", mmio_enable);
    if (mmio_enable) {
        printf(" -> MMIO Base/Limit pair is enabled\n");
    } else {
        printf(" -> MMIO Base/Limit pair is disabled\n");
    }

    printf("\n");
}



void amd_print_root_complex_nb_control() {
	// D0F0x64_x00 Northbridge Control 
    write_pci_dword(0, 0, 0, ROOT_COMPLEX_MISC_INDEX, ROOT_COMPLEX_MISC_INDEX_OFFSET_NORTHBRIDGE_CONTROL);
	DWORD nb_control = read_pci_dword(0, 0, 0, ROOT_COMPLEX_MISC_INDEX_DATA);

    int HwInitWrLock = (nb_control >> 7) & 1;

    printf("D0F0x64_x00 Northbridge Control.HwInitWrLock: %d - ", HwInitWrLock);
    if (HwInitWrLock == 0) {
        print_red("FAILED\n");
    } else {
        print_green("OK\n");
    }

    printf("\n");
}

void amd_print_root_complex_mmio_range() {
	// D0F0x64_x17 Memory Mapped IO Base Address 
    // D0F0x64_x18 Memory Mapped IO Limit
	write_pci_dword(0, 0, 0, ROOT_COMPLEX_MISC_INDEX, ROOT_COMPLEX_MISC_INDEX_OFFSET_MMIO_BASE);
	DWORD mmio_base = read_pci_dword(0, 0, 0, ROOT_COMPLEX_MISC_INDEX_DATA);

    write_pci_dword(0, 0, 0, ROOT_COMPLEX_MISC_INDEX, ROOT_COMPLEX_MISC_INDEX_OFFSET_MMIO_LIMIT);
	DWORD mmio_limit = read_pci_dword(0, 0, 0, ROOT_COMPLEX_MISC_INDEX_DATA);

    printf("D0F0x64_x17 Memory Mapped IO Base Address: %08x\n", mmio_base);
    printf("D0F0x64_x18 Memory Mapped IO Limit: %08x\n", mmio_limit);

    printf("\n");
}

void amd_print_root_complex_tom2() {
	// D0F0x64_x19 Top of Memory 2 Low
    // D0F0x64_x1A Top of Memory 2 High
	write_pci_dword(0, 0, 0, ROOT_COMPLEX_MISC_INDEX, ROOT_COMPLEX_MISC_INDEX_OFFSET_TOM2_LOW);
	DWORD tom2_low = read_pci_dword(0, 0, 0, ROOT_COMPLEX_MISC_INDEX_DATA);

    write_pci_dword(0, 0, 0, ROOT_COMPLEX_MISC_INDEX, ROOT_COMPLEX_MISC_INDEX_OFFSET_TOM2_HIGH);
	DWORD tom2_high = read_pci_dword(0, 0, 0, ROOT_COMPLEX_MISC_INDEX_DATA);

    DWORD64 tom2 = ((DWORD64)tom2_high << 32) | ((DWORD64) tom2_low);
    int enabled = tom2 & 1;
    tom2 = tom2 & 0xFFFFFFFFFFFFFFE;

    printf("D0F0x64_x19-1A Top of Memory 2: %016lx\n", tom2);
    printf("  -> Enabled: %d\n", enabled);
    printf("\n");
}

void amd_print_msr_tom2() {    
    DWORD64 tom2 = 0;
    do_read_msr(AMD_MSR_TOP_OF_MEMORY_2, &tom2);

    printf("MSR C001_001D Top Of Memory 2 (TOM2)\n");
	printf(" => Value: %016llx\n", tom2);
    printf("\n");
}


void amd_print_root_complex_tom() {
	// D0F0x90 Northbridge Top of Memory

	DWORD tom = read_pci_dword(0, 0, 0, ROOT_COMPLEX_NORTHBRIDGE_TOP_OF_MEMORY_OFFSET);
    /*
        TopOfDram. Read-write. BIOS: MSR C001_001A[TOM[31:23]]. Specifies the address that divides
        between MMIO and DRAM. From TopOfDram to 4G is MMIO; below TopOfDram is DR
    */
    printf("D0F0x90 Northbridge Top of Memory: %08x\n", tom);   
    printf("\n");
}

void amd_print_msr_tom() {    
    DWORD64 tom = 0;
    do_read_msr(AMD_MSR_TOP_OF_MEMORY, &tom);

    printf("MSR C001_001A Top Of Memory (TOM)\n");
	printf(" => Value: %016llx\n", tom);
    printf("\n");
}


void amd_print_dram_range_base_limit() {
    // D18F1x[17C:140,7C:40] DRAM Base/Limit
    // 8 Ranges?
    printf("D18F1 DRAM Base/Limit\n");
    printf("  -> Only RANGE-0 should be in use. The rest should be reserved\n");
    //for (int i = 0; i < D18F1_DRAM_BASE_LIMIT__NUMBER_OF_RANGES; i++) {
    for (int i = 0; i < 1; i++) {
        DWORD base_low_offset   = D18F1_DRAM_BASE_LIMIT__BASE_LOW_START   + (8 * i);
        DWORD limit_low_offset  = D18F1_DRAM_BASE_LIMIT__LIMIT_LOW_START  + (8 * i);
        //DWORD base_high_offset  = D18F1_DRAM_BASE_LIMIT__BASE_HIGH_START  + (8 * i);
        //DWORD limit_high_offset = D18F1_DRAM_BASE_LIMIT__LIMIT_HIGH_START + (8 * i);

        printf("  => Range %d\n", i);
        
        DWORD base_low   = read_pci_dword(0, 0x18, 1, base_low_offset);
        DWORD limit_low  = read_pci_dword(0, 0x18, 1, limit_low_offset);
        //DWORD base_high  = read_pci_dword(h, 0, 0x18, 1, base_high_offset);
        //DWORD limit_high = read_pci_dword(h, 0, 0x18, 1, limit_high_offset);

        /*
        D18F1x[7:4][8,0] DRAM Base Low
            31:16 DramBase[39:24]: DRAM base address register bits[39:24].
            15:2 Reserved.
            1 WE: write enable. 1=Writes to this address range are enabled.
            0 RE: read enable. 1=Reads to this address range are enabled.

        D18F1x[7:4][C,4] DRAM Limit Low        
            31:16 DramLimit[39:24]: DRAM limit address register bits[39:24]. Reset: FCFFh.
            15:11 Reserved.
            10:8 Reserved.
            7:3 Reserved.
            2:0 DstNode: destination Node ID. Reset: 000b. Specifies the node that a packet is routed to if it is
                within the address range.
        */

        printf("     => BaseLow   : %08x\n", base_low);
        printf("       => WritesEnable: %d\n", (base_low >> 1) & 1);
        printf("       => ReadsEnable: %d\n", (base_low >> 0) & 1);
        printf("     => LimitLow : %08x\n", limit_low);
        printf("       => DstNode : %d\n", limit_low & 0b111);            
    }
}


void amd_print_mmio_base_limit_ranges() {

   printf("D18F1x[2CC:2A0,1CC:180,BC:80] MMIO Base/Limit\n");
   for (int i = 0; i < D18F1_MMIO_BASE_LIMIT__NUMBER_OF_RANGES; i++) {
        DWORD base_low_offset   = D18F1_MMIO_BASE_LIMIT__BASE_LOW_START   + (8 * i);
        DWORD limit_low_offset  = D18F1_MMIO_BASE_LIMIT__LIMIT_LOW_START  + (8 * i);

        printf("  => Range %d\n", i);
        
        DWORD base_low   = read_pci_dword(0, 0x18, 1, base_low_offset);
        DWORD limit_low  = read_pci_dword(0, 0x18, 1, limit_low_offset);

        printf("    => BaseLow   : %08x\n", base_low & 0xFFFFFF00);
        printf("      => Lock: %d\n", (base_low >> 3) & 1);
        printf("      => WritesEnable: %d\n", (base_low >> 1) & 1);
        printf("      => ReadsEnable:  %d\n", (base_low >> 0) & 1);
        printf("    => limit_low : %08x\n", limit_low & 0xFFFFFF00);                
        printf("      => NP (non-posted):  %d\n", (base_low >> 7) & 1);
        printf("      => DstSubLink:  %d\n", (base_low >> 6) & 1);
        printf("      => DstLink:  %d\n", (base_low >> 4) & 0b11);
        printf("      => DstNode:  %d\n", (base_low >> 0) & 0b111);
    }

    printf("\n");

}

void amd_print_dram_system_address_range() {
	// D18F1x120 DRAM Base System Address
    // D18F1x124 DRAM Limit System Address

	DWORD dram_base_system_address  = read_pci_dword(0, 0x18, 1, D18F1_DRAM_BASE_SYSTEM_ADDRESS_OFFSET);
    DWORD dram_limit_system_address = read_pci_dword(0, 0x18, 1, D18F1_DRAM_LIMIT_SYSTEM_ADDRESS_OFFSET);
    
    printf("D18F1x120 DRAM System Address Range\n");
    printf(" =>  Base: %08x\n", dram_base_system_address);
    printf(" => Limit: %08x\n", dram_limit_system_address);
    printf("\n");
}


void amd_print_dram_controller_range() {
	// D18F1x2[1,0][8,0] DRAM Controller Base
    // D18F1x2[1,0][C,4] DRAM Controller Limit

    /*
    The DRAM controller base and limit registers define a DRAM controller address range and specify the
    mapping of physical DRAM addresses to a DCT as selected by DctSel or DctIntLvEn.
    Only DCT 0 is available for now. Only Range0

    Function Base Address Limit Address
    Range 0  D18F1x200    D18F1x204
    */


	DWORD dram_controller_base  = read_pci_dword(0, 0x18, 1, D18F1_DRAM_CONTROLLER_BASE_RANGE_0_OFFSET);
    DWORD dram_controller_limit = read_pci_dword(0, 0x18, 1, D18F1_DRAM_CONTROLLER_LIMIT_RANGE_0_OFFSET);
    
    printf("D18F1x2[1C:00] DRAM Controller Base/Limit\n");
    printf(" => Base: %08x\n", dram_controller_base & 0xFFFFF000);
    printf("   -> DctSel (DRAM Controller Select): %d\n", (dram_controller_base >> 4) & 0b111);
    printf("   -> LgcyMmioHoleEn: %d\n", (dram_controller_base >> 1) & 1);
    printf("   -> DctAddrVal: %d\n", dram_controller_base & 1);
    printf(" => Limit: %08x\n", dram_controller_limit);
    
    printf("\n");
}

void amd_print_dram_hole() {
    // D18F1xF0 DRAM Hole Address

    /*
    Bits Description
        31:24 DramHoleBase[31:24]: DRAM hole base address. Specifies the base address of the IO hole, below
            the 4GB address level, that is used in memory hoisting. Normally, DramHoleBase >=
            MSRC001_001A[TOM[31:24]].
        23:16 Reserved.
        15:7 DramHoleOffset[31:23]: DRAM hole offset address. When D18F1x2[1,0][8,0][LgcyMmioHoleEn]==1, this offset is subtracted from the physical address of certain accesses in forming the normalized address.
        6:3 Reserved.
        2 Reserved.
        1 DramMemHoistValid: dram memory hoist valid. 1=Memory hoisting for the address range is
            enabled. 0=Memory hoisting is not enabled. This bit should be set if any D18F1x2[1,0][8,0][LgcyMmioHoleEn]==1 or DramHoleBase != 0.
        0 DramHoleValid: dram hole valid. 1=Memory hoisting is enabled in the node. 0=Memory hoisting is
            not enabled. This bit should be set in the node that owns the DRAM address space that is hoisted
            above the 4 GB address level. See DramHoleBase.
    */
   

    DWORD dram_hole_address  = read_pci_dword(0, 0x18, 1, D18F1_DRAM_HOLE_ADDRESS_OFFSET);
    
    
    printf("D18F1xF0 DRAM Hole Address\n");
    printf(" => DramHoleBase: %08x\n", dram_hole_address & 0xFF000000);
    printf(" => DramHoleOffset: %08x\n", ((dram_hole_address >> 7) & 0xFF) << 23);
    printf(" => DramMemHoistValid: %d\n", (dram_hole_address >> 1) & 1);
    printf(" => DramHoleValid: %d\n", dram_hole_address & 1);
    printf("\n");
}

void amd_print_memory_controller_configuration_lock() {
    // D18F2x118 Memory Controller Configuration Low

    /*
    If lockDramCfg is 1, then the following registers are read-only until next reset:
        D18F1xF0 [DRAM Hole Address]
        D18F2x[5C:40]_dct[0] [DRAM CS Base Address]
        D18F2x[6C:60]_dct[0] [DRAM CS Mask]
        D18F2x80_dct[0] [DRAM Bank Address Mapping]
        D18F2x110 [DRAM Controller Select Low]
        D18F2x114 [DRAM Controller Select High]
        D18F2x250_dct[0] [DRAM Loopback and Training Control]
        D18F4x128[CoreStateSaveDestNode]
        D18F1x[17C:140,7C:40] [DRAM Base/Limit]
        D18F1x120 [DRAM Base System Address]
        D18F1x124 [DRAM Limit System Address]
        D18F2x118[CC6SaveEn]
    */

    DWORD mem_controller_config_low  = read_pci_dword(0, 0x18, 2, D18F2_MEMORY_CONTROLLER_CONFIGURATION_LOW_OFFSET);
    
    int lockDramCfg = (mem_controller_config_low >> 19) & 1;
    
    printf("D18F2x118 Memory Controller Configuration Low\n");
    printf("  => LockDramCfg: %d - ", lockDramCfg);
    if (lockDramCfg == 0) {
        print_red("FAILED\n");
    } else {
        print_green("OK\n");
    }
    printf("\n");
}

void print_SPI_Cntrl0() {
    // AMD 15h Family
    // SPIx00 SPI_Cntrl0

    /*
    SpiHostAccessRomEn. Read; Write-0-only. Reset: 1. This is a clear-once protection bit; once it is 
    cleared to 0 it cannot be set back to 1. 0=MAC cannot access BIOS ROM space (upper 512KB). 
    1=MAC can access BIOS ROM space.

    SpiAccessMacRomEn. IF (Mode == SMI) Read-write. ELSE Read; Write-0-only. ENDIF. Reset: 1. 
    This is a clear-once protection bit. 0=Software cannot access MAC’s portion of the ROM space 
    (lower 512KB). 1=Software can access MAC’s portion of the ROM space
    */
    DWORD spiCntrl0 = g_spi_registers.SPI_Cntrl0;
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


void amd_get_chipset_information() {
    _read_lpc_isa_bridge();
    _read_mmio_cfg_base_addr();

    IO_MEM_PORT_DECODE *pIOMemPortDecodeEnable =  (IO_MEM_PORT_DECODE *) &g_lpc_isa_bridge_registers.IOMemPortDecodeEnableRegister;
    printf("IO/Mem Port Decode\n");
    printf("  - SUPER_IO_CONFIGURATION_PORT_ENABLE: %d\n", pIOMemPortDecodeEnable->SUPER_IO_CONFIGURATION_PORT_ENABLE);
    printf("  - ALTERNATE_SUPER_IO_CONFIGURATION_PORT_ENABLE: %d\n", pIOMemPortDecodeEnable->ALTERNATE_SUPER_IO_CONFIGURATION_PORT_ENABLE);
    printf("  - WIDE_GENERIC_IO_PORT_ENABLE: %d\n", pIOMemPortDecodeEnable->WIDE_GENERIC_IO_PORT_ENABLE);
    printf("  - ROM_RANGE_1_PORT_ENABLE: %d\n", pIOMemPortDecodeEnable->ROM_RANGE_1_PORT_ENABLE);
    printf("  - ROM_RANGE_2_PORT_ENABLE: %d\n", pIOMemPortDecodeEnable->ROM_RANGE_2_PORT_ENABLE);
    printf("  - MEMORY_RANGE_PORT_ENABLE: %d\n", pIOMemPortDecodeEnable->MEMORY_RANGE_PORT_ENABLE);
    printf("  - RCT_IO_RANGE_PORT_ENABLE: %d\n", pIOMemPortDecodeEnable->RCT_IO_RANGE_PORT_ENABLE);
    printf("  - SYNC_TIMEOUT_COUNTER_ENABLE: %d\n", pIOMemPortDecodeEnable->SYNC_TIMEOUT_COUNTER_ENABLE);
    printf("  - SYNC_TIMEOUT: %d\n", pIOMemPortDecodeEnable->SYNC_TIMEOUT);
    printf("  - IO_PORT_ENABLE_0: %d\n", pIOMemPortDecodeEnable->IO_PORT_ENABLE_0);
    printf("  - IO_PORT_ENABLE_1: %d\n", pIOMemPortDecodeEnable->IO_PORT_ENABLE_1);
    printf("  - IO_PORT_ENABLE_2: %d\n", pIOMemPortDecodeEnable->IO_PORT_ENABLE_2);
    printf("  - IO_PORT_ENABLE_3: %d\n", pIOMemPortDecodeEnable->IO_PORT_ENABLE_3);
    printf("  - MEM_PORT_ENABLE: %d\n", pIOMemPortDecodeEnable->MEM_PORT_ENABLE);
    printf("  - IO_PORT_ENABLE_4: %d\n", pIOMemPortDecodeEnable->IO_PORT_ENABLE_4);
    printf("  - IO_PORT_ENABLE_5: %d\n", pIOMemPortDecodeEnable->IO_PORT_ENABLE_5);
    printf("  - IO_PORT_ENABLE_6: %d\n", pIOMemPortDecodeEnable->IO_PORT_ENABLE_6);
    printf("  - WIDE_IO1_ENABLE: %d\n", pIOMemPortDecodeEnable->WIDE_IO1_ENABLE);
    printf("  - WIDE_IO2_ENABLE: %d\n", pIOMemPortDecodeEnable->WIDE_IO2_ENABLE);
    printf("\n");

    printf("MemoryRange: %lx\n", g_lpc_isa_bridge_registers.MemoryRangeRegister);
    printf("\n");

    printf("\n=== SPI Range Protections ===\n");
    ROM_PROTECT *pRomProtect;
    pRomProtect = (ROM_PROTECT *) & g_lpc_isa_bridge_registers.ROMProtect0;
    printf("Rom Protect 0: \n");
    printf("  - Base: %08x\n", pRomProtect->RomBase << 11);
    printf("  - RangeUnit: %d\n", pRomProtect->RangeUnit);
    printf("  - Range: %08x\n", pRomProtect->Range);
    printf("  - Protected size: %08x\n", pRomProtect->RangeUnit ? pRomProtect->Range << 16 : pRomProtect->Range << 12);
    printf("  - WriteProtected: %d\n", pRomProtect->WriteProtect);
    printf("  - ReadProtected: %d\n", pRomProtect->ReadProtect);
    printf("  - Total range [%08x, %08x]\n", pRomProtect->RomBase << 11, (pRomProtect->RomBase << 11) + (pRomProtect->RangeUnit ? (pRomProtect->Range + 1) << 16 : (pRomProtect->Range + 1) << 12) - 1);
    printf("  - STATUS: ");
    if (pRomProtect->RomBase == 0 && pRomProtect->Range == 0) {
        print_red("Warning - Unused ROM Range Protection\n");
    } else {
        print_green("OK - Range in use\n");
    }
    pRomProtect = (ROM_PROTECT *) & g_lpc_isa_bridge_registers.ROMProtect1;
    printf("Rom Protect 1: \n");
    printf("  - Base: %08x\n", pRomProtect->RomBase << 11);
    printf("  - RangeUnit: %d\n", pRomProtect->RangeUnit);
    printf("  - Range: %08x\n", pRomProtect->Range);
    printf("  - Protected size: %08x\n", pRomProtect->RangeUnit ? pRomProtect->Range << 16 : pRomProtect->Range << 12);
    printf("  - WriteProtected: %d\n", pRomProtect->WriteProtect);
    printf("  - ReadProtected: %d\n", pRomProtect->ReadProtect);
    printf("  - Total range [%08x, %08x]\n", pRomProtect->RomBase << 11, (pRomProtect->RomBase << 11) + (pRomProtect->RangeUnit ? (pRomProtect->Range + 1) << 16 : (pRomProtect->Range + 1) << 12) - 1);
    printf("  - STATUS: ");
    if (pRomProtect->RomBase == 0 && pRomProtect->Range == 0) {
        print_red("Warning - Unused ROM Range Protection\n");
    } else {
        print_green("OK - Range in use\n");
    }

    pRomProtect = (ROM_PROTECT *) & g_lpc_isa_bridge_registers.ROMProtect2;
    printf("Rom Protect 2: \n");
    printf("  - Base: %08x\n", pRomProtect->RomBase << 11);
    printf("  - RangeUnit: %d\n", pRomProtect->RangeUnit);
    printf("  - Range: %08x\n", pRomProtect->Range);
    printf("  - Protected size: %08x\n", pRomProtect->RangeUnit ? pRomProtect->Range << 16 : pRomProtect->Range << 12);
    printf("  - WriteProtected: %d\n", pRomProtect->WriteProtect);
    printf("  - ReadProtected: %d\n", pRomProtect->ReadProtect);
    printf("  - Total range [%08x, %08x]\n", pRomProtect->RomBase << 11, (pRomProtect->RomBase << 11) + (pRomProtect->RangeUnit ? (pRomProtect->Range + 1) << 16 : (pRomProtect->Range + 1) << 12) - 1);
    printf("  - STATUS: ");
    if (pRomProtect->RomBase == 0 && pRomProtect->Range == 0) {
        print_red("Warning - Unused ROM Range Protection\n");
    } else {
        print_green("OK - Range in use\n");
    }

    pRomProtect = (ROM_PROTECT *) & g_lpc_isa_bridge_registers.ROMProtect3;
    printf("Rom Protect 3: \n");
    printf("  - Base: %08x\n", pRomProtect->RomBase << 11);
    printf("  - RangeUnit: %d\n", pRomProtect->RangeUnit);
    printf("  - Range: %08x\n", pRomProtect->Range);
    printf("  - Protected size: %08x\n", pRomProtect->RangeUnit ? pRomProtect->Range << 16 : pRomProtect->Range << 12);
    printf("  - WriteProtected: %d\n", pRomProtect->WriteProtect);
    printf("  - ReadProtected: %d\n", pRomProtect->ReadProtect);
    printf("  - Total range [%08x, %08x]\n", pRomProtect->RomBase << 11, (pRomProtect->RomBase << 11) + (pRomProtect->RangeUnit ? (pRomProtect->Range + 1) << 16 : (pRomProtect->Range + 1) << 12) - 1);
    printf("  - STATUS: ");
    if (pRomProtect->RomBase == 0 && pRomProtect->Range == 0) {
        print_red("Warning - Unused ROM Range Protection\n");
    } else {
        print_green("OK - Range in use\n");
    }

    printf("\n");

    DWORD spi_addr = (*(DWORD *) &g_lpc_isa_bridge_registers.SPI_BASE) & 0xFFFFFFC0;
    
    printf("SPI BASE: %lx\n", spi_addr);

    read_physical_memory(spi_addr, sizeof(SPI), &g_spi_registers, false);

    // SPIx00 SPI_Cntrl0
    print_SPI_Cntrl0();

    ALT_SPI_CS *pAltSpiCs = (ALT_SPI_CS *) & g_spi_registers.Alt_SPI_CS;
    printf("SPIx1D - SpiProtectEn0: %d - ", pAltSpiCs->SpiProtectEn0);
    if (pAltSpiCs->SpiProtectEn0 == 0) {
        print_red("FAILED\n");
    } else {
        print_green("OK\n");
    }
    printf("SPIx1D - SpiProtectEn1: %d - ", pAltSpiCs->SpiProtectEn1);
    if (pAltSpiCs->SpiProtectEn1 == 0) {
        print_red("FAILED\n");
    } else {
        print_green("OK\n");
    }
    printf("SPIx1D - SpiProtectLock: %d - ", pAltSpiCs->SpiProtectLock);
    if (pAltSpiCs->SpiProtectLock == 0) {
        print_red("FAILED\n");
    } else {
        print_green("OK\n");
    }
    printf("\n");

    


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
    printf("\n");

    amd_print_smm_base();
    amd_print_smm_tseg_addr();
    amd_print_smm_tseg_mask();
    amd_print_hwcr_smmlock();
    //amd_print_smi_on_io_trap_configuration();
    amd_print_io_trap_control_status();
    amd_print_apic_bar();
    amd_print_mmio_ba();

    amd_print_root_complex_pci_control();
    amd_print_root_complex_nb_control();
    amd_print_root_complex_mmio_range();
    amd_print_root_complex_tom2();
    amd_print_msr_tom2();
    amd_print_root_complex_tom();
    amd_print_msr_tom();
    amd_print_dram_range_base_limit();
    amd_print_mmio_base_limit_ranges();
    amd_print_dram_system_address_range();
    amd_print_dram_controller_range();
    amd_print_dram_hole();
    amd_print_memory_controller_configuration_lock();
}


void amd_dump_spi_flash(const char *output_filename) {
    _read_lpc_isa_bridge();

    LPC_ROM_ADDRESS lpc_rom_addr_range2 = {0};
    lpc_rom_addr_range2.StartAddr = (DWORD)g_lpc_isa_bridge_registers.LPC_ROM_Address_Range_2_StartAddress << 16;
    lpc_rom_addr_range2.EndAddr = ((DWORD)g_lpc_isa_bridge_registers.LPC_ROM_Address_Range_2_EndAddress << 16) | 0xffff;

    size_t rom_size = lpc_rom_addr_range2.EndAddr - lpc_rom_addr_range2.StartAddr + 1;
    char *rom_data  = (char *) calloc(1, rom_size);
    
    debug_print(" -> About to read %08x bytes from %016llx pa\n", rom_size, lpc_rom_addr_range2.StartAddr);
    read_physical_memory_into_file(lpc_rom_addr_range2.StartAddr, rom_size, output_filename );
    
    free(rom_data);
}