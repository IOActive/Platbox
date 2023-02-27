#include "intel_chipset.h"
#include "pci.h"
#include "physmem.h"
#include "msr.h"
#include "global.h"


//////////////////////////////////////////////////////
// SPI Memory Mapped Resgisters (SPIBAR)

void print_spi_bar_bios_bfpreg(DWORD *p) {
	printf("SPIBAR::_BIOS_BFPREG - SPI BIOS MMIO PRI (BIOS_BFPREG) - Offset 0h\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> PRB (BIOS Flash Primary Region Base): %08x\n", ((_BIOS_BFPREG *)p)->PRB << 12);
	printf("   -> PRL (BIOS Flash Primary Region Limit): %08x\n", ((_BIOS_BFPREG *)p)->PRL << 12);
}

void print_spi_bar_hsfs_ctl(DWORD *p) {
	printf("SPIBAR::_BIOS_HSFSTS_CTL - Hardware Sequencing Flash Status and Control (BIOS_HSFSTS_CTL) - Offset 4h\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> FDONE (Flash Cycle Done): %02x\n", ((_BIOS_HSFSTS_CTL *)p)->FDONE);
	printf("   -> WRSDIS (Flash Configuration Lock-Down): %02x\n", ((_BIOS_HSFSTS_CTL *)p)->WRSDIS);
	printf("   -> PRR34_LOCKDN (PRR3 PRR4 Lock-Down): %02x\n", ((_BIOS_HSFSTS_CTL *)p)->PRR34_LOCKDN);
	printf("   -> FLOCKDN (Flash Configuration Lock-Down): %02x\n", ((_BIOS_HSFSTS_CTL *)p)->FLOCKDN);
}

void print_spi_bar_faddr(DWORD *p) {
	printf("SPIBAR::_BIOS_FADDR - Flash Address (BIOS_FADDR) - Offset 8h\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> FLA (Flash Linear Address): %08x\n", ((_BIOS_FADDR *)p)->FLA);
}

void print_spi_bar_dlock(DWORD *p) {
	printf("SPIBAR::_BIOS_DLOCK - Discrete Lock Bits (BIOS_DLOCK) - Offset Ch\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> BMWAGLOCKDN (BMWAG Lock-Down): %02x\n", ((_BIOS_DLOCK *)p)->BMWAGLOCKDN);
	printf("   -> BMRAGLOCKDN (BMRAG Lock-Down): %02x\n", ((_BIOS_DLOCK *)p)->BMRAGLOCKDN);
	printf("   -> SBMWAGLOCKDN (SBMWAGLOCKDN Lock-Down): %02x\n", ((_BIOS_DLOCK *)p)->SBMWAGLOCKDN);
	printf("   -> SBMRAGLOCKDN (SBMRAGLOCKDN Lock-Down): %02x\n", ((_BIOS_DLOCK *)p)->SBMRAGLOCKDN);
	printf("   -> PR0LOCKDN (PR0 Lock-Down): %02x\n", ((_BIOS_DLOCK *)p)->PR0LOCKDN);
	printf("   -> PR1LOCKDN (PR1 Lock-Down): %02x\n", ((_BIOS_DLOCK *)p)->PR1LOCKDN);
	printf("   -> PR2LOCKDN (PR2 Lock-Down): %02x\n", ((_BIOS_DLOCK *)p)->PR2LOCKDN);
	printf("   -> PR3LOCKDN (PR3 Lock-Down): %02x\n", ((_BIOS_DLOCK *)p)->PR3LOCKDN);
	printf("   -> PR4LOCKDN (PR4 Lock-Down): %02x\n", ((_BIOS_DLOCK *)p)->PR4LOCKDN);
	printf("   -> SSEQLOCKDN (SSEQ Lock-Down): %02x\n", ((_BIOS_DLOCK *)p)->SSEQLOCKDN);
}

void print_spi_bar_freg0(DWORD *p) {
	printf("SPIBAR::_BIOS_FREG0 - Flash Region 0 (BIOS_FREG0) - Offset 54h\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> RB (Region Base): %08x\n", ((_BIOS_FREG *)p)->RB<<12);
	printf("   -> RL (Region Limit): %08x\n", ((_BIOS_FREG *)p)->RL << 12);
}

void print_spi_bar_freg1(DWORD *p) {
	printf("SPIBAR::_BIOS_FREG1 - Flash Region 1 (BIOS_FREG1) - Offset 58h\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> RB (Region Base): %08x\n", ((_BIOS_FREG *)p)->RB << 12);
	printf("   -> RL (Region Limit): %08x\n", ((_BIOS_FREG *)p)->RL << 12);
}

void print_spi_bar_freg2(DWORD *p) {
	printf("SPIBAR::_BIOS_FREG2 - Flash Region 2 (BIOS_FREG2) - Offset 5Ch\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> RB (Region Base): %08x\n", ((_BIOS_FREG *)p)->RB << 12);
	printf("   -> RL (Region Limit): %08x\n", ((_BIOS_FREG *)p)->RL << 12);
}

void print_spi_bar_freg3(DWORD *p) {
	printf("SPIBAR::_BIOS_FREG3 - Flash Region 3 (BIOS_FREG3) - Offset 60h\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> RB (Region Base): %08x\n", ((_BIOS_FREG *)p)->RB << 12);
	printf("   -> RL (Region Limit): %08x\n", ((_BIOS_FREG *)p)->RL << 12);
}

void print_spi_bar_freg4(DWORD *p) {
	printf("SPIBAR::_BIOS_FREG4 - Flash Region 4 (BIOS_FREG4) - Offset 64h\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> RB (Region Base): %08x\n", ((_BIOS_FREG *)p)->RB << 12);
	printf("   -> RL (Region Limit): %08x\n", ((_BIOS_FREG *)p)->RL << 12);
}

void print_spi_bar_freg5(DWORD *p) {
	printf("SPIBAR::_BIOS_FREG5 - Flash Region 5 (BIOS_FREG5) - Offset 68h\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> RB (Region Base): %08x\n", ((_BIOS_FREG *)p)->RB << 12);
	printf("   -> RL (Region Limit): %08x\n", ((_BIOS_FREG *)p)->RL << 12);
}

void print_spi_bar_fpr0(DWORD *p) {
	printf("SPIBAR::_BIOS_FPR0 - Flash Protected Range 0 (BIOS_FPR0) - Offset 84h\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> PRB (Protected Range Base): %08x\n", ((_BIOS_FPR *)p)->PRB << 12);
	printf("   -> PRL (Protected Range Limit): %08x\n", ((_BIOS_FPR *)p)->PRL << 12);
	printf("   -> RPE (Read Protection Enable): %s\n", ((_BIOS_FPR *)p)->RPE ? "TRUE" : "FALSE");
	printf("   -> WPE (Write Protection Enable): %s\n", ((_BIOS_FPR *)p)->WPE ? "TRUE" : "FALSE");
}

