#include "intel_gpio.h"
#include "physmem.h"
#include "Util.h"
#include "pci.h"

GPIO_PAD_CFG GPIO_PAD_CFG_GPP_S[8];
GPIO_PAD_CFG GPIO_PAD_CFG_GPP_E[22];
GPIO_PAD_CFG GPIO_PAD_CFG_GPP_K[12];
GPIO_PAD_CFG GPIO_PAD_CFG_GPP_F[24];


void print_pad_cfg_reg(GPIO_PAD_CFG *reg) {
    printf("  GPIOTXSTATE: %u\n", reg->GPIOTXSTATE);
    printf("  GPIORXSTATE: %u\n", reg->GPIORXSTATE);
    printf("  GPIOTXDIS: %u\n", reg->GPIOTXDIS);
    printf("  GPIORXDIS: %u\n", reg->GPIORXDIS);
    printf("  PMODE: %u\n", reg->PMODE);
    printf("  GPIROUTNMI: %u\n", reg->GPIROUTNMI);
    printf("  GPIROUTSMI: %u\n", reg->GPIROUTSMI);
    printf("  GPIROUTSCI: %u\n", reg->GPIROUTSCI);
    printf("  GPIROUTIOXAPIC: %u\n", reg->GPIROUTIOXAPIC);
    printf("  RXTXENCFG: %u\n", reg->RXTXENCFG);
    printf("  RXINV: %u\n", reg->RXINV);
    printf("  PREGFRXSEL: %u\n", reg->PREGFRXSEL);
    printf("  RXEVCFG: %u\n", reg->RXEVCFG);
    printf("  RXRAW1: %u\n", reg->RXRAW1);
    printf("  RXPADSTSEL: %u\n", reg->RXPADSTSEL);
    printf("  PADRSTCFG: %u\n", reg->PADRSTCFG);
    printf("  INTSEL: %u\n", reg->INTSEL);
    printf("  IOSTREM: %u\n", reg->IOSTREM);
    printf("  TERM: %u\n", reg->TERM);
    printf("  IOSTATE: %u\n", reg->IOSTATE);     
}  


void retrieve_gpio_gpp_f(struct GpioGroupSettings *pad) {

    char *gpio_mem = (char *) map_physical_memory(SBREG_BAR | (INTEL_PCR_ID_GPIOCOM4 << 0x10), PAGE_SIZE );

    UINT32 *dw_ptr;

    pad->PadCfgLock        = *(UINT32 *) (gpio_mem + GPIO_PADCFGLOCK_GPP_F_0);
    pad->PadCfgLockTxState = *(UINT32 *) (gpio_mem + GPIO_PADCFGLOCKTX_GPP_F_0);

    dw_ptr = (UINT32 *) (gpio_mem + GPIO_PAD_CFG_DW0_GPP_F_0);
    for (int i = 0 ; i < GPIO_GPP_F_COUNT; i++) {
        pad->GpioConfig[i].dw0 = dw_ptr[0];
        pad->GpioConfig[i].dw1 = dw_ptr[1];
        // printf("DW0:[%08x] - DW1:[%08x]\n,", dw_ptr[0], dw_ptr[1]);
        dw_ptr+= 4;
    }
    // print_pad_cfg_reg( (GPIO_PAD_CFG *)(gpio_mem + GPIO_PAD_CFG_DW0_GPP_F_15));
    // 02 01 00 84 32 00 00 00 00 09 00 00 00 00 00 00

    unmap_physical_memory(gpio_mem, PAGE_SIZE);
}

UINT32 PchSbMmioRead32(UINT32 community, UINT32 regOffset) {
    UINT32 val = 0;
    UINT64 pcr_reg = SBREG_BAR | (community << 0x10);
    char *gpio_mem = (char *) map_physical_memory(pcr_reg, PAGE_SIZE);
    debug_print("PchSbMmioRead32 at %016lx\n", pcr_reg + regOffset);
    val = *(UINT32 *)(gpio_mem + regOffset);    
    unmap_physical_memory(gpio_mem, PAGE_SIZE);
    return val;
}

void PchSbMmioWrite32(UINT32 community, UINT32 regOffset, UINT32 val) {    
    char *gpio_mem = (char *) map_physical_memory(SBREG_BAR | (community << 0x10), PAGE_SIZE);
    *(UINT32 *)(gpio_mem + regOffset) = val;
    unmap_physical_memory(gpio_mem, PAGE_SIZE);    
}


void PchSbMmioAndThenOr32(UINT32 community, UINT32 regOffset, UINT32 andMask, UINT32 orValue)
{
    char *gpio_mem = (char *) map_physical_memory(SBREG_BAR | (community << 0x10), PAGE_SIZE);
    UINT32 *address = (UINT32 *)(gpio_mem + regOffset);
    UINT32 currentValue = *address;  // Read the current value from the MMIO register
    UINT32 newValue = (currentValue & andMask) | orValue;  // Apply the AND and OR operations
    *address = newValue;  // Write the new value back to the MMIO register
    unmap_physical_memory(gpio_mem, PAGE_SIZE);    
}


