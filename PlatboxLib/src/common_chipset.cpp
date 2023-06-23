#include "common_chipset.h"

int get_processor_type() {
    
    #ifdef __linux__
        UINT32 eax = 0, ebx = 0, ecx = 0, edx = 0;
        
        __get_cpuid(0, &eax, &ebx, &ecx, &edx);        
        if ( ebx == signature_INTEL_ebx && edx == signature_INTEL_edx && ecx == signature_INTEL_ecx) {
            return _INTEL_PROCESSOR;
        } else if (ebx == signature_AMD_ebx && edx == signature_AMD_edx && ecx == signature_AMD_ecx) {
            return _AMD_PROCESSOR;
        } else {
            printf("Unknown architecture?\n");
            return _UNKNOWN_UNSUPPORTED_ARCH;
        }
        

    #else // _WIN32
        int regs[4] = {0};

        __cpuidex(&regs[0], 0, 0);
        if ( regs[1] == signature_INTEL_ebx && regs[3] == signature_INTEL_edx && regs[2] == signature_INTEL_ecx) {
            return _INTEL_PROCESSOR;
        } else if (regs[1] == signature_AMD_ebx && regs[3] == signature_AMD_edx && regs[2] == signature_AMD_ecx) {
            return _AMD_PROCESSOR;
        } else {
            printf("Unknown architecture?\n");
            return _UNKNOWN_UNSUPPORTED_ARCH;
        }

    #endif
}


void get_processor_model(BYTE *family, BYTE *model) {

#ifdef __linux__
    UINT32 eax = 0, ebx = 0, ecx = 0, edx = 0;

    __get_cpuid(1, &eax, &ebx, &ecx, &edx);
    *family = ((eax >> 20) & 0x000000FF) + ((eax >> 8) & 0x0000000F);
    *model = (((eax >> 16) & 0x0000000F) << 4) + ((eax >> 4) & 0x0000000F);


#else // _WIN32
    int regs[4] = { 0 };

    __cpuidex(&regs[0], 1, 0);
    *family = ((regs[0] >> 20) & 0x000000FF) + ((regs[0] >> 8) & 0x0000000F);
    *model = (((regs[0] >> 16) & 0x0000000F) << 4) + ((regs[0] >> 4) & 0x0000000F);


#endif
}


void dump_spi_flash(const char *output_filename) {
    switch(get_processor_type()) {
        case _INTEL_PROCESSOR:
            intel_dump_spi_flash(output_filename);
            break;
        case _AMD_PROCESSOR:
            amd_dump_spi_flash(output_filename);
            break;
    }
}

void get_chipset_information() {
    switch(get_processor_type()) {
        case _INTEL_PROCESSOR:
            intel_get_chipset_information();
            break;
        case _AMD_PROCESSOR:

            BYTE family, model = 0;
            get_processor_model(&family, &model);
            printf("Detected chipset:\n");
            printf("=> Family: %02x\n", family);
            printf("=> Model: %02x\n", model);
            printf("\n");
            
            amd_print_chipset_information();
            break;
    }
}

void get_tseg_region(UINT64 *base, UINT32 *size) {
    switch(get_processor_type()) {
        case _INTEL_PROCESSOR:
            get_smrr(base, size);
            break;
        case _AMD_PROCESSOR:
            get_tseg_info(base, size);
            break;
    }
}

UINT16 get_smi_trigger_port() {
    switch(get_processor_type()) {
        case _INTEL_PROCESSOR:
            return INTEL_SMI_TRIGGER_PORT;
            break;
        case _AMD_PROCESSOR:
            return get_amd_smi_trigger_port();
            break;
    }

    return 0;
}