void print_spi_bar_fpr1(DWORD *p) {
	printf("SPIBAR::_BIOS_FPR1 - Flash Protected Range 1 (BIOS_FPR1) - Offset 88h\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> PRB (Protected Range Base): %08x\n", ((_BIOS_FPR *)p)->PRB << 12);
	printf("   -> PRL (Protected Range Limit): %08x\n", ((_BIOS_FPR *)p)->PRL << 12);
	printf("   -> RPE (Read Protection Enable): %s\n", ((_BIOS_FPR *)p)->RPE ? "TRUE" : "FALSE");
	printf("   -> WPE (Write Protection Enable): %s\n", ((_BIOS_FPR *)p)->WPE ? "TRUE" : "FALSE");
}

void print_spi_bar_fpr2(DWORD *p) {
	printf("SPIBAR::_BIOS_FPR2 - Flash Protected Range 2 (BIOS_FPR2) - Offset 8Ch\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> PRB (Protected Range Base): %08x\n", ((_BIOS_FPR *)p)->PRB << 12);
	printf("   -> PRL (Protected Range Limit): %08x\n", ((_BIOS_FPR *)p)->PRL << 12);
	printf("   -> RPE (Read Protection Enable): %s\n", ((_BIOS_FPR *)p)->RPE ? "TRUE" : "FALSE");
	printf("   -> WPE (Write Protection Enable): %s\n", ((_BIOS_FPR *)p)->WPE ? "TRUE" : "FALSE");
}

void print_spi_bar_fpr3(DWORD *p) {
	printf("SPIBAR::_BIOS_FPR3 - Flash Protected Range 3 (BIOS_FPR3) - Offset 90h\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> PRB (Protected Range Base): %08x\n", ((_BIOS_FPR *)p)->PRB << 12);
	printf("   -> PRL (Protected Range Limit): %08x\n", ((_BIOS_FPR *)p)->PRL << 12);
	printf("   -> RPE (Read Protection Enable): %s\n", ((_BIOS_FPR *)p)->RPE ? "TRUE" : "FALSE");
	printf("   -> WPE (Write Protection Enable): %s\n", ((_BIOS_FPR *)p)->WPE ? "TRUE" : "FALSE");
}

void print_spi_bar_fpr4(DWORD *p) {
	printf("SPIBAR::_BIOS_FPR4 - Flash Protected Range 4 (BIOS_FPR4) - Offset 94h\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> PRB (Protected Range Base): %08x\n", ((_BIOS_FPR *)p)->PRB << 12);
	printf("   -> PRL (Protected Range Limit): %08x\n", ((_BIOS_FPR *)p)->PRL << 12);
	printf("   -> RPE (Read Protection Enable): %s\n", ((_BIOS_FPR *)p)->RPE ? "TRUE" : "FALSE");
	printf("   -> WPE (Write Protection Enable): %s\n", ((_BIOS_FPR *)p)->WPE ? "TRUE" : "FALSE");
}


//////////////////////////////////////////////////////
// SPI Interface B0:D31:F5 Registers

void print_spi_interface_bar0_mmio(DWORD *p) {
	printf("0:31:5_BIOS_SPI_BAR0 - SPI BAR0 MMIO (BIOS_SPI_BAR0)\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> SPI BAR: %08x\n", ((PCI_BAR *)p)->BaseAddress << 4);
	printf("   -> I/O Space: %s\n", ((PCI_BAR *)p)->IOMapped ? "TRUE" : "FALSE");
}
void read_spi_interface_bar0_mmio(DWORD *pBar0Mmio) {
	// SPI BAR0 MMIO (BIOS_SPI_BAR0)
	UINT32 offset1 = 0x10;
	*pBar0Mmio = read_pci_dword(0, 31, 5, offset1/sizeof(DWORD));
	debug_print("read_spi_interface_bar0_mmio -> %08x\n", *pBar0Mmio);
}

void print_spi_interface_bios_decode_enable(DWORD *p) {
	printf("0:31:5_BIOS_SPI_BDE - BIOS Decode Enable (BIOS_SPI_BDE)\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> E40 (40-4F Enable) 1MB FF400000h-FF4FFFFFh: %s\n", ((_BIOS_SPI_BDE *)p)->E40 ? "TRUE" : "FALSE");
	printf("   -> E50 (50-5F Enable) 1MB FF500000h-FF5FFFFFh: %s\n", ((_BIOS_SPI_BDE *)p)->E50 ? "TRUE" : "FALSE");
	printf("   -> E60 (60-6F Enable) 1MB FF600000h-FF6FFFFFh: %s\n", ((_BIOS_SPI_BDE *)p)->E60 ? "TRUE" : "FALSE");
	printf("   -> E70 (70-7F Enable) 1MB FF700000h-FF7FFFFFh: %s\n", ((_BIOS_SPI_BDE *)p)->E70 ? "TRUE" : "FALSE");
	printf("   -> LEE (Legacy E Segment Enable) 64K E0000h-EFFFFh: %s\n", ((_BIOS_SPI_BDE *)p)->LEE ? "TRUE" : "FALSE");
	printf("   -> LFE (Legacy F Segment Enable) 64K F0000h-FFFFFh: %s\n", ((_BIOS_SPI_BDE *)p)->LFE ? "TRUE" : "FALSE");
	printf("   -> EC0 (C0-C7 Enable) 512K FFC00000h-FFC7FFFFh: %s\n", ((_BIOS_SPI_BDE *)p)->EC0 ? "TRUE" : "FALSE");
	printf("   -> EC8 (C8-CF Enable) 512K FFC80000h-FFCFFFFFh: %s\n", ((_BIOS_SPI_BDE *)p)->EC8 ? "TRUE" : "FALSE");
	printf("   -> ED0 (D0-D7 Enable) 512K FFD00000h-FFD7FFFFh: %s\n", ((_BIOS_SPI_BDE *)p)->ED0 ? "TRUE" : "FALSE");
	printf("   -> ED8 (D8-DF Enable) 512K FFD80000h-FFDFFFFFh: %s\n", ((_BIOS_SPI_BDE *)p)->ED8 ? "TRUE" : "FALSE");
	printf("   -> EE0 (E0-E7 Enable) 512K FFE00000h-FFE7FFFFh: %s\n", ((_BIOS_SPI_BDE *)p)->EE0 ? "TRUE" : "FALSE");
	printf("   -> EE8 (E8-EF Enable) 512K FFE80000h-FFEFFFFFh: %s\n", ((_BIOS_SPI_BDE *)p)->EE8 ? "TRUE" : "FALSE");
	printf("   -> EF0 (F0-F7 Enable) 512K FFF00000h-FFF7FFFFh: %s\n", ((_BIOS_SPI_BDE *)p)->EF0 ? "TRUE" : "FALSE");
	printf("   -> EF8 (F8-FF Enable) 512K FFF80000h-FFFFFFFFh: %s\n", ((_BIOS_SPI_BDE *)p)->EF8 ? "TRUE" : "FALSE");
}
void read_spi_interface_bios_decode_enable(DWORD *pBiosSpiBDE) {
	// BIOS Decode Enable (BIOS_SPI_BDE)
	UINT32 offset1 = 0xD8;
	*pBiosSpiBDE = read_pci_dword(0, 31, 5, offset1/sizeof(DWORD));
}

