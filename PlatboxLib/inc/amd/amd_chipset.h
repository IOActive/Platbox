#pragma once
#include "types.h"
#include "amd_spi.h"

void amd_retrieve_chipset_information();
void amd_print_chipset_information();
DWORD64 amd_retrieve_smm_base();
void get_tseg_info(UINT64 *base, UINT32 *size);
void _read_mmio_cfg_base_addr();
DWORD64 amd_retrieve_smm_tseg_mask();
DWORD64 amd_retrieve_apic_bar();
DWORD64 amd_retrieve_hwcr_smmlock();
DWORD64 amd_retrieve_mmio_ba();
DWORD amd_retrieve_root_complex_pci_control();

void amd_dump_spi_flash(const char *output_filename);

//////////////////// AMD ////////////////

#define AMD_SMI_HANDLER_ENTRY_POINT 0x8000
#define AMD_SMM_STATE_SAVE_AREA     0xFE00

#define BIT_MSR_HWCR_SMMLOCK  1 << 0

// https://www.amd.com/system/files/TechDocs/45482.pdf

enum AMD_MSRS {

	/*
	MSR C001_0015 Hardware Configuration (HWCR)
	Bits Description
		63:32 Reserved.
		31:30 Reserved.
		...
		13 SmiSpCycDis: SMI special bus cycle disable. IF MSRC001_0015[SmmLock] THEN Read-only 
			ELSE Read-write ENDIF. Reset: 0. 0=A link special bus cycle, SMIACK, is generated when an SMI 
			interrupt is taken
		...	
		0 SmmLock: SMM code lock. Read; write-1-only. Reset: 0. SBIOS: 1. 1=SMM code in the ASeg and 
			TSeg range and the SMM registers are read-only and SMI interrupts are not intercepted in SVM.

	*/
	AMD_MSR_HARDWARE_CONFIGURATION = 0xC0010015,

	/* 
	MSR C001_0111 SMM Base Address (SMM_BASE)
	Reset: 0000_0000_0003_0000h. This holds the base of the SMM memory region. The value of this register is 
	stored in the save state on entry into SMM (see 2.4.8.2.5 [SMM Save State]) and it is restored on returning 
	from SMM. The 16-bit CS (code segment) selector is loaded with SmmBase[19:4] on entering SMM. 
	SmmBase[3:0] is required to be 0. The SMM base address can be changed in two ways:
		• The SMM base address, at offset FF00h in the SMM state save area, may be changed by the SMI handler.
			 The RSM instruction updates SmmBase with the new value.
		• Normal WRMSR access to this register --> WHAT?

	Bits Description
		63:32 Reserved.
		31:0 SmmBase. IF MSRC001_0015[SmmLock] THEN Read-only; Not-same-for-all. ELSE Read-write; 
			Not-same-for-all. ENDIF. 
	*/
	AMD_MSR_SMM_BASE_ADDRESS = 0xC0010111,

	/*
	MSR C001_0112 SMM TSeg Base Address (SMMAddr)
	Reset: 0000_0000_0000_0000h.

	Each CPU access, directed at CPUAddr, is determined to be in the TSeg range if the following is true:
	CPUAddr[39:17] & TSegMask[39:17] == TSegBase[39:17] & TSegMask[39:17].

	For example, if TSeg spans 256 KB and starts at the 1 MB address. The MSRC001_0112[TSegBase] would be 
	set to 0010_0000h and the MSRC001_0113[TSegMask] to FFFC_0000h (with zeros filling in for bits[16:0]). 
	This results in a TSeg range from 0010_0000 to 0013_FFFFh.

	Bits Description
		63:40 Reserved.
		39:17 TSegBase[39:17]: TSeg address range base. IF MSRC001_0015[SmmLock] THEN Read-only 
			ELSE Read-write ENDIF.
		16:0 Reserved.
	*/
	AMD_MSR_SMM_TSEG_BASE_ADDRESS = 0xC0010112,