void print_gpio_community4_mem() {

    char *gpio_mem = (char *) map_physical_memory(SBREG_BAR | (INTEL_PCR_ID_GPIOCOM4 << 0x10), PAGE_SIZE );

    print_memory(0, gpio_mem, PAGE_SIZE);

    unmap_physical_memory(gpio_mem, PAGE_SIZE);

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
.data:0000000000008D00 TargetGroupInfoArray1 GPIO_GROUP_INFO <6Eh, 20h, 0B0h, 100h, 120h, 140h, 160h, 180h, 1A0h, \
.data:0000000000008D00                                         ; DATA XREF: GpioGetGroupInfoTable+13o
.data:0000000000008D00                                  1C0h, 1E0h, 80h, 84h, 700h, 1Ah>
.data:0000000000008D00                 GPIO_GROUP_INFO <6Eh, 30h, 0B4h, 104h, 124h, 144h, 164h, 0FFFFh, \
.data:0000000000008D00                                  0FFFFh, 0FFFFh, 0FFFFh, 88h, 8Ch, 8A0h, 10h>
.data:0000000000008D00                 GPIO_GROUP_INFO <6Eh, 38h, 0B8h, 108h, 128h, 148h, 168h, 0FFFFh, \
.data:0000000000008D00                                  0FFFFh, 0FFFFh, 0FFFFh, 90h, 94h, 9A0h, 19h>
.data:0000000000008D00                 GPIO_GROUP_INFO <69h, 20h, 0B0h, 100h, 120h, 140h, 160h, 0FFFFh, \
.data:0000000000008D00                                  0FFFFh, 0FFFFh, 0FFFFh, 80h, 84h, 700h, 8>
.data:0000000000008D00                 GPIO_GROUP_INFO <69h, 24h, 0B4h, 0FFFFh, 0FFFFh, 0FFFFh, 0FFFFh, \
.data:0000000000008D00                                  0FFFFh, 0FFFFh, 0FFFFh, 0FFFFh, 88h, 8Ch, 780h, 9>
.data:0000000000008D00                 GPIO_GROUP_INFO <6Ch, 20h, 0B0h, 100h, 120h, 140h, 160h, 0FFFFh, \
.data:0000000000008D00                                  0FFFFh, 0FFFFh, 0FFFFh, 80h, 84h, 700h, 11h>
.data:0000000000008D00                 GPIO_GROUP_INFO <6Dh, 20h, 0B0h, 100h, 120h, 140h, 160h, 0FFFFh, \
.data:0000000000008D00                                  0FFFFh, 0FFFFh, 0FFFFh, 80h, 84h, 700h, 8>
.data:0000000000008D00                 GPIO_GROUP_INFO <6Dh, 24h, 0B4h, 104h, 124h, 144h, 164h, 0FFFFh, \
.data:0000000000008D00                                  0FFFFh, 0FFFFh, 0FFFFh, 88h, 8Ch, 780h, 18h>
.data:0000000000008D00                 GPIO_GROUP_INFO <6Dh, 30h, 0B8h, 108h, 128h, 148h, 168h, 188h, 1A8h, \
.data:0000000000008D00                                  1C8h, 1E8h, 90h, 94h, 900h, 15h>
.data:0000000000008D00                 GPIO_GROUP_INFO <6Dh, 3Ch, 0BCh, 10Ch, 12Ch, 14Ch, 16Ch, 0FFFFh, \
.data:0000000000008D00                                  0FFFFh, 0FFFFh, 0FFFFh, 98h, 9Ch, 0A50h, 18h>
.data:0000000000008D00                 GPIO_GROUP_INFO <6Dh, 48h, 0C0h, 110h, 130h, 150h, 170h, 0FFFFh, \
.data:0000000000008D00                                  0FFFFh, 0FFFFh, 0FFFFh, 0A0h, 0A4h, 0BD0h, 1Dh>
.data:0000000000008D00                 GPIO_GROUP_INFO <6Ah, 20h, 0B0h, 100h, 120h, 140h, 160h, 180h, 1A0h, \
.data:0000000000008D00                                  1C0h, 1E0h, 80h, 84h, 700h, 18h>
.data:0000000000008D00                 GPIO_GROUP_INFO <6Ah, 2Ch, 0B4h, 104h, 124h, 144h, 164h, 0FFFFh, \
.data:0000000000008D00                                  0FFFFh, 0FFFFh, 0FFFFh, 88h, 8Ch, 880h, 19h>
.data:0000000000008D00                 GPIO_GROUP_INFO <6Ah, 3Ch, 0B8h, 0FFFFh, 0FFFFh, 0FFFFh, 0FFFFh, \
.data:0000000000008D00                                  0FFFFh, 0FFFFh, 0FFFFh, 0FFFFh, 90h, 94h, 0A10h, 6>
.data:0000000000008D00                 GPIO_GROUP_INFO <6Ah, 40h, 0BCh, 10Ch, 12Ch, 14Ch, 16Ch, 18Ch, 1ACh, \
.data:0000000000008D00                                  1CCh, 1ECh, 98h, 9Ch, 0A70h, 19h>
.data:0000000000008D00                 GPIO_GROUP_INFO <6Ah, 50h, 0C0h, 0FFFFh, 0FFFFh, 0FFFFh, 0FFFFh, \
.data:0000000000008D00                                  0FFFFh, 0FFFFh, 0FFFFh, 0FFFFh, 0A0h, 0A4h, 0C00h, \
.data:0000000000008D00                                  0Ah>
.data:0000000000008D00                 GPIO_GROUP_INFO <6Bh, 20h, 0B0h, 0FFFFh, 0FFFFh, 0FFFFh, 0FFFFh, \
.data:0000000000008D00                                  0FFFFh, 0FFFFh, 0FFFFh, 0FFFFh, 80h, 84h, 700h, 0Fh>
.data:0000000000008D00                 GPIO_GROUP_INFO <6Bh, 28h, 0B4h, 0FFFFh, 0FFFFh, 0FFFFh, 0FFFFh, \
.data:0000000000008D00                                  0FFFFh, 0FFFFh, 0FFFFh, 0FFFFh, 88h, 8Ch, 7F0h, 5Bh>
*/

GPIO_GROUP_INFO mPchLpGpioGroupInfo[] = {
{ 0x6E, 0x20, 0xB0, 0x100, 0x120, 0x140, 0x160, 0x180, 0x1A0, 0x1C0, 0x1E0, 0x80, 0x84, 0x700, 0x1A },
{ 0x6E, 0x30, 0xB4, 0x104, 0x124, 0x144, 0x164, 0x0FFFF,  0x0FFFF, 0x0FFFF, 0xFFFF, 0x88, 0x8C, 0x8A0, 0x10},
{0x6E, 0x38, 0x0B8, 0x108, 0x128, 0x148, 0x168, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x90, 0x94, 0x9A0, 0x19 },
{0x69, 0x20, 0x0B0, 0x100, 0x120, 0x140, 0x160, 0x0FFFF,0x0FFFF, 0x0FFFF, 0x0FFFF, 0x80, 0x84, 0x700, 0x8 },
{0x69, 0x24, 0x0B4, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x88, 0x8C, 0x780, 0x9 },
{0x6C, 0x20, 0x0B0, 0x100, 0x120, 0x140, 0x160, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x80, 0x84, 0x700, 0x11 },
{0x6D, 0x20, 0x0B0, 0x100, 0x120, 0x140, 0x160, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x80, 0x84, 0x700, 0x8 },
{0x6D, 0x24, 0x0B4, 0x104, 0x124, 0x144, 0x164, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x88, 0x8C, 0x780, 0x18 },
{0x6D, 0x30, 0x0B8, 0x108, 0x128, 0x148, 0x168, 0x188, 0x1A8, 0x1C8, 0x1E8, 0x90, 0x94, 0x900, 0x15 },
{0x6D, 0x3C, 0x0BC, 0x10C, 0x12C, 0x14C, 0x16C, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x98, 0x9C, 0x0A50, 0x18 },
{0x6D, 0x48, 0x0C0, 0x110, 0x130, 0x150, 0x170, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0A0, 0x0A4, 0x0BD0, 0x1D },
{0x6A, 0x20, 0x0B0, 0x100, 0x120, 0x140, 0x160, 0x180, 0x1A0, 0x1C0, 0x1E0, 0x80, 0x84, 0x700, 0x18 },
{0x6A, 0x2C, 0x0B4, 0x104, 0x124, 0x144, 0x164, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x88, 0x8C, 0x880, 0x19 },
{0x6A, 0x3C, 0x0B8, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x90, 0x94, 0x0A10, 0x6 },
{0x6A, 0x40, 0x0BC, 0x10C, 0x12C, 0x14C, 0x16C, 0x18C, 0x1AC, 0x1CC, 0x1EC, 0x98, 0x9C, 0x0A70, 0x19 },
{0x6A, 0x50, 0x0C0, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0A0, 0x0A4, 0x0C00, 0x0A },
{0x6B, 0x20, 0x0B0, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x80, 0x84, 0x700, 0x0F },
{0x6B, 0x28, 0x0B4, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x0FFFF, 0x88, 0x8C, 0x7F0, 0x5B },
};


GPIO_GROUP_INFO mPchHGpioGroupInfo[] = {
     { 0x6E, 0x20, 0xC0, 0x100, 0x120, 0x140, 0x160, 0x180, 0x1A0, 0x1C0, 0x1E0, 0x90, 0x94, 0x600, 0x19 },
    { 0x6E, 0x30, 0x0C4, 0x104, 0x124, 0x144, 0x164, 0x184, 0x1A4, 0x1C4, 0x1E4, 0x98, 0x9C, 0x790, 0x14 },
    { 0x6E, 0x3C, 0x0C8, 0x108, 0x128, 0x148, 0x168, 0x188, 0x1A8, 0x1C8, 0x1E8, 0x0A0, 0x0A4, 0x8D0, 0x18 },
    { 0x6E, 0x4C, 0x0CC, 0x10C, 0x12C, 0x14C, 0x16C, 0x18C, 0x1AC, 0x1CC, 0x1EC, 0x0A8, 0x0AC, 0x0A70, 0x8 },
    { 0x6D, 0x20, 0x0C0, 0x100, 0x120, 0x140, 0x160, 0x180, 0x1A0, 0x1C0, 0x1E0, 0x90, 0x94, 0x600, 0x18 },
    { 0x6D, 0x30, 0x0C4, 0x104, 0x124, 0x144, 0x164, 0x184, 0x1A4, 0x1C4, 0x1E4, 0x98, 0x9C, 0x7A0, 0x18 },
    { 0x6D, 0x3C, 0x0C8, 0x108, 0x128, 0x148, 0x168, 0x188, 0x1A8, 0x1C8, 0x1E8, 0x0A0, 0x0A4, 0x920, 0x8 },
    { 0x6D, 0x40, 0x0CC, 0x10C, 0x12C, 0x14C, 0x16C, 0x18C, 0x1AC, 0x1CC, 0x1EC, 0x0A8, 0x0AC, 0x9A0, 0x10 },
    { 0x6D, 0x4C, 0x0D0, 0x110, 0x130, 0x150, 0x170, 0x190, 0x1B0, 0x1D0, 0x1F0, 0x0B0, 0x0B4, 0x0AB0, 0x1B },
    { 0x6C, 0x20, 0x0C0, 0x100, 0x120, 0x140, 0x160, 0x180, 0x1A0, 0x1C0, 0x1E0, 0x90, 0x94, 0x600, 0x0D },
    { 0x6B, 0x20, 0x0C0, 0x100, 0x120, 0x140, 0x160, 0x180, 0x1A0, 0x1C0, 0x1E0, 0x90, 0x94, 0x600, 0x0D },
    { 0x6B, 0x28, 0x0C4, 0x104, 0x124, 0x144, 0x164, 0x184, 0x1A4, 0x1C4, 0x1E4, 0x98, 0x9C, 0x6D0, 0x18 },
    { 0x6B, 0x34, 0x0C8, 0x108, 0x124, 0x148, 0x168, 0x188, 0x1A8, 0x1C8, 0x1E8, 0x0A0, 0x0A4, 0x850, 0x54 }, // this one
    { 0x6A, 0x20, 0x0C0, 0x100, 0x120, 0x140, 0x160, 0x180, 0x1A0, 0x1C0, 0x1E0, 0x90, 0x94, 0x600, 0x18 },
    { 0x6A, 0x2C, 0x0C4, 0x104, 0x124, 0x144, 0x164, 0x184, 0x1A4, 0x1C4, 0x1E4, 0x98, 0x9C, 0x780, 0x0A },
    { 0x6A, 0x34, 0x0C8, 0x108, 0x128, 0x148, 0x168, 0x188, 0x1A8, 0x1C8, 0x1E8, 0x0A0, 0x0A4, 0x820, 0x0C },
    { 0x69, 0x20, 0x0C0, 0x100, 0x120, 0x140, 0x160, 0x180, 0x1A0, 0x1C0, 0x1E0, 0x90, 0x94, 0x600, 0x0F },
    { 0x69, 0x28, 0x0C4, 0x104, 0x124, 0x144, 0x164, 0x184, 0x1A4, 0x1C4, 0x1E4, 0x98, 0x9C, 0x6F0, 0x9 },
    { 0x69, 0x30, 0x0C8, 0x108, 0x128, 0x148, 0x168, 0x188, 0x1A8, 0x1C8, 0x1E8, 0x0A0, 0x0A4, 0x780, 0x0E },
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

UINT16 GetLpcDID() {
    UINT16 val = read_pci_dword(0, 31, 0, 0) >> 16;
    //printf("LpcDid: %04x\n", val);
    return val;
}



int GetPchSeries() {
    int result;
    UINT16 lpc_did = GetLpcDID();
    if (lpc_did == 0x5180) {
        return 5;
    } else {
        if (((lpc_did - 0x7A00) & 0xFFFFFF7F) != 0) {
            result = -1;
        } else {
            result = 4;
       }
    }    
    return result;
}

bool IsPchLp() {
    int pch_ver = GetPchSeries();
    //printf("pch_ver: %08x\n", pch_ver);
    return pch_ver == 2 || pch_ver == 5 || pch_ver == 6 || pch_ver == 3;
}


const GPIO_GROUP_INFO* GpioGetGroupInfoTable (UINT32 *GpioGroupInfoTableLength)
{
  if (IsPchLp()) {    
    *GpioGroupInfoTableLength = sizeof(mPchLpGpioGroupInfo)/sizeof(mPchLpGpioGroupInfo[0]);
    return mPchLpGpioGroupInfo;
  } else {    
    *GpioGroupInfoTableLength = sizeof(mPchHGpioGroupInfo)/sizeof(mPchHGpioGroupInfo[0]);
    return mPchHGpioGroupInfo;
  }
}

UINT32 GpioGetNumberOfGroups()
{
  UINT32                 GpioGroupInfoLength;
  GpioGetGroupInfoTable (&GpioGroupInfoLength);
  return GpioGroupInfoLength;
}

GPIO_GROUP GpioGetGroupFromGpioPad (GPIO_PAD GpioPad) {
  return GPIO_GET_GROUP_FROM_PAD (GpioPad);
}

UINT32 GpioGetPadNumberFromGpioPad(GPIO_PAD GpioPad) {
  return (UINT32) GPIO_GET_PAD_NUMBER (GpioPad);
}

UINT32 GpioGetGroupIndexFromGroup(GPIO_GROUP GpioGroup) {
  return (UINT32) GPIO_GET_GROUP_INDEX (GpioGroup);
}



UINT32 GpioGetGroupIndexFromGpioPad (GPIO_PAD GpioPad) {
  return (UINT32) GPIO_GET_GROUP_INDEX_FROM_PAD(GpioPad);
}


bool GpioIsCorrectPadForThisChipset(GPIO_PAD GpioPad)
{
    // (GPIO_GET_CHIPSET_ID (GpioPad) == GpioGetThisChipsetId ())
  return GpioGetGroupIndexFromGpioPad(GpioPad) < GpioGetNumberOfGroups();
}


bool GpioIsPadValid (GPIO_PAD GpioPad) {
  const GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;
  UINT32                 PadNumber;

  if (!GpioIsCorrectPadForThisChipset (GpioPad)) {
    printf("GPIO ERROR: Incorrect GpioPad (0x%08x) used on this chipset!\n", GpioPad);
    goto Error;
  }

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  //
  // Check if legal pin number
  //
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);
  if (PadNumber >= GpioGroupInfo[GpioGetGroupIndexFromGpioPad (GpioPad)].PadPerGroup) {
    printf("GPIO ERROR: Pin number (%d) exceeds possible range for this group\n", PadNumber);
    goto Error;
  }

  return true;
Error:  
  return false;
}

bool GpioGetPadOwnership(GPIO_PAD GpioPad, GPIO_PAD_OWN *PadOwnVal) {
  UINT32                 Mask;
  UINT32                 RegOffset;
  UINT32                 GroupIndex;
  UINT32                 PadNumber;
  const GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;
  UINT32                 PadOwnRegValue;

  if (!GpioIsPadValid (GpioPad)) {
    return false;
  }

  GroupIndex = GpioGetGroupIndexFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  //
  // Calculate RegOffset using Pad Ownership offset and GPIO Pad number.
  // One DWord register contains information for 8 pads.
  //
  RegOffset = GpioGroupInfo[GroupIndex].PadOwnOffset + (PadNumber >> 3) * 0x4;

  //
  // Calculate pad bit position within DWord register
  //
  PadNumber %= 8;
  Mask = (BIT1 | BIT0) << (PadNumber * 4);

  PadOwnRegValue = PchSbMmioRead32(GpioGroupInfo[GroupIndex].Community, RegOffset);

  *PadOwnVal = (GPIO_PAD_OWN) ((PadOwnRegValue & Mask) >> (PadNumber * 4));

  return true;
}


bool GpioIsPadHostOwned (GPIO_PAD GpioPad) {
  GPIO_PAD_OWN         PadOwnVal;

  //
  // Check if selected GPIO Pad is not owned by CSME/ISH
  // If GPIO is not owned by Host all access to PadCfg will be dropped
  //
  GpioGetPadOwnership (GpioPad, &PadOwnVal);
  if (PadOwnVal != GpioPadOwnHost) {
    printf("GPIO ERROR: %08x is not owned by host!\n", GpioPad);
    return false;
  }

  return true;
}

/**
  This procedure will read GPIO Pad Configuration register

  @param[in] GpioPad          GPIO pad
  @param[in] DwReg            Choose PADCFG register: 0:DW0, 1:DW1

  @retval PadCfgRegValue      PADCFG_DWx value
**/

UINT32 GpioReadPadCfgReg (GPIO_PAD GpioPad, UINT8 DwReg) {
  UINT32                 PadCfgReg;
  const GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;
  UINT32                 GroupIndex;
  UINT32                 PadNumber;

  GroupIndex = GpioGetGroupIndexFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  //
  // Create Pad Configuration register offset
  //
  PadCfgReg = GpioGroupInfo[GroupIndex].PadCfgOffset + S_GPIO_PCR_PADCFG * PadNumber + 0x4 * DwReg;

  return PchSbMmioRead32(GpioGroupInfo[GroupIndex].Community, PadCfgReg);
}

void GpioWritePadCfgReg (GPIO_PAD GpioPad, UINT8 DwReg, UINT32 val) {
  UINT32                 PadCfgReg;
  const GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;
  UINT32                 GroupIndex;
  UINT32                 PadNumber;

  GroupIndex = GpioGetGroupIndexFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  //
  // Create Pad Configuration register offset
  //
  PadCfgReg = GpioGroupInfo[GroupIndex].PadCfgOffset + S_GPIO_PCR_PADCFG * PadNumber + 0x4 * DwReg;

  PchSbMmioWrite32(GpioGroupInfo[GroupIndex].Community, PadCfgReg, val);
}


// bool GpioIsDswGroup (GPIO_GROUP Group) {
//   if ((Group == GPIO_CNL_LP_GROUP_GPD) || (Group == GPIO_CNL_H_GROUP_GPD)) {
//     return TRUE;
//   } else {
//     return FALSE;
//   }
// }

 UINT32 PadRstCfgTable1[] = {1, 3, 5, 9 };
 UINT32 PadRstCfgTable2[] = {7, 3, 5, 1 };

void GpioConfigFromPadCfgRegValue (GPIO_PAD GpioPad, const UINT32  *PadCfgDwReg, GPIO_CONFIG  *GpioConfig)
{
  int v3; // er9@1
  int v4; // ecx@1
  int *v5; // r10@1
  UINT64 v6; // rax@1
  int v7; // edx@3
  unsigned int v8; // edx@5
  UINT64 result; // rax@7

  UINT32 *GpioData = (UINT32 *) GpioConfig;

  v3 = *PadCfgDwReg;
  v4 = GpioPad & 0xF1F0000;
  v5 = (int *)PadCfgDwReg;
  v6 = (unsigned int)*PadCfgDwReg >> 30;
  if ( v4 == 151322624 || v4 == 134807552 )
    v7 = PadRstCfgTable2[v6];
  else
    v7 = PadRstCfgTable1[v6];
  *((BYTE *)GpioConfig + 3) = v7;
  v8 = v3 & 0x300 | *GpioData & 0xFF10E0FF | ((v3 & 0x1E0000 | ((v3 & 0x6000000 | 0x1080800u) >> 3)) >> 1);
  if ((v3 & (1 << 9)) == 0) {
    v8 |= ((v3 & 0x800000) >> 12) | 0x40000;
  }  
  GpioData[1] = GpioData[1] & 0xFFC03E01;
  result = v5[1] & 0x3C00;
  GpioData[0] = v8 & 0xFFFFBFE1 | ((v3 & 1) << 14) | ((v3 & 0x1C00 | 0x400200u) >> 9);
  GpioData[1] |= ((unsigned int)result | ((v3 & 0x10000000 | 0x8004000u) >> 5)) >> 9;  
}

// void GpioConfigFromPadCfgRegValue (GPIO_PAD GpioPad, const UINT32  *PadCfgDwReg, GPIO_CONFIG  *GpioConfig) 
// {
//   UINT32               PadRstCfg;
//   //
//   // Get Reset Type (PadRstCfg)
//   //
//   PadRstCfg = (PadCfgDwReg[0] & B_GPIO_PCR_RST_CONF) >> N_GPIO_PCR_RST_CONF;

//   GpioConfig->PowerConfig = GpioResetConfigFromPadRstCfg (
//                               GpioPad,
//                               PadRstCfg
//                               );

//   //
//   // Get how interrupt is triggered (RxEvCfg)
//   //
//   GpioConfig->InterruptConfig = ((PadCfgDwReg[0] & B_GPIO_PCR_RX_LVL_EDG) >> (N_GPIO_PCR_RX_LVL_EDG - (N_GPIO_INT_CONFIG_INT_TYPE_BIT_POS + 1))) | (0x1 << N_GPIO_INT_CONFIG_INT_TYPE_BIT_POS);

//   //
//   // Get interrupt generation (GPIRoutIOxAPIC/SCI/SMI/NMI)
//   //
//   GpioConfig->InterruptConfig |= ((PadCfgDwReg[0] & (B_GPIO_PCR_RX_NMI_ROUTE | B_GPIO_PCR_RX_SCI_ROUTE | B_GPIO_PCR_RX_SMI_ROUTE | B_GPIO_PCR_RX_APIC_ROUTE)) >> (N_GPIO_PCR_RX_NMI_ROUTE - (N_GPIO_INT_CONFIG_INT_SOURCE_BIT_POS + 1))) | (0x1 << N_GPIO_INT_CONFIG_INT_SOURCE_BIT_POS);

//   //
//   // Get GPIO direction (GPIORxDis and GPIOTxDis)
//   //
//   GpioConfig->Direction = ((PadCfgDwReg[0] & (B_GPIO_PCR_RXDIS | B_GPIO_PCR_TXDIS)) >> (N_GPIO_PCR_TXDIS - (N_GPIO_DIRECTION_DIR_BIT_POS + 1))) | (0x1 << N_GPIO_DIRECTION_DIR_BIT_POS);

//   //
//   // Get GPIO input inversion (RXINV)
//   // (Only meaningful if input enabled)
//   //
//   if((PadCfgDwReg[0] & B_GPIO_PCR_RXDIS) == 0) {
//     GpioConfig->Direction |= ((PadCfgDwReg[0] & B_GPIO_PCR_RXINV) >> (N_GPIO_PCR_RXINV - (N_GPIO_DIRECTION_INV_BIT_POS + 1))) | (0x1 << N_GPIO_DIRECTION_INV_BIT_POS);
//   }

//   //
//   // Get GPIO output state (GPIOTxState)
//   //
//   GpioConfig->OutputState = ((PadCfgDwReg[0] & B_GPIO_PCR_TX_STATE) << (N_GPIO_PCR_TX_STATE + (N_GPIO_OUTPUT_BIT_POS + 1))) | (0x1 << N_GPIO_OUTPUT_BIT_POS);

//   //
//   // Configure GPIO RX raw override to '1' (RXRAW1)
//   //
//   GpioConfig->OtherSettings = ((PadCfgDwReg[0] & B_GPIO_PCR_RX_RAW1) >> (N_GPIO_PCR_RX_RAW1 - (N_GPIO_OTHER_CONFIG_RXRAW_BIT_POS + 1))) | (0x1 << N_GPIO_OTHER_CONFIG_RXRAW_BIT_POS);

//   //
//   // Get GPIO Pad Mode (PMode)
//   //
//   GpioConfig->PadMode = ((PadCfgDwReg[0] & B_GPIO_PCR_PAD_MODE) >> (N_GPIO_PCR_PAD_MODE - (N_GPIO_PAD_MODE_BIT_POS + 1))) | (0x1 << N_GPIO_PAD_MODE_BIT_POS);

//   //
//   // Get GPIO termination (Term)
//   //
//   GpioConfig->ElectricalConfig = ((PadCfgDwReg[1] & B_GPIO_PCR_TERM) >> (N_GPIO_PCR_TERM - (N_GPIO_ELECTRICAL_CONFIG_TERMINATION_BIT_POS + 1))) | (0x1 << N_GPIO_ELECTRICAL_CONFIG_TERMINATION_BIT_POS);
// }


void GpioReadReg(GPIO_REG  RegType, GPIO_GROUP Group, UINT32 DwNum, UINT32 *ReadVal)
{
  UINT32                 RegOffset;
  UINT32                 GroupIndex;
  const GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;

  RegOffset = NO_REGISTER_FOR_PROPERTY;
  GroupIndex = GpioGetGroupIndexFromGroup (Group);

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  switch (RegType) {
    case GpioHostOwnershipRegister:
      RegOffset = GpioGroupInfo[GroupIndex].HostOwnOffset;
      break;
    case GpioGpeEnableRegister:
      RegOffset = GpioGroupInfo[GroupIndex].GpiGpeEnOffset;
      break;
    case GpioGpeStatusRegister:
      RegOffset = GpioGroupInfo[GroupIndex].GpiGpeStsOffset;
      break;
    case GpioSmiEnableRegister:
      RegOffset = GpioGroupInfo[GroupIndex].SmiEnOffset;
      break;
    case GpioSmiStatusRegister:
      RegOffset = GpioGroupInfo[GroupIndex].SmiStsOffset;
      break;
    case GpioNmiEnableRegister:
      RegOffset = GpioGroupInfo[GroupIndex].NmiEnOffset;
      break;
    case GpioPadConfigLockRegister:
      RegOffset = GpioGroupInfo[GroupIndex].PadCfgLockOffset;
      break;
    case GpioPadLockOutputRegister:
      RegOffset = GpioGroupInfo[GroupIndex].PadCfgLockTxOffset;
      break;
    default:
      break;
  }

  //
  // Check if selected register exists
  //
  if (RegOffset == NO_REGISTER_FOR_PROPERTY) {
    *ReadVal = 0;
    printf("error: NO_REGISTER_FOR_PROPERTY\n");
    return;
  }

  //
  // If there are more then 32 pads per group then certain
  // group information would be split into more then one DWord register.
  //
  if ((RegType == GpioPadConfigLockRegister) || (RegType == GpioPadLockOutputRegister)) {
    //
    // PadConfigLock and OutputLock registers when used for group containing more than 32 pads
    // are not placed in a continuous way, e.g:
    // 0x0 - PadConfigLock_DW0
    // 0x4 - OutputLock_DW0
    // 0x8 - PadConfigLock_DW1
    // 0xC - OutputLock_DW1
    //
    RegOffset += DwNum * 0x8;
  } else {
    RegOffset += DwNum * 0x4;
  }

  *ReadVal = PchSbMmioRead32(GpioGroupInfo[GroupIndex].Community, RegOffset);
}



bool GpioGetPadConfig(GPIO_PAD GpioPad, GPIO_CONFIG  *GpioData) {

  UINT32               PadCfgDwReg[GPIO_PADCFG_DW_REG_NUMBER];
  UINT32               RegVal;
  GPIO_GROUP           Group;
  UINT32               PadNumber;
  UINT32               PadBitPosition;

  Group = GpioGetGroupFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);
  PadBitPosition = GPIO_GET_PAD_POSITION (PadNumber);

  debug_print("GPIO: Group:[%08x] - PadNumber:[%08x] - PadBitPosition:[%08x]\n", Group, PadNumber, PadBitPosition);

  if (!GpioIsPadValid (GpioPad)) {
    return false;
  }

  if (!GpioIsPadHostOwned (GpioPad)) {
    return false;
  }

  //
  // Read PADCFG DW0 register
  //
  PadCfgDwReg[0] = GpioReadPadCfgReg (GpioPad, 0);
  printf("DW0: %08x\n", PadCfgDwReg[0]);

  //
  // Read PADCFG DW1 register
  //
  PadCfgDwReg[1] = GpioReadPadCfgReg (GpioPad, 1);

  //
  // Read PADCFG DW2 register
  //
  PadCfgDwReg[2] = GpioReadPadCfgReg (GpioPad, 2);

  GpioConfigFromPadCfgRegValue (
    GpioPad,
    PadCfgDwReg,
    GpioData
    );

  //
  // Read HOSTSW_OWN registers
  //
  GpioReadReg (
    GpioHostOwnershipRegister,
    Group,
    GPIO_GET_DW_NUM (PadNumber),
    &RegVal
    );

  //
  // Get Host Software Ownership
  //
  GpioData->HostSoftPadOwn = (((RegVal >> PadBitPosition) & 0x1) << (N_GPIO_HOSTSW_OWN_BIT_POS + 1)) | (0x1 << N_GPIO_HOSTSW_OWN_BIT_POS);

  //
  // Read PADCFGLOCK register
  //
  GpioReadReg (
    GpioPadConfigLockRegister,
    Group,
    GPIO_GET_DW_NUM (PadNumber),
    &RegVal
    );

  //
  // Get Pad Configuration Lock state
  //
  GpioData->LockConfig = ((!((RegVal >> PadBitPosition) & 0x1)) << 1) | 0x1;

  //
  // Read PADCFGLOCKTX register
  //
  GpioReadReg (
    GpioPadLockOutputRegister,
    Group,
    GPIO_GET_DW_NUM (PadNumber),
    &RegVal
    );

  //
  // Get Pad Configuration Lock Tx state
  //
  GpioData->LockConfig |= ((!((RegVal >> PadBitPosition) & 0x1)) << 2) | 0x1;

  return true;
}