void print_spi_interface_bios_control(DWORD *p) {
	printf("0:31:5_BIOS_SPI_BC - BIOS Control (BIOS_SPI_BC) - Offset DCh\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> WPD (Write Protect Disable):  %s\n", ((_BIOS_SPI_BC *)p)->WPD ? "TRUE" : "FALSE");
	printf("   -> LE (BIOS Lock Enable): %s\n", ((_BIOS_SPI_BC *)p)->LE ? "TRUE" : "FALSE");
	printf("   -> SRC (SPI Read Configuration): %02x\n", ((_BIOS_SPI_BC *)p)->SRC);
	printf("   -> TSS (Top Swap Status): %02x\n", ((_BIOS_SPI_BC *)p)->TSS);
	printf("   -> EISS (Enable InSMM.STS): %s\n", ((_BIOS_SPI_BC *)p)->EISS ? "TRUE" : "FALSE");
	// BBS determines the destination of accesses to the BIOS memory range.
	// Cannot be changed if BILD is set
	printf("   -> BBS (Boot BIOS Strap): %s\n", ((_BIOS_SPI_BC *)p)->BBS ? "LPC" : "SPI");
	printf("   -> BILD (BIOS Interface Lock-Down): %s\n", ((_BIOS_SPI_BC *)p)->BILD ? "TRUE" : "FALSE");
	printf("   -> OSFH (OS Function Hide): %s\n", ((_BIOS_SPI_BC *)p)->OSFH ? "TRUE" : "FALSE");
	printf("   -> ASE_BWP (Async SMI Enable for BIOS Write Protection): %s\n", ((_BIOS_SPI_BC *)p)->ASE_BWP ? "TRUE" : "FALSE");
}
void read_spi_interface_bios_control(DWORD *pBiosCtl) {
	// 
	UINT32 offset1 = 0xDC;
	*pBiosCtl = read_pci_dword(0, 31, 5, offset1/sizeof(DWORD));
}

//////////////////////////////////////////////////////
// POWER MANAGEMENT / ACPI Registers B0:F31:D2


void print_pmc_acpi_base_address(DWORD *p) {
	printf("0:31:2_ABASE - ACPI Base Address (ABASE)\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> ACPI BAR: %08x\n", ((_ABASE *)p)->BA << 8);
	printf("   -> I/O Space: %s\n", ((_ABASE *)p)->STYPE ? "TRUE" : "FALSE");
}

void read_pmc_acpi_base_address(DWORD *pAcpiBase) {
	// ACPI Base Address (ABASE)
	UINT32 offset1 = 0x40;
	*pAcpiBase = read_pci_dword(0, 31, 2, offset1/sizeof(DWORD));
}

void print_pmc_acpi_control(DWORD *p) {
	printf("0:31:2_ACTL - ACPI Control (ACTL)\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> ACPI Enable: %s\n", ((_ACTL *)p)->EN ? "TRUE" : "FALSE");
	printf("   -> PWRM Enable: %s\n", ((_ACTL *)p)->PWRM_EN ? "TRUE" : "FALSE");
}
void read_pmc_acpi_control(DWORD *pAcpiControl) {
	// ACPI Control (ACTL)
	UINT32 offset1 = 0x44;
	*pAcpiControl = read_pci_dword(0, 31, 2, offset1/sizeof(DWORD));
}

void print_pmc_pm_base_address(DWORD *p) {
	printf("0:31:2_PWRMBASE - PM Base Address (PWRMBASE)\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> PWRMBASE: %08xh\n", ((_PWRMBASE *)p)->BA << 8);
	printf("   -> I/O Space: %s\n", ((_PWRMBASE *)p)->STYPE ? "TRUE" : "FALSE");
}
void read_pmc_pm_base_address(DWORD *pPmBaseAddress) {
	// PM Base Address (PWRMBASE)
	UINT32 offset1 = 0x48;
	*pPmBaseAddress = read_pci_dword(0, 31, 2, offset1/sizeof(DWORD));
}

////////////////////////////////////////////////////////
// Hostbridge / DRAM Controller Registers B0:F0:D0

void print_pxpepbar(DWORD64 *p) {
	printf("0:0:0_PXPEPBAR - PCI Express* Egress Port Base Address (PXPEPBAR)\n");
	printf(" => Value: %p\n", *p);
	printf("   -> PXPEPBAR: %08x\n", ((_PEXPEPBAR *)p)->PXPEPBAR << 12);
	BOOL enabled = ((_PEXPEPBAR *)p)->PXPEPBAREN;
	printf("   -> ENABLED: %s\n", enabled ? "TRUE" : "FALSE");
}
void read_hostbridge_pxpepbar(DWORD64 *pPxpepbar) {
	// PCI Express* Egress Port Base Address (PXPEPBAR)
	UINT32 offset1 = 0x40;
	UINT32 offset2 = 0x44;
	DWORD64 low = read_pci_dword(0, 0, 0, offset1/sizeof(DWORD));
	DWORD64 high = read_pci_dword(0, 0, 0, offset2/sizeof(DWORD));
	*pPxpepbar = (high << 0x20) | low;
}

void print_mchbar(DWORD64 *p) {
	printf("0:0:0_MCHBAR - Host Memory Mapped Register Range Base (MCHBAR)\n");
	printf(" => Value: %p\n", *p);
	printf("   -> MCHBAR: %08x\n", ((_MCHBAR *)p)->MCHBAR << 15);
	BOOL enabled = ((_MCHBAR *)p)->MCHBAREN;
	printf("   -> ENABLED: %s\n", enabled ? "TRUE" : "FALSE");
}
void read_hostbridge_mchbar(DWORD64 *pMchbar) {
	// Host Memory Mapped Register Range Base (MCHBAR)
	UINT32 offset1 = 0x48;
	UINT32 offset2 = 0x4C;
	DWORD64 low = read_pci_dword(0, 0, 0, offset1/sizeof(DWORD));
	DWORD64 high = read_pci_dword(0, 0, 0, offset2/sizeof(DWORD));
	*pMchbar = (high << 0x20) | low;
}

void print_gmch(WORD *p) {
	printf("0:0:0_GMCH - GMCH Graphics Control Register (GGC)\n");
	printf(" => Value: %04x\n", *p);
	printf("   -> Device2 Enabled : %s\n", ((_GMCH *)p)->IVD ? "FALSE" : "TRUE");
	printf("   -> LOCKED: %s\n", ((_GMCH *)p)->GGCLCK ? "TRUE" : "FALSE");
	printf("   -> GMS: ");
	int gms = (*p >> 4) & 0xf;
	switch (gms)
	{
	case 0:
		printf(" 0MB");
		break;
	case 1:
		printf(" 32MB");
		break;
	case 2:
		printf(" 64MB");
		break;
	case 3:
		printf(" 96MB");
		break;
	case 4:
		printf(" 128MB");
		break;
	case 5:
		printf(" 160MB");
		break;
	case 6:
		printf(" 192MB");
		break;
	case 7:
		printf(" 224MB");
		break;
	case 8:
		printf(" 256MB");
		break;
	case 9:
		printf(" 288MB");
		break;
	case 10:
		printf(" 320MB");
		break;
	case 11:
		printf(" 352MB");
		break;
	case 12:
		printf(" 384MB");
		break;
	case 13:
		printf(" 416MB");
		break;
	case 14:
		printf(" 448MB");
		break;
	case 15:
		printf(" 480MB");
		break;
	case 16:
		printf(" 512MB");
		break;
	default:
		break;
	}
	printf(" (Gfx STOLEN Memory)\n");
}
void read_hostbridge_gmch(WORD *pGmch) {
	// GMCH Graphics Control Register (GGC)
	UINT32 offset1 = 0x50;
	*pGmch = read_pci_word(0, 0, 0, offset1/2);
}