	/*
	MSR C001_0113 SMM TSeg Mask (SMMMask)

	Reset: 0000_0000_0000_0000h.
	The ASeg address range is located at a fixed address from A0000h–BFFFFh. The TSeg range is located at a 
	variable base (specified by MSRC001_0112[TSegBase]) with a variable size (specified by 
	MSRC001_0113[TSegMask]). These ranges provide a safe location for SMM code and data that is not readily 
	accessible by non-SMM applications. The SMI handler can be located in one of these two ranges, or it can be 
	located outside these ranges. These ranges must never overlap each other.

	This register specifies how accesses to the ASeg and TSeg address ranges are controlled as follows:
	- If [A,T]Valid=1, then:
	- If in SMM, then:
	- If [A, T]Close=0, then the accesses are directed to DRAM with memory type as specified in [A,T] MTypeDram.
	- If [A, T]Close=1, then instruction accesses are directed to DRAM with memory type as specified in
		[A, T]MTypeDram and data accesses are directed at MMIO space and with attributes based on [A,T] MTypeIoWc.
	- If not in SMM, then the accesses are directed at MMIO space with attributes based on
		[A,T]MTypeIoWc.

	Bits Description
		63:40 Reserved.
		39:17 TSegMask[39:17]: TSeg address range mask. IF MSRC001_0015[SmmLock] THEN Read-only 
			ELSE Read-write ENDIF. See MSRC001_0112.
		16:15 Reserved. 
		14:12 TMTypeDram: TSeg address range memory type. IF MSRC001_0015[SmmLock] THEN Read only. ELSE Read-write. ENDIF. Specifies the memory type for SMM accesses to the TSeg range that 
			are directed to DRAM. See: Table 219 [Valid Values for Memory Type Definition].
		11 Reserved. 
		10:8 AMTypeDram: ASeg Range Memory Type. IF MSRC001_0015[SmmLock] THEN Read-only. 
			ELSE Read-write. ENDIF. Specifies the memory type for SMM accesses to the ASeg range that are 
			directed to DRAM. See: Table 219 [Valid Values for Memory Type Definition].
		7:6 Reserved. 
		5 TMTypeIoWc: non-SMM TSeg address range memory type. IF MSRC001_0015[SmmLock] 
			THEN Read-only. ELSE Read-write. ENDIF. Specifies the attribute of TSeg accesses that are 
			directed to MMIO space. 0=UC (uncacheable). 1=WC (write combining).
		4 AMTypeIoWc: non-SMM ASeg address range memory type. IF MSRC001_0015[SmmLock] 
			THEN Read-only. ELSE Read-write. ENDIF. Specifies the attribute of ASeg accesses that are 
			directed to MMIO space. 0=UC (uncacheable). 1=WC (write combining).
		3 TClose: send TSeg address range data accesses to MMIO. Read-write. 1=When in SMM, direct 
			data accesses in the TSeg address range to MMIO space. See AClose.
		2 AClose: send ASeg address range data accesses to MMIO. Read-write. 1=When in SMM, direct 
			data accesses in the ASeg address range to MMIO space. 
			[A, T]Close allows the SMI handler to access the MMIO space located in the same address region as 
			the [A, T]Seg. When the SMI handler is finished accessing the MMIO space, it must clear the bit. 
			Failure to do so before resuming from SMM causes the CPU to erroneously read the save state from 
			MMIO space. 
		1 TValid: enable TSeg SMM address range. IF MSRC001_0015[SmmLock] THEN Read-only. 
			ELSE Read-write. ENDIF. 1=The TSeg address range SMM enabled. 
		0 AValid: enable ASeg SMM address range. IF MSRC001_0015[SmmLock] THEN Read-only. 
			ELSE Read-write. ENDIF. 1=The ASeg address range SMM enabled.
	*/
	AMD_MSR_SMM_TSEG_MASK = 0xC0010113,

	/*
	MSR C001_0116 SMM Control (SMM_CTL)

	IF (MSRC001_0015[SmmLock]) THEN GP-read-write. ELSE GP-read; write-only. ENDIF.
	The bits in this register are processed in the order of: SmmEnter, SmiCycle, SmmDismiss, RsmCycle and 
	SmmExit. However, only the following combination of bits may be set in a single write (all other combinations 
	result in undefined behavior):
	• SmmEnter and SmiCycle.
	• SmmEnter and SmmDismiss.
	• SmmEnter, SmiCycle and SmmDismiss.
	• SmmExit and RsmCycle.
	Software is responsible for ensuring that SmmEnter and SmmExit operations are properly matched and are not 
	nested.

	Bits Description
		63:5 MBZ.
		4 RsmCycle: send RSM special cycle. 1=Send a RSM special cycle.
		3 SmmExit: exit SMM. 1=Exit SMM. 
		2 SmiCycle: send SMI special cycle. 1=Send a SMI special cycle.
		1 SmmEnter: enter SMM. 1=Enter SMM.
		0 SmmDismiss: clear SMI. 1=Clear the SMI pending flag.

	*/
	AMD_MSR_SMM_CONTROL = 0xC0010116,