/**
  This procedure will check state of Pad Config Lock for selected pad

  @param[in] GpioPad              GPIO pad
  @param[out] PadCfgLock          PadCfgLock for selected pad
                                  0: NotLocked, 1: Locked

**/
bool GpioGetPadCfgLock (GPIO_PAD GpioPad, UINT32 *PadCfgLock) {  
  UINT32      PadNumber;
  UINT32      PadCfgLockRegVal;

  if (!GpioIsPadValid (GpioPad)) {    
    return false;
  }

  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  GpioReadReg (
    GpioPadConfigLockRegister,
    GpioGetGroupFromGpioPad (GpioPad),
    GPIO_GET_DW_NUM (PadNumber),
    &PadCfgLockRegVal
  );


  *PadCfgLock = (PadCfgLockRegVal >> GPIO_GET_PAD_POSITION (PadNumber)) & 0x1;

  return true;
}


#define GPIO_GROUP_DEF(Index,ChipsetId)    ((Index) | ((ChipsetId) << 8))


UINT32 GpioGetThisChipsetId () {
  if (IsPchLp()) {
    return 9;
  } else {
    return 8;
  }
}




GPIO_GROUP GpioGetGroupFromGroupIndex(UINT32 GroupIndex) {
  return GPIO_GROUP_DEF (GroupIndex, GpioGetThisChipsetId());
}