void print_deven(DWORD *p) {
	printf("0:0:0_DEVEN - Device Enable (DEVEN)\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> D0EN: %s\n", ((_DEVEN *)p)->D0EN ? "TRUE" : "FALSE"); 
	printf("   -> D1F2EN: %s\n", ((_DEVEN *)p)->D1F2EN ? "TRUE" : "FALSE");
	printf("   -> D1F1EN: %s\n", ((_DEVEN *)p)->D1F1EN ? "TRUE" : "FALSE");
	printf("   -> D1F0EN: %s\n", ((_DEVEN *)p)->D1F0EN ? "TRUE" : "FALSE");
	printf("   -> D2EN: %s\n", ((_DEVEN *)p)->D2EN ? "TRUE" : "FALSE");
	printf("   -> D3EN: %s\n", ((_DEVEN *)p)->D3EN ? "TRUE" : "FALSE");
	printf("   -> D4EN: %s\n", ((_DEVEN *)p)->D4EN ? "TRUE" : "FALSE");
	printf("   -> D5EN: %s\n", ((_DEVEN *)p)->D5EN ? "TRUE" : "FALSE");
	printf("   -> D7EN: %s\n", ((_DEVEN *)p)->D7EN ? "TRUE" : "FALSE");
	printf("   -> D8EN: %s\n", ((_DEVEN *)p)->D8EN ? "TRUE" : "FALSE");
}
void read_hostbridge_deven(DWORD *pDeven) {
	// Device Enable (DEVEN)
	UINT32 offset1 = 0x54;
	*pDeven = read_pci_dword(0, 0, 0, offset1/sizeof(DWORD));	
}

void print_pavpc(DWORD *p) {
	printf("0:0:0_PAVPC - Protected Audio Video Path Control (PAVPC)\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> PCMBASE: %08x\n", ((_PAVPC *)p)->PCMBASE<<20);
	printf("   -> PCME: %s\n", ((_PAVPC *)p)->PCME ? "TRUE" : "FALSE");
	printf("   -> PAVPE: %s\n", ((_PAVPC *)p)->PAVPE ? "TRUE" : "FALSE");
	printf("   -> PAVPLCK: %s\n", ((_PAVPC *)p)->PAVPLCK ? "TRUE" : "FALSE");
}

void read_hostbridge_pavpc(DWORD *pPavpc) {
	// Protected Audio Video Path Control (PAVPC)
	UINT32 offset1 = 0x58;
	*pPavpc = read_pci_dword(0, 0, 0, offset1/sizeof(DWORD));
}

void print_dpr(DWORD *p) {
	printf("0:0:0_DPR - DMA Protected Range (DPR)\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> TopOfDPR: %08x\n", ((_DPR *)p)->TopOfDPR << 20);
	printf("   -> DPRSIZE (in MB): %08x\n", ((_DPR *)p)->DPRSIZE);
	printf("   -> DMA Requests Blocked: %s\n", ((_DPR *)p)->EPM ? "TRUE" : "FALSE");
	printf("   -> DPR Protection Enabled: %s\n", ((_DPR *)p)->PRS ? "TRUE" : "FALSE");
	printf("   -> LOCK: %s\n", ((_DPR *)p)->LOCK ? "TRUE" : "FALSE");
}
void read_hostbridge_dpr(DWORD *pDpr) {
	// DMA Protected Range (DPR)
	UINT32 offset1 = 0x5C;
	*pDpr = read_pci_dword(0, 0, 0, offset1/sizeof(DWORD));
}

void print_pciexbar(DWORD64 *p) {
	printf("0:0:0_PCIEXBAR - PCI Express Register Range Base Address (PCIEXBAR)\n");
	printf(" => Value: %p\n", *p);
	printf("   -> PCIEXBAR: %08x\n", ((_PCIEXBAR *)p)->PCIEXBAR << 28);
	switch (((_PCIEXBAR *)p)->LENGTH) {
		case 0:
			printf("   -> LENGTH: 256MB\n");
			break;
		case 1:
			printf("   -> LENGTH: 128MB\n");
			break;
		case 2:
			printf("   -> LENGTH: 64MB\n");
			break;
		case 3:
			printf("   -> LENGTH: Reserved\n");
			break;
	}		
	printf("   -> PCIEXBAREN: %s\n", ((_PCIEXBAR *)p)->PCIEXBAREN ? "TRUE" : "FALSE");
}

void read_hostbridge_pciexbar(DWORD64 *pPciexbar) {
	// PCI Express Register Range Base Address (PCIEXBAR)
	UINT32 offset1 = 0x60;
	UINT32 offset2 = 0x64;
	DWORD64 low = read_pci_dword(0, 0, 0, offset1/sizeof(DWORD));
	DWORD64 high = read_pci_dword(0, 0, 0, offset2/sizeof(DWORD));
	*pPciexbar = (high << 0x20) | low;
}

void print_dmibar(DWORD64 *p) {
	printf("0:0:0_DMIBAR - Root Complex Register Range Base Address (DMIBAR)\n");
	printf(" => Value: %p\n", *p);
	printf("   -> DMIBAR: %08x\n", ((_DMIBAR *)p)->DMIBAR << 12);
	printf("   -> DMIBAREN: %s\n", ((_DMIBAR *)p)->DMIBAREN ? "TRUE" : "FALSE");
}

void read_hostbridge_dmibar(DWORD64 *pDmibar) {
	// Root Complex Register Range Base Address (DMIBAR)
	UINT32 offset1 = 0x68;
	UINT32 offset2 = 0x6C;
	DWORD64 low = read_pci_dword(0, 0, 0, offset1/sizeof(DWORD));
	DWORD64 high = read_pci_dword(0, 0, 0, offset2/sizeof(DWORD));
	*pDmibar = (high << 0x20) | low;
}

void print_mesegbase(DWORD64 *p) {
	printf("0:0:0_MESEG_BASE - Manageability Engine Base Address Register (MESEG)\n");
	printf(" => Value: %p\n", *p);
	printf("    -> MEBASE: %08x\n", ((_MESEG_BASE *)p)->MEBASE << 20);	
}

void read_hostbridge_meseg_base(DWORD64 *pMsegBase) {
	// Manageability Engine Base Address Register (MESEG)
	UINT32 offset1 = 0x70;
	UINT32 offset2 = 0x74;
	DWORD64 low = read_pci_dword(0, 0, 0, offset1/sizeof(DWORD));
	DWORD64 high = read_pci_dword(0, 0, 0, offset2/sizeof(DWORD));
	*pMsegBase = (high << 0x20) | low;
}