	/*
	MSR C001_011A Local SMI Status
	Reset: 0000_0000_0000_0000h. This registers returns the same information that is returned in SMMFEC4 
	[Local SMI Status] portion of the SMM save state. The information in this register is only updated when 
	MSRC001_0116[SmmDismiss] is set by software.

	Bits Description
		63:32 Reserved.
		31:0 Alias of SMMFEC4 [Local SMI Status].
	*/
	AMD_MSR_LOCAL_SMI_STATUS = 0xC001011A,




	/*
	MSR C001_00[53:50] IO Trap (SMI_ON_IO_TRAP_[3:0])

	provide a mechanism for executing the SMI handler if a an access to one of the specified addresses 
	is detected. Access address and access type checking is performed before IO	instruction execution. 
	If the access address and access type match one of the specified IO address and access types, then:
	    (1) the IO instruction is not executed; 
		(2) any breakpoint, other than the single-step breakpoint, set on the IO instruction is not taken 
			(the single-step breakpoint is taken after resuming from SMM); 
		(3) the	SMI-trigger IO cycle specified by MSRC001_0056. 
	The status is stored in SMMFEC4[IoTrapSts].

	The access address is compared with SmiAddr, and the instruction access type is compared with the enabled
	access types defined by ConfigSMI, SmiOnRdEn, and SmiOnWrEn. 
	Access address bits[23:0] can be masked	with SmiMask.

	IO and configuration space trapping to SMI applies only to single IO instructions; it does not apply to string
	and REP IO instructions. ?????????

	Bits Description
		63 SmiOnRdEn: enable SMI on IO read. Read-write. 1=Enables SMI generation on a read access.
		62 SmiOnWrEn: enable SMI on IO write. Read-write. 1=Enables SMI generation on a write access.
		61 ConfigSmi: configuration space SMI. Read-write. 1=Configuration access. 0=IO access (that is not
			an IO-space configuration access).
		60:56 Reserved.
		55:32 SmiMask[23:0]. Read-write. SMI IO trap mask. 0=Mask address bit. 1=Do not mask address bit.
		31:0 SmiAddr[31:0]. Read-write. SMI IO trap address.

	*/
	AMD_MSR_SMI_ON_IO_TRAP_0 = 0xC0010050,
	AMD_MSR_SMI_ON_IO_TRAP_1 = 0xC0010051,
	AMD_MSR_SMI_ON_IO_TRAP_2 = 0xC0010052,
	AMD_MSR_SMI_ON_IO_TRAP_3 = 0xC0010053,

	/*
	MSR C001_0054 IO Trap Control (SMI_ON_IO_TRAP_CTL_STS)

	For each of the SmiEn bits below, 1=The trap specified by the corresponding MSR is enabled. See MSR C001_00[53:50].

	Bits Description
		63:32 RAZ.
		31:16 Reserved. 
		15 IoTrapEn: IO trap enable. Read-write. Reset: 0. 1=Enable IO and configuration space trapping
			specified by MSRC001_00[53:50] and MSRC001_0054.
		14:8 Reserved.
		7 SmiEn3: SMI enable for the trap specified by MSRC001_0053. Read-write. Reset: 0.
		6 Reserved.
		5 SmiEn2: SMI enable for the trap specified by MSRC001_0052. Read-write. Reset: 0.
		4 Reserved.
		3 SmiEn1: SMI enable for the trap specified by MSRC001_0051. Read-write. Reset: 0.
		2 Reserved.
		1 SmiEn0: SMI enable for the trap specified by MSRC001_0050. Read-write. Reset: 0.
		0 Reserved

	*/
	AMD_MSR_IO_TRAP_CTL_STS = 0xC0010054,