GPIO_GROUP GpioGetHighestGroup ()
{
  return GpioGetGroupFromGroupIndex (GpioGetNumberOfGroups () - 1);
}


GPIO_GROUP GpioGetLowestGroup ()
{
  return GpioGetGroupFromGroupIndex(0);
}


bool GpioIsGroupAndDwNumValid(GPIO_GROUP Group, UINT32 DwNum) {
  UINT32                 GroupIndex;
  const GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  GroupIndex = GpioGetGroupIndexFromGroup (Group);

  printf("GroupIndex: %08x\n", GroupIndex);

  if ((Group < GpioGetLowestGroup()) || (Group > GpioGetHighestGroup()) || (GroupIndex >= GpioGroupInfoLength)) {
    printf("GPIO ERROR: Group argument (%d) is not within range of possible groups for this PCH\n", GroupIndex);
    goto Error;
  }

  //
  // Check if DwNum argument does not exceed number of DWord registers
  // resulting from available pads for certain group
  //
  if (DwNum > GPIO_GET_DW_NUM (GpioGroupInfo[GroupIndex].PadPerGroup - 1)){
    goto Error;
  }

  return true;
Error:  
  return false;
}


/**
  This procedure will clear PadCfgLock for selected pads within one group.
  This function should be used only inside SMI.

  @param[in]  Group               GPIO group
  @param[in]  DwNum               PadCfgLock register number for current group.
                                  For group which has less then 32 pads per group DwNum must be 0.
  @param[in]  PadsToUnlock        Bitmask for pads which are going to be unlocked,
                                  Bit position - PadNumber
                                  Bit value - 0: DoNotUnlock, 1: Unlock

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
bool GpioUnlockPadCfgForGroupDw (
  GPIO_GROUP                Group,
  UINT32                    DwNum,
  UINT32                    PadsToUnlock
  )
{
  if (!GpioIsGroupAndDwNumValid (Group, DwNum)) {
    printf("Invalid GpioIsGroupAndDwNumValid");
    return false;
  }


//   return GpioWriteLockReg (
//            GpioPadConfigLockRegister,
//            Group,
//            DwNum,
//            ~PadsToUnlock,
//            0
//            );
}


bool GpioUnlockPadCfg(GPIO_PAD GpioPad)
{
  GPIO_GROUP   Group;
  UINT32       PadNumber;

  Group = GpioGetGroupFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  return GpioUnlockPadCfgForGroupDw (
           Group,
           GPIO_GET_DW_NUM (PadNumber),
           1 << GPIO_GET_PAD_POSITION (PadNumber)
           );
}


bool GpioGetPadCfgLockTx(GPIO_PAD GpioPad, UINT32 *PadCfgLockTx) {
  UINT32      PadNumber;
  UINT32      PadCfgLockTxRegVal;

  if (!GpioIsPadValid (GpioPad)) {
    return false;
  }

  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  GpioReadReg (
    GpioPadLockOutputRegister,
    GpioGetGroupFromGpioPad (GpioPad),
    GPIO_GET_DW_NUM (PadNumber),
    &PadCfgLockTxRegVal
    );

  *PadCfgLockTx = (PadCfgLockTxRegVal >> GPIO_GET_PAD_POSITION (PadNumber)) & 0x1;

  return true;
}


void GpioWritePadCfgReg (
  GPIO_PAD             GpioPad,
  UINT8                DwReg,
  UINT32               PadCfgAndMask,
  UINT32               PadCfgOrMask
 )
{
  UINT32                 PadCfgReg;
  const GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32                 GpioGroupInfoLength;
  UINT32                 GroupIndex;
  UINT32                 PadNumber;
  UINT32                 PadCfgLock;
  UINT32                 PadCfgLockTx;

  PadCfgLock = 0;
  PadCfgLockTx = 0;

  //
  // Check if Pad Configuration (except output state) is to be changed.
  // If AND and OR masks will indicate that configuration fields (other than output control)
  // are to be modified it means that there is a need to perform an unlock (if set)
  //
  if ((~PadCfgAndMask | PadCfgOrMask) & (UINT32)~B_GPIO_PCR_TX_STATE) {
    GpioGetPadCfgLock(GpioPad, &PadCfgLock);
    if (PadCfgLock) {
       printf("Error: We cannot unlock PadCfgLock unless P2SB is not hidden (SecurePciSpace)\n");
      //GpioUnlockPadCfg (GpioPad);
    }
  }

  //
  // Check if Pad Output state is to be changed
  // If AND and OR masks will indicate that output control
  // is to be modified it means that there is a need to perform an unlock (if set)
  //
  if ((~PadCfgAndMask | PadCfgOrMask) & B_GPIO_PCR_TX_STATE) {
    GpioGetPadCfgLockTx (GpioPad, &PadCfgLockTx);
    if (PadCfgLockTx) {
       printf("Error: We cannot unlock PadCfgLockTx unless P2SB is not hidden (SecurePciSpace)\n");
      //GpioUnlockPadCfgTx (GpioPad);
    }
  }

  GroupIndex = GpioGetGroupIndexFromGpioPad (GpioPad);
  PadNumber = GpioGetPadNumberFromGpioPad (GpioPad);

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  //
  // Create Pad Configuration register offset
  //
  PadCfgReg = GpioGroupInfo[GroupIndex].PadCfgOffset + S_GPIO_PCR_PADCFG * PadNumber + 0x4 * DwReg;

  PchSbMmioAndThenOr32(
    GpioGroupInfo[GroupIndex].Community, 
    PadCfgReg,
    PadCfgAndMask,
    PadCfgOrMask
  );
//   if (PadCfgLock) {
//     GpioLockPadCfg (GpioPad);
//   }
//   if (PadCfgLockTx) {
//     GpioLockPadCfgTx (GpioPad);
//   }
}


void GpioWrite(GPIO_PAD GpioPad, UINT8 val) {

    GpioWritePadCfgReg(GpioPad, 0, val & 0x1);
}