void print_meseglimit(DWORD64 *p) {
	printf("0:0:0_MESEG_LIMIT - Manageability Engine Limit Address Register (MESEG)\n");
	printf(" => Value: %p\n", *p);
	printf("   -> MEMASK: %08x\n", ((_MESEG_LIMIT *)p)->MEMASK << 20);
	printf("   -> ME_STLEN_EN: %s\n", ((_MESEG_LIMIT *)p)->ME_STLEN_EN ? "TRUE" : "FALSE");
	printf("   -> MELCK: %s\n", ((_MESEG_LIMIT *)p)->MELCK ? "TRUE" : "FALSE");
}

void read_hostbridge_meseg_limit(DWORD64 *pMsegLimit) {
	// Manageability Engine Limit Address Register (MESEG)
	UINT32 offset1 = 0x78;
	UINT32 offset2 = 0x7C;
	DWORD64 low = read_pci_dword(0, 0, 0, offset1/sizeof(DWORD));
	DWORD64 high = read_pci_dword(0, 0, 0, offset2/sizeof(DWORD));
	*pMsegLimit = (high << 0x20) | low;
}

void print_smramc(BYTE *p) {
	printf("0:0:0_SMRAMC - System Management RAM Control (SMRAMC)\n");
	printf(" => Value: %02x\n", *p);
	printf("   -> C_BASE_SEG: %02x\n", ((_SMRAMC *)p)->C_BASE_SEG );	
	printf("   -> G_SMRAME (Compatible SMRAM): %s\n", ((_SMRAMC *)p)->G_SMRAME ? "TRUE" : "FALSE");
	printf("   -> D_LCK: %s\n", ((_SMRAMC *)p)->D_LCK ? "TRUE" : "FALSE");
	printf("   -> D_CLS: %s\n", ((_SMRAMC *)p)->D_CLS ? "TRUE" : "FALSE");
	printf("   -> D_OPEN: %s\n", ((_SMRAMC *)p)->D_OPEN ? "TRUE" : "FALSE");
}

void read_hostbridge_smramc(BYTE *pSmramc) {
	// System Management RAM Control (SMRAMC)
	UINT32 offset1 = 0x88;
	*pSmramc = read_pci_byte(0, 0, 0, offset1);
}

void print_remapbase(DWORD64 *p) {
	printf("0:0:0_REMAPBASE - Remap Base Address Register (REMAPBASE)\n");
	printf(" => Value: %p\n", *p);
	printf("   -> REMAPBASE: %08x\n", ((_REMAPBASE *)p)->REMAPBASE<<20);
	printf("   -> LOCK: %s\n", ((_REMAPBASE *)p)->LOCK ? "TRUE" : "FALSE");	
}

void read_hostbridge_remap_base(DWORD64 *pRemapBase) {
	// Remap Base Address Register (REMAPBASE)
	UINT32 offset1 = 0x90;
	UINT32 offset2 = 0x94;
	DWORD64 low = read_pci_dword(0, 0, 0, offset1/sizeof(DWORD));
	DWORD64 high = read_pci_dword(0, 0, 0, offset2/sizeof(DWORD));
	*pRemapBase = (high << 0x20) | low;
}

void print_remaplimit(DWORD64 *p) {
	printf("0:0:0_REMAPLIMIT - Remap Limit Address Register (REMAPLIMIT)\n");
	printf(" => Value: %p\n", *p);
	printf("   -> REMAPLMT: %08x\n", ((_REMAPLIMIT *)p)->REMAPLMT<<20);
	printf("   -> LOCK: %s\n", ((_REMAPLIMIT *)p)->LOCK ? "TRUE" : "FALSE");
}

void read_hostbridge_remap_limit(DWORD64 *pRemapLimit) {
	// Remap Limit Address Register (REMAPLIMIT)
	UINT32 offset1 = 0x98;
	UINT32 offset2 = 0x9C;
	DWORD64 low = read_pci_dword(0, 0, 0, offset1/sizeof(DWORD));
	DWORD64 high = read_pci_dword(0, 0, 0, offset2/sizeof(DWORD));
	*pRemapLimit = (high << 0x20) | low;
}

void print_tom(DWORD64 *p) {
	printf("0:0:0_TOM - Top of Memory (TOM)\n");
	printf(" => Value: %p\n", *p);
	printf("   -> TOM: %ph\n", ((_TOM *)p)->TOM << 20);
	printf("   -> LOCK: %s\n", ((_TOM *)p)->LOCK ? "TRUE" : "FALSE");
}

void read_hostbridge_tom(DWORD64 *pTom) {
	// Top of Memory (TOM)
	UINT32 offset1 = 0xA0;
	UINT32 offset2 = 0xA4;
	DWORD64 low = read_pci_dword(0, 0, 0, offset1/sizeof(DWORD));
	DWORD64 high = read_pci_dword(0, 0, 0, offset2/sizeof(DWORD));
	*pTom = (high << 0x20) | low;
}

void print_touud(DWORD64 *p) {
	printf("0:0:0_TOUUD - Top of Upper Usable DRAM (TOUUD)\n");
	printf(" => Value: %p\n", *p);
	printf("   -> TOUUD: %ph\n", ((_TOUUD *)p)->TOUUD << 20);
	printf("   -> LOCK: %s\n", ((_TOUUD *)p)->LOCK ? "TRUE" : "FALSE");
}

void read_hostbridge_touud(DWORD64 *pTouud) {
	// Top of Upper Usable DRAM (TOUUD)
	UINT32 offset1 = 0xA8;
	UINT32 offset2 = 0xAC;
	DWORD64 low = read_pci_dword(0, 0, 0, offset1/sizeof(DWORD));
	DWORD64 high = read_pci_dword(0, 0, 0, offset2/sizeof(DWORD));
	*pTouud = (high << 0x20) | low;
}

void print_bdsm(DWORD *p) {
	printf("0:0:0_BDSM - Base Data of Stolen Memory (BDSM)\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> BDSM: %08xh\n", ((_BDSM *)p)->BDSM << 20);
	printf("   -> LOCK: %s\n", ((_BDSM *)p)->LOCK ? "TRUE" : "FALSE");
}

void read_hostbridge_bdsm(DWORD *pBdsm) {
	// Base Data of Stolen Memory (BDSM)
	UINT32 offset1 = 0xB0;
	*pBdsm = read_pci_dword(0, 0, 0, offset1/sizeof(DWORD));
}

void print_bgsm(DWORD *p) {
	printf("0:0:0_BGSM - Base of GTT Stolen Memory (BGSM)\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> BGSM: %08xh\n", ((_BGSM *)p)->BGSM << 20);
	printf("   -> LOCK: %s\n", ((_BGSM *)p)->LOCK ? "TRUE" : "FALSE");
}
void read_hostbridge_bgsm(DWORD *pBgsm) {
	// Base of GTT Stolen Memory (BGSM)
	UINT32 offset1 = 0xB4;
	*pBgsm = read_pci_dword(0, 0, 0, offset1/sizeof(DWORD));
}