	/*
	MSR 0000_001B APIC Base Address (APIC_BAR)
	
	Bits Description
		63:40 MBZ.
		39:12 ApicBar[39:12]: APIC base address register. Read-write. Reset: 00_FEE0_0h. Specifies the base
			address, physical address [39:12], for the APICXX register set in xAPIC mode. See 2.4.8.1.2 [APIC
			Register Space].
		11 ApicEn: APIC enable. Read-write. Reset: 0. See 2.4.8.1.2 [APIC Register Space]. 1=Local APIC is
			enabled in xAPIC mode.
		10 MBZ.
		9 MBZ.
		8 BSC: boot strap core. Read-write; updated-by-hardware. Reset: x.
			 1=The core is the boot core of the BSP. 
			 0=The core is not the boot core of the BSP.
		7:0 MBZ.
	*/
	AMD_MSR_APIC_BAR = 0x0000001B,



	/*
	MSR C001_0058 MMIO Configuration Base Address

	Bits Description
		63:40 RAZ.
		39:20 MmioCfgBaseAddr[39:20]: MMIO configuration base address bits[39:20]. Read-write. Reset: 0.
			Specifies the base address of the MMIO configuration range. The size of the MMIO configurationspace address
			range is specified by BusRange as follows. All lower order undefined bits must be 0.
			
			BusRange MmioCfgBaseAddr BusRange MmioCfgBaseAddr
			0h [39:20] 5h [39:25]
			1h [39:21] 6h [39:26]
			2h [39:22] 7h [39:27]
			3h [39:23] 8h [39:28]
			4h [39:24] Fh-9h Reserved

		19:6 RAZ.
		5:2 BusRange: bus range identifier. Read-write. Reset: 0. Specifies the number of buses in the MMIO
			configuration space range. The size of the MMIO configuration space is 1 MB times the number of
			buses.
				Bits Description Bits Description
				0h 1 5h 32
				1h 2 6h 64
				2h 4 7h 128
				3h 8 8h 256
				4h 16 Fh-9h Reserved
		1 Reserved.
		0 Enable. Read-write. Reset: 0. 1=MMIO configuration space is enabled.
	*/
	AMD_MSR_MMIO_CONFIGURATION_BASE_ADDRRESS = 0xC0010058,


	/*
	MSR C001_001D Top Of Memory 2 (TOM2)

	Bits Description
	63:40 RAZ.
	39:23 TOM2[39:23]: second top of memory. Read-write. Specifies the address divides between MMIO
		and DRAM. 
			This value is normally placed above 4G. From 4G to TOM2 - 1 is DRAM; 
			TOM2 and above is MMIO. 
			This register is enabled by MSR C001_0010[MtrrTom2En].
	22:0 RAZ.
	*/
	AMD_MSR_TOP_OF_MEMORY_2 = 0xC001001D,

	/*
	MSR C001_001A Top Of Memory (TOP_MEM)

	TOM[39:23]: top of memory. Read-write. Specifies the address that divides between MMIO and
	DRAM. This value is normally placed below 4G. From TOM to 4G is MMIO; below TOM is DRAM.
	*/
	AMD_MSR_TOP_OF_MEMORY  = 0xC001001A,

};


#define BIT_SUPER_IO_CONFIGURATION_PORT_ENABLE 1 << 0
#define BIT_ALTERNATE_SUPER_IO_CONFIGURATION_PORT_ENABLE 1 << 1
#define BIT_WIDE_GENERIC_IO_PORT_ENABLE 1 << 2
#define BIT_ROM_RANGE_1_PORT_ENABLE 1 << 3
#define BIT_ROM_RANGE_2_PORT_ENABLE 1 << 4
#define BIT_MEMORY_RANGE_PORT_ENABLE 1 << 5
#define BIT_RCT_IO_RANGE_PORT_ENABLE 1 << 6
#define BIT_SYNC_TIMEOUT_COUNTER_ENABLE 1 << 7
#define BIT_IO_PORT_ENABLE_0 1 << 16
#define BIT_IO_PORT_ENABLE_1 1 << 17
#define BIT_IO_PORT_ENABLE_2 1 << 18
#define BIT_IO_PORT_ENABLE_3 1 << 19
#define BIT_MEM_PORT_ENABLE  1 << 20
#define BIT_IO_PORT_ENABLE_4 1 << 21
#define BIT_IO_PORT_ENABLE_5 1 << 22
#define BIT_IO_PORT_ENABLE_6 1 << 23
#define BIT_WIDE_IO1_ENABLE  1 << 24
#define BIT_WIDE_IO2_ENABLE  1 << 25


