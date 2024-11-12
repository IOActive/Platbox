#pragma once
#include "types.h"
#include "intel_chipset.h"
#include "amd_chipset.h"

#ifndef signature_INTEL_ebx 
    #define signature_INTEL_ebx 0x756e6547
#endif
#ifndef signature_INTEL_edx
    #define signature_INTEL_edx 0x49656e69
#endif
#ifndef signature_INTEL_ecx
    #define signature_INTEL_ecx 0x6c65746e
#endif
#ifndef signature_AMD_ebx
    #define signature_AMD_ebx 0x68747541
#endif
#ifndef signature_AMD_edx
    #define signature_AMD_edx 0x69746e65
#endif
#ifndef signature_AMD_ecx
    #define signature_AMD_ecx 0x444d4163
#endif

enum _PROCESSOR_TYPE {
    _INTEL_PROCESSOR = 0,
    _AMD_PROCESSOR   = 1,
    _UNKNOWN_UNSUPPORTED_ARCH = 2
};

int get_processor_type();

void get_chipset_information();
void dump_spi_flash(const char *output_filename);

void get_tseg_region(UINT64 *base, UINT32 *size);

UINT16 get_smi_trigger_port();
int get_number_of_cores();

int is_5_level_paging();