void print_tsegmb(DWORD *p) {
	/*
	This is the segment that lies in the top of main memory range below the 4GB limit. 
	The start of this memory range is determined by the value of the TSEG memory base (TSEGMB)
	register in the hostbridge. Contents of this segment can only be seen when the CPU is 
	running in system management mode (SMM). Therefore, code running outside of SMM doesn’t
	have visibility of this range in RAM, even the operating system code.
	This segment stores the runtime data and code of the platform firmware.
	*/
	printf("0:0:0_TSEGMB - TSEG Memory Base (TSEGMB) (Top of Main Memory Segment)\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> TSEGMB: %08xh\n", ((_TSEGMB *)p)->TSEGMB << 20);
	printf("   -> LOCK: %s\n", ((_TSEGMB *)p)->LOCK ? "TRUE" : "FALSE");
}
void read_hostbridge_tseg(DWORD *pTseg) {
	// TSEG Memory Base (TSEGMB)
	UINT32 offset1 = 0xB8;
	*pTseg = read_pci_dword(0, 0, 0, offset1/sizeof(DWORD));
}

void print_tolud(DWORD *p) {
	printf("0:0:0_TOLUD - Top of Low Usable DRAM (TOLUD)\n");
	printf(" => Value: %08x\n", *p);
	printf("   -> TOLUD: %08xh\n", ((_TOLUD *)p)->TOLUD << 20);
	printf("   -> LOCK: %s\n", ((_TOLUD *)p)->LOCK ? "TRUE" : "FALSE");
}
void read_hostbridge_tolud(DWORD *pTolud) {
	// Top of Low Usable DRAM (TOLUD)
	UINT32 offset1 = 0xBC;
	*pTolud = read_pci_dword(0, 0, 0, offset1/sizeof(DWORD));
}

//////////////////////////////////////////////////////////////

void get_chipset_id(
	WORD *platformVID,
	WORD *platformDID,
	WORD *PCHVID,
	WORD *PCHDID)
{
	DWORD platform = read_pci_dword(0, 0, 0, 0/sizeof(DWORD));
	*platformVID = platform & 0xFFFF;
	*platformDID = platform >> 0x10;
	DWORD pch = read_pci_dword(0, 0x1f, 0, 0/sizeof(DWORD));
	*PCHVID = pch & 0xFFFF;
	*PCHDID = pch >> 0x10;
}





void check_smm_msr_feature_control() {

	UINT64 smm_feature_control = 0;
	if (do_read_msr(MSR_SMM_FEATURE_CONTROL, &smm_feature_control)) {

		printf("-> MSR_SMM_FEATURE_CONTROL (4E0h): %016lx\n", smm_feature_control);
		if (smm_feature_control & 1) {
			printf("    -> LOCKED\n");
		}
		else {
			printf("    -> UNLOCKED\n");
		}
		if (smm_feature_control & 4) {
			printf("    -> SMM_Code_Chk_En: TRUE\n");
		}
		else {
			printf("    -> SMM_Code_Chk_En: FALSE\n");
		}
	}
}

void check_ia32_msr_feature_control() {
	/*
	If the lock bit (bit 0) isn't set, then
	IA32_SMRR_PHYSBASE_MSR and IA32_SMRR_PHYSMASK_MSR
	can be changed.
	If the Lock bit is set then those can only be changed while in SMM
	*/
	UINT64 msr_feature_control = 0;
	if (do_read_msr(IA32_FEATURE_CONTROL, &msr_feature_control)) {

		printf("-> MSR_FEATURE_CONTROL (3Ah): %016lx\n", msr_feature_control);
		if (msr_feature_control & 1) {
			printf("    -> LOCKED\n");
		}
		else {
			printf("    -> UNLOCKED\n");
		}
		if (msr_feature_control & 4) {
			printf("    -> SMRR Enabled: TRUE\n");
		}
		else {
			printf("    -> SMRR Enabled: FALSE\n");
		}
	}
}

void check_memory_lock_msr() {
	/*
	If the lock bit (bit 0) isn't set, then
	IOBAS and IOLIM registers can change
	*/

	UINT64 mt_lock_msr = 0;
	if (do_read_msr(LT_MEMORY_LOCK_MSR, &mt_lock_msr)) {
		printf("-> LT_MEMORY_LOCK_MSR (2E7h): %016lx\n", mt_lock_msr);
		if (mt_lock_msr & 1) {
			printf("    -> LOCKED\n");
		}
		else {
			printf("    -> UNLOCKED\n");
		}
	}
}

void get_smrr(UINT64 *out_smrambase, UINT32 *out_smramsize) {

	UINT64 smrr_physbase = 0;
	UINT64 smrrmask = 0;

	if (do_read_msr(IA32_SMRR_PHYSBASE_MSR, &smrr_physbase)
		&& do_read_msr(IA32_SMRR_PHYSMASK_MSR, &smrrmask))
	{
		UINT32 smram_base = smrr_physbase & smrrmask;
		UINT32 smram_size = ((~smrrmask) & 0xFFFFFFFF) + 1;
		UINT32 smram_limit = smram_base + smram_size - 1;
		
		printf("-> IA32_SMRR_PHYSBASE_MSR: %08x\n", smrr_physbase);
		printf("-> IA32_SMRR_PHYSMASK_MSR: %08x\n", smrrmask);
		printf("-> SMRAM base: %08x\n", smram_base);
		printf("-> SMRAM size: %08x\n", smram_size);
		printf("-> SMRAM limit: %08x\n", smram_limit);
		if (out_smrambase != NULL)
			*out_smrambase = smram_base;
		if (out_smramsize != NULL)
			*out_smramsize = smram_size;
	}
}