typedef struct _IO_MEM_PORT_DECODE {
	DWORD SUPER_IO_CONFIGURATION_PORT_ENABLE: 1;
	DWORD ALTERNATE_SUPER_IO_CONFIGURATION_PORT_ENABLE: 1;
	DWORD WIDE_GENERIC_IO_PORT_ENABLE: 1;
	DWORD ROM_RANGE_1_PORT_ENABLE: 1;
	DWORD ROM_RANGE_2_PORT_ENABLE: 1;
	DWORD MEMORY_RANGE_PORT_ENABLE: 1;
	DWORD RCT_IO_RANGE_PORT_ENABLE: 1;
	DWORD SYNC_TIMEOUT_COUNTER_ENABLE: 1;
	DWORD SYNC_TIMEOUT: 8;
	DWORD IO_PORT_ENABLE_0: 1;
	DWORD IO_PORT_ENABLE_1: 1;
	DWORD IO_PORT_ENABLE_2: 1;
	DWORD IO_PORT_ENABLE_3: 1;
	DWORD MEM_PORT_ENABLE: 1;
	DWORD IO_PORT_ENABLE_4: 1;
	DWORD IO_PORT_ENABLE_5: 1;
	DWORD IO_PORT_ENABLE_6: 1;
	DWORD WIDE_IO1_ENABLE: 1;
	DWORD WIDE_IO2_ENABLE: 1;
	DWORD RESVD: 7;
} IO_MEM_PORT_DECODE;

// Enabled if SPIx1D Alt_SPI_CS:
// - SpiProtectEn0 == 1
// - SpiProtectEn1 == 1 (optional)
// - SpiProtectLock == 1
typedef struct _ROM_PROTECT {
	DWORD Range 		: 8;
	DWORD RangeUnit  	: 1;
	DWORD ReadProtect   : 1;
	DWORD WriteProtect	: 1;
	DWORD RomBase       : 21;
} ROM_PROTECT;

typedef struct _LPC_ROM_ADDRESS {
	DWORD StartAddr;
	DWORD EndAddr;
} LPC_ROM_ADDRESS;


typedef struct _SPI_BASE_ADDR {
	DWORD reserved0 : 1;
	DWORD SpiRomEnable: 1;
	DWORD reserved1 : 3;
	DWORD SPI_BaseAddr: 27;
} SPI_BASE_ADDR;

typedef struct _LPC_ISA_Bridge {
	WORD VendorID;
	WORD DeviceID;
	WORD Command;
	WORD Status;
	DWORD RevisionId_ClassCode;
	BYTE CacheLineSize;
	BYTE LatencyTimer;
	BYTE HeaderType;
	BYTE BIST;
	DWORD BAR0[7];
	DWORD SubsystemID;
	DWORD SubsystemVendorID;
	DWORD CapabilitiesPointer[3];

	// 0x40
	DWORD PCIControl;
	// 0x44
	DWORD IOPortDecodeEnableRegister;


	// 0x48


	/*
	Bits Description
		31:26 Reserved.
		25 WideIO2Enable. Read-write. Reset: 0. Port enable for wide generic IO port 2 defined by
		D14F3x90[IOBaseAddress2]. 1=Enable the IO range.
		24 WideIO1Enable. Read-write. Reset: 0. Port enable for wide generic IO port 1 defined by
		D14F3x64[IOBaseAddress1]. 1=Enable the IO range.
		23 IOPortEnable6: IO port enable 6. Read-write. Reset: 0. Port enable for IO port FD60h-FD6Fh.
		1=Enable the IO range.
		22 IOPortEnable5: IO port enable 5. Read-write. Reset: 0. Port enable for IO port 4700h-470Bh.
		1=Enable the IO range.
		21 IOPortEnable4: IO port enable 4. Read-write. Reset: 1. Port enable for IO port 80h. 1=Enable the
		IO range.
		20 MemPortEnable: Mem port enable. Read-write. Reset: 0. Port enable for 4K byte memory range
		defined in D14F3x4C. 1=Enable the memory range.
		19 IOPortEnable3: IO port enable 3. Read-write. Reset: 0. Port enable for IO port 580h-5BFh.
		1=Enable the IO range.
		18 IOPortEnable2: IO port enable 2. Read-write. Reset: 0. Port enable for IO port 500h-53Fh.
		1=Enable the IO range.
		17 IOPortEnable1: IO port enable 1. Read-write. Reset: 0. Port enable for IO port 480h-4BFh.
		1=Enable the IO range.
		16 IOPortEnable0: IO port enable 0. Read-write. Reset: 0. Port enable for IO port 400h-43Fh.
		1=Enable the IO range.
		15:8 SyncTimeoutCount: Sync Timeout Count. Read-write. Reset: FFh. When [SyncTimeoutCounterEnable] == 1, this is the number of LPC clocks that the state machine will wait during LPC data sync
		before aborting the cycle.
		7 SyncTimeoutCounterEnable: Sync Timeout Counter Enable. Read-write. Reset: 0. 1=LPC sync
		timeout counter is enabled. 0=The counter is disabled. This counter is used to avoid a deadlock condition if an LPC device drives sync forever. Timeout count is programmed in [SyncTimeoutCount].
		Write 0 to this bit if an LPC device is extremely slow and takes more than 255 LPC clocks to complete a cycle.
		6 RtcIORangePortEnable: RTC IO Range Port Enable. Read-write. Reset: 0. Port enable for RTC
		IO range 70h-73h. 1=Enable the IO range.
		5 MemoryRangePortEnable: Memory Range Port Enable. Read-write. Reset: 0. Port enable for
		LPC memory target range defined by D14F3x60. 1=Enable the memory range.
		4 Reserved.
		3 Reserved.
		2 WideIO0Enable. Read-write. Reset: 0. Port enable for wide generic IO port defined by
		D14F3x64[IOBaseAddress0]. 1=Enable the IO range.
		1 AlternateSuperIOConfigurationPortEnable: Alternate Super IO Configuration Port Enable.
		Read-write. Reset: 0. Port enable for alternate Super IO configuration port, 4Eh-4Fh. 1=Enable the IO
		range.
		0 SuperIOConfigurationPortEnable: Super IO Configuration Port Enable. Read-write. Reset: 0.
		Port enable for Super IO configuration port, 2Eh-2Fh. 1=Enable the IO range.
	*/

	DWORD IOMemPortDecodeEnableRegister;

	// 0x4C
	DWORD MemoryRangeRegister;
	// 0x50
	DWORD ROMProtect0;
	// 0x54
	DWORD ROMProtect1;
	// 0x58
	DWORD ROMProtect2;
	// 0x5C
	DWORD ROMProtect3;
	
	// 0x60   D14F3x60 PCI Memory Address for LPC Target Cycles
	WORD PCIMemoryStartAddressForLPCTargetCycles;
	WORD PCIMemoryEndAddressForLPCTargetCycles;


	// 0x64
	DWORD PCIIOBaseAddressForWideGenericPort;

	
	
	/*
	This register contains the upper 16 bits of the starting address of the LPC ROM address range 1. The lower 16 bits 
	of the starting address are considered 0’s. This range can be enabled/disabled using reg0x48[3] (Rom Range 1 
	Port Enable) or when the strap pins are set to choose LPC ROM
	*/
	// 0x68
	WORD LPC_ROM_Address_Range_1_StartAddress;

	/*
	This register contains the upper 16 bits of the ending address of the LPC ROM address range 1. The lower 16 bits 
	of the end address are considered 1’s. This range can be enabled/disabled using reg0x48[3] (Rom Range 1 Port 
	Enable) or when the strap pins are set to choose LPC ROM
	*/
	WORD LPC_ROM_Address_Range_1_EndAddress;

	// 0x6C
	WORD LPC_ROM_Address_Range_2_StartAddress;
	WORD LPC_ROM_Address_Range_2_EndAddress;


	// 0x70
	DWORD FirmwareHubSelect;

	// 0x74
	DWORD AlternativeWideIORangeEnable;

	// 0x78
	DWORD MiscCtlBits; // MISC_CTL_BITS

	// 0x7C
	BYTE TPM;
	BYTE LPC_CLP_Cntl[3];

	// 0x80
	DWORD Reserved1;

	// 0x84
	DWORD TMKBC_BaseAddrLow;
	DWORD TMKBC_BaseAddrHigh;
	DWORD TMKBC_Remap;

	DWORD64 Wide_IO2;

	DWORD IMC_LPC_Cntrl;

	DWORD Gec_ShadowRom_Address;

	// 0xA0
	DWORD SPI_BASE;

	// 0xA4
	DWORD IMC_PortAddress[3];

	// 0xB0
	DWORD ROM_DMA_src_address;
	DWORD ROM_DMA_dst_address;

	// 0xB8
	// XXX There might be some mistakes here
	WORD ROM_DMA_Control;
	BYTE IMC_Control;
	BYTE Host_Control[5];

	DWORD IMC_Rom_Wr_Of;
	DWORD IMC_Rom_Rd_Of;

} LPC_ISA_Bridge;