void intel_get_chipset_information() {
	WORD PlatformVID, PlatformDID = 0;
	WORD PCHVID, PCHDID = 0;
	WORD GMCH = 0;
	BYTE SMRAMC;
	DWORD64 PXPEPBAR, PCIEXBAR, MCHBAR, DMIBAR, MSEGBASE, MSEGLIMIT = 0;
	DWORD64 REMAPBASE, REMAPLIMIT, TOM, TOUUD = 0;
	DWORD DEVEN, PAVPC, DPR, BDSM, BGSM, TSEGMB, TOLUD = 0;
	DWORD cpu_info[4];
	UINT64 smram_base;
	DWORD smram_limit;
	DWORD ACPI_BAR, PWRM_BAR, ACPI_CTL = 0;
	DWORD SPIBar0Mmio, BiosSPIBDE, BiosCtl = 0;
	DWORD BFPREG, HSFS_CTL, BIOS_FADDR, BIOS_DLOCK = 0;
	DWORD FREG0, FREG1, FREG2, FREG3, FREG4, FREG5 = 0;
	DWORD FPR0, FPR1, FPR2, FPR3, FPR4 = 0;

	get_chipset_id(&PlatformVID, &PlatformVID, &PCHVID, &PCHDID);
	read_hostbridge_pxpepbar(&PXPEPBAR);
	read_hostbridge_mchbar(&MCHBAR);
	read_hostbridge_gmch(&GMCH);
	read_hostbridge_deven(&DEVEN);
	read_hostbridge_pavpc(&PAVPC);
	read_hostbridge_dpr(&DPR);
	read_hostbridge_pciexbar(&PCIEXBAR);
	read_hostbridge_dmibar(&DMIBAR);
	read_hostbridge_meseg_base(&MSEGBASE);
	read_hostbridge_meseg_limit(&MSEGLIMIT);
	read_hostbridge_smramc(&SMRAMC);
	read_hostbridge_remap_base(&REMAPBASE);
	read_hostbridge_remap_limit(&REMAPLIMIT);
	read_hostbridge_tom(&TOM);
	read_hostbridge_touud(&TOUUD);
	read_hostbridge_bdsm(&BDSM);
	read_hostbridge_bgsm(&BGSM);
	read_hostbridge_tseg(&TSEGMB);
	read_hostbridge_tolud(&TOLUD);
	get_smrr(&smram_base, (UINT32 *)&smram_limit);

	#ifdef __linux__ 
		__get_cpuid(1, &cpu_info[0], &cpu_info[1], &cpu_info[2], &cpu_info[3]);
	#elif _WIN32				
		__cpuid((int *)&cpu_info, 1);
	#else
	#endif	
	
	BYTE SteppingID = cpu_info[0] & 0xF;
	BYTE ModelNumber = (cpu_info[0]>>4) & 0xF;
	BYTE FamilyCode = (cpu_info[0] >> 8) & 0xF;
	BYTE ProcessorType = (cpu_info[0] >> 12) & 0x3;
	BYTE ExtendedModel = (cpu_info[0] >> 0x10) & 0xF;
	BYTE ExtendedFamily = (cpu_info[0] >> 0x14) & 0xFF;

	read_pmc_acpi_base_address(&ACPI_BAR);
	read_pmc_pm_base_address(&PWRM_BAR);
	read_pmc_acpi_control(&ACPI_CTL);

	read_spi_interface_bar0_mmio(&SPIBar0Mmio);
	read_spi_interface_bios_decode_enable(&BiosSPIBDE);
	read_spi_interface_bios_control(&BiosCtl);

	void *mapped_spi_bar = map_physical_memory(SPIBar0Mmio, PAGE_SIZE);

	BFPREG = *((DWORD *) ((volatile char *)mapped_spi_bar + SPI_BIOS_MMIO_BIOS_BFPREG_OFFSET));
	HSFS_CTL = *((DWORD *) ((volatile char *)mapped_spi_bar + SPI_BIOS_MMIO_BIOS_HSFSTS_CTL));
	BIOS_FADDR = *((DWORD *) ((volatile char *)mapped_spi_bar + SPI_BIOS_MMIO_BIOS_FADDR));
	BIOS_DLOCK = *((DWORD *) ((volatile char *)mapped_spi_bar + SPI_BIOS_MMIO_BIOS_DLOCK));
	FREG0 = *((DWORD *) ((volatile char *)mapped_spi_bar + SPI_BIOS_MMIO_BIOS_FREG0));
	FREG1 = *((DWORD *) ((volatile char *)mapped_spi_bar + SPI_BIOS_MMIO_BIOS_FREG1));
	FREG2 = *((DWORD *) ((volatile char *)mapped_spi_bar + SPI_BIOS_MMIO_BIOS_FREG2));
	FREG3 = *((DWORD *) ((volatile char *)mapped_spi_bar + SPI_BIOS_MMIO_BIOS_FREG3));
	FREG4 = *((DWORD *) ((volatile char *)mapped_spi_bar + SPI_BIOS_MMIO_BIOS_FREG4));
	FREG5 = *((DWORD *) ((volatile char *)mapped_spi_bar + SPI_BIOS_MMIO_BIOS_FREG5));
	FPR0 = *((DWORD *) ((volatile char *)mapped_spi_bar + SPI_BIOS_MMIO_BIOS_FPR0));
	FPR1 = *((DWORD *) ((volatile char *)mapped_spi_bar + SPI_BIOS_MMIO_BIOS_FPR1));
	FPR2 = *((DWORD *) ((volatile char *)mapped_spi_bar + SPI_BIOS_MMIO_BIOS_FPR2));
	FPR3 = *((DWORD *) ((volatile char *)mapped_spi_bar + SPI_BIOS_MMIO_BIOS_FPR3));
	FPR4 = *((DWORD *) ((volatile char *)mapped_spi_bar + SPI_BIOS_MMIO_BIOS_FPR4));

	unmap_physical_memory(mapped_spi_bar, PAGE_SIZE);

	

	printf("-----------------------------------------\n");
	printf("ProcessorType: %02x\n", ProcessorType);
	printf("SteppingID: %02x\n", SteppingID);
	printf("ModelNumber: %02x\n", ModelNumber);
	printf("ExtendedModel: %02x\n", ExtendedModel);
	printf("FamilyCode: %02x\n", FamilyCode);
	printf("ExtendedFamily: %02x\n", ExtendedFamily);
	printf("-----------------------------------------\n");
	printf("+] Processor\n");
	printf(" -> VID: %04x\n", PlatformVID);
	printf(" -> DID: %04x\n", PlatformDID);
	printf("+] PCH\n");
	printf(" -> VID: %04x\n", PCHVID);
	printf(" -> DID: %04x\n", PCHDID);
	printf("-----------------------------------------\n");
	printf(" -> SMRR SMRAM Base: %08x - Limit: %08x\n", smram_base, smram_limit);
	check_memory_lock_msr();	
	check_ia32_msr_feature_control();
	check_smm_msr_feature_control();
	printf("-----------------------------------------\n");
	printf("+] HostBridge / DRAM Controller Registers\n");
	print_pxpepbar(&PXPEPBAR);
	print_mchbar(&MCHBAR);
	print_gmch(&GMCH);
	print_deven(&DEVEN);
	print_pavpc(&PAVPC);
	print_dpr(&DPR);
	print_pciexbar(&PCIEXBAR);
	print_dmibar(&DMIBAR);
	print_mesegbase(&MSEGBASE);
	print_meseglimit(&MSEGLIMIT);
	print_smramc(&SMRAMC);
	print_remapbase(&REMAPBASE);
	print_remaplimit(&REMAPLIMIT);	
	print_tom(&TOM);
	print_touud(&TOUUD);
	print_bdsm(&BDSM);
	print_bgsm(&BGSM);
	print_tsegmb(&TSEGMB);
	print_tolud(&TOLUD);
	printf("-----------------------------------------\n");	
	printf("+] Power Management / ACPI\n");
	print_pmc_acpi_base_address(&ACPI_BAR);
	print_pmc_pm_base_address(&PWRM_BAR);
	print_pmc_acpi_control(&ACPI_CTL);
	printf("-----------------------------------------\n");
	printf("+] BIOS - SPI Interface\n");
	print_spi_interface_bar0_mmio(&SPIBar0Mmio);
	print_spi_interface_bios_decode_enable(&BiosSPIBDE);
	print_spi_interface_bios_control(&BiosCtl);
	printf("-----------------------------------------\n");
	printf("+] BIOS - SPI BAR\n");
	print_spi_bar_bios_bfpreg(&BFPREG);
	//print_spi_bar_faddr(&BIOS_FADDR);
	//print_spi_bar_dlock(&BIOS_DLOCK);
	print_spi_bar_freg0(&FREG0);
	print_spi_bar_freg1(&FREG1);
	print_spi_bar_freg2(&FREG2);
	print_spi_bar_freg3(&FREG3);
	print_spi_bar_freg4(&FREG4);
	print_spi_bar_freg5(&FREG5);

	print_spi_bar_hsfs_ctl(&HSFS_CTL);
	

	print_spi_bar_fpr0(&FPR0);
	print_spi_bar_fpr1(&FPR1);
	print_spi_bar_fpr2(&FPR2);
	print_spi_bar_fpr3(&FPR3);
	print_spi_bar_fpr4(&FPR4);
}