typedef struct _MISC_CTL_BITS {
	DWORD NoHog				: 1;
	DWORD Reserved1			: 1;
	DWORD LDRQ0				: 1;
	DWORD Reserved2			: 1;
	DWORD SmmWriteRomEn		: 1;
	DWORD GateSpiAccessDis	: 1;
	DWORD GateWrongRx		: 1;
	DWORD AllowHostInDma	: 1;
	DWORD Reserved3			: 1;
	DWORD LDRQ0_PU_EN		: 1;
	DWORD LDRQ0_PD_EN		: 1;
	DWORD Reserved4			: 21;
} MISC_CTL_BITS;


#define ROOT_COMPLEX_PCI_CONTROL_OFFSET 0x4C
#define ROOT_COMPLEX_MISC_INDEX 0x60
#define ROOT_COMPLEX_MISC_INDEX_DATA 0x64
#define ROOT_COMPLEX_NORTHBRIDGE_TOP_OF_MEMORY_OFFSET 0x90


#define ROOT_COMPLEX_MISC_INDEX_OFFSET_NORTHBRIDGE_CONTROL 0x00
#define ROOT_COMPLEX_MISC_INDEX_OFFSET_MMIO_BASE 0x17
#define ROOT_COMPLEX_MISC_INDEX_OFFSET_MMIO_LIMIT 0x18
#define ROOT_COMPLEX_MISC_INDEX_OFFSET_TOM2_LOW 0x19
#define ROOT_COMPLEX_MISC_INDEX_OFFSET_TOM2_HIGH 0x1A

#define ROOT_COMPLEX_LINK_INDEX 0xE0
#define ROOT_COMPLEX_LINK_INDEX_DATA 0xE4


#define D18F1_DRAM_BASE_LIMIT__NUMBER_OF_RANGES 8
#define D18F1_DRAM_BASE_LIMIT__BASE_LOW_START 0x40
#define D18F1_DRAM_BASE_LIMIT__LIMIT_LOW_START 0x44
#define D18F1_DRAM_BASE_LIMIT__BASE_HIGH_START 0x140
#define D18F1_DRAM_BASE_LIMIT__LIMIT_HIGH_START 0x144

#define D18F1_MMIO_BASE_LIMIT__NUMBER_OF_RANGES 12
#define D18F1_MMIO_BASE_LIMIT__BASE_LOW_START 0x80
#define D18F1_MMIO_BASE_LIMIT__LIMIT_LOW_START 0x84

#define D18F1_DRAM_HOLE_ADDRESS_OFFSET 0xF0

#define D18F1_DRAM_BASE_SYSTEM_ADDRESS_OFFSET 0x120
#define D18F1_DRAM_LIMIT_SYSTEM_ADDRESS_OFFSET 0x124

#define D18F1_DRAM_CONTROLLER_BASE_RANGE_0_OFFSET 0x200
#define D18F1_DRAM_CONTROLLER_LIMIT_RANGE_0_OFFSET 0x204


#define D18F2_MEMORY_CONTROLLER_CONFIGURATION_LOW_OFFSET 0x118


/// Expose these to other modules

extern LPC_ISA_Bridge g_lpc_isa_bridge_registers;
extern DWORD64 g_MmioCfgBaseAddr;


void write_rom_protect0();