int _wait_spi_cycle(volatile _BIOS_HSFSTS_CTL *hsfsts_ctl) {
	/*
		H_SCPI: Hardware sets this bit when software sets the  
			Flash Cycle Go (FGO) bit in the Hardware Sequencing Flash Control register.
			This bit remains set until the cycle completes on the SPI interface. 
	*/
	int cycle_done = 0;

	int iterations = 500;
	while (iterations > 0) {
		cycle_done = !(hsfsts_ctl->H_SCPI);
		if ( cycle_done ) {
			break;			
		}
		printf("Waiting for SPI Cycle to finish...\n");
#ifdef __linux__
		usleep(100);
#else
		Sleep(100);
#endif
		iterations--;
	}

	if (cycle_done) {
		printf("Clear Access Error Log (H_AEL)\n");
		hsfsts_ctl->H_AEL = 1;
		printf("Clear Flash Cycle Error (FCERR)\n");
		hsfsts_ctl->FCERR = 1;
		printf("Clear Flash Cycle Done (FDONE)\n");

		/*
			FDONE: The PCH sets this bit to 1 when the SPI Cycle completes 
					after sw previously set the FGO bit.
		*/

		hsfsts_ctl->FDONE = 1;
	}

	if (hsfsts_ctl->H_AEL == 1 || hsfsts_ctl-> FCERR == 1) {
		printf("error H_AEL or FCERR -> SPI not ready\n");
		return 0;
	}

	return 1;
}

void intel_dump_spi_flash(const char *output_filename) {
	DWORD SPIBar0Mmio = 0;;
	DWORD BFPREG = 0;

	read_spi_interface_bar0_mmio(&SPIBar0Mmio);
	DWORD64 SPIBAR_ADDR = SPIBar0Mmio;

	// Map the SPIBAR here
	char *spi_bar = (char *) map_physical_memory(SPIBAR_ADDR, 0x1000);

	BFPREG = *((DWORD *) ((volatile char *)spi_bar + SPI_BIOS_MMIO_BIOS_BFPREG_OFFSET));

	UINT flash_base  = ((_BIOS_BFPREG *)&BFPREG)->PRB << 12;
	UINT flash_limit = ((_BIOS_BFPREG *)&BFPREG)->PRL << 12;
	UINT flash_size  = flash_limit - flash_base;
	printf("   -> PRB (BIOS Flash Primary Region Base) : %#08x\n", flash_base);
	printf("   -> PRL (BIOS Flash Primary Region Limit): %#08x\n", flash_limit);
	printf("   -> Flash region size: %#08x\n", flash_size);

	DWORD *content = (DWORD *) calloc(1, flash_size);
	if (!content) {
		printf("[intel_dump_spi_flash] error allocating memory\n");
		return;
	}

	
	if (spi_bar == NULL) {
		printf("    -> error mapping SPIBAR\n");
	}
	printf("Mapped SPIBAR: %p\n", spi_bar);
	volatile _BIOS_HSFSTS_CTL *hsfsts_ctl = (_BIOS_HSFSTS_CTL *) (spi_bar + 4);
	volatile _BIOS_FADDR *faddr           = (_BIOS_FADDR *) (spi_bar + 8);
	volatile DWORD *fdata0				 = (DWORD *) (spi_bar + 0x10);
	volatile DWORD *fdata1				 = (DWORD *) (spi_bar + 0x14);
	volatile DWORD *fdata2				 = (DWORD *) (spi_bar + 0x18);
	volatile DWORD *fdata3				 = (DWORD *) (spi_bar + 0x1C);
	volatile DWORD *fdata4				 = (DWORD *) (spi_bar + 0x20);
	volatile DWORD *fdata5				 = (DWORD *) (spi_bar + 0x24);
	volatile DWORD *fdata6				 = (DWORD *) (spi_bar + 0x28);
	volatile DWORD *fdata7				 = (DWORD *) (spi_bar + 0x2C);
	volatile DWORD *fdata8				 = (DWORD *) (spi_bar + 0x30);
	volatile DWORD *fdata9				 = (DWORD *) (spi_bar + 0x34);
	volatile DWORD *fdata10				 = (DWORD *) (spi_bar + 0x38);
	volatile DWORD *fdata11				 = (DWORD *) (spi_bar + 0x3C);
	volatile DWORD *fdata12				 = (DWORD *) (spi_bar + 0x40);
	volatile DWORD *fdata13				 = (DWORD *) (spi_bar + 0x44);
	volatile DWORD *fdata14				 = (DWORD *) (spi_bar + 0x48);
	volatile DWORD *fdata15				 = (DWORD *) (spi_bar + 0x4C);

	int r = _wait_spi_cycle(hsfsts_ctl);
	if (!r) {
		printf("error 1\n");
		return;
	}

	const int DATA_CHUNK = 64; // 16 FDATAx Regs of 4 bytes each
	int chunks = flash_size / DATA_CHUNK;
	int i = 0;
	while (i < chunks) {
		printf("reading chunk %d\n", i);
		faddr->FLA = (flash_base + DATA_CHUNK * i) & 0x7FFFFFF;
		
		hsfsts_ctl->FDBC   = DATA_CHUNK - 1; // 0x3f = 64 bytes (0 based)
		hsfsts_ctl->FCYCLE = 0; // READ OPERATION
		hsfsts_ctl->FGO    = 1;

		r = _wait_spi_cycle(hsfsts_ctl);
		if (!r) {
			printf("SPI flash read failed: %#08x\n", faddr->FLA);
			return;
		}

		printf("SPI Cycle done: read 64 bytes from %#08x\n", faddr->FLA);

		int index = i * 16;
		content[index]    = (*fdata0);
		content[index+1]  = (*fdata1);
		content[index+2]  = (*fdata2);
		content[index+3]  = (*fdata3);
		content[index+4]  = (*fdata4);
		content[index+5]  = (*fdata5);
		content[index+6]  = (*fdata6);
		content[index+7]  = (*fdata7);
		content[index+8]  = (*fdata8);
		content[index+9]  = (*fdata9);
		content[index+10] = (*fdata10);
		content[index+11] = (*fdata11);
		content[index+12] = (*fdata12);
		content[index+13] = (*fdata13);
		content[index+14] = (*fdata14);
		content[index+15] = (*fdata15);

		i++;
	}

	printf(" -> finished reading flash memory -> dumping to %s\n", output_filename);
		
	FILE *fp = fopen(output_filename, "wb");
	fwrite(content, flash_size, 1, fp);
	fclose(fp);

	// Unmap SPIBAR
	unmap_physical_memory(spi_bar, 0x100);
}
