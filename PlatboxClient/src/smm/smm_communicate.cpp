
#include "smm_communicate.h"
#include "intel_chipset.h"
#include "pci.h"
#include "physmem.h"
#include "msr.h"
#include "global.h"
#include "Util.h"
#include <string.h>

#ifdef _WIN32
	#include "__win.h"
#elif __linux__
	#include "__linux.h"
#endif


UINT64 g_SMMCorePrivateAddr = 0;
UINT64 _get_smm_core_private() {
    if (g_SMMCorePrivateAddr != 0) {
        return g_SMMCorePrivateAddr;
    }

    #ifdef __linux__ 

        UINT64 EfiRtCodeStart = 0;
        UINT64 EfiRtCodeEnd   = 0;

        read_efi_memory_map();

        efi_memory_desc_t *mm_desc  = g_EFI_memmap.mm_desc_array;

        for (int i = 0; i < g_EFI_memmap.efi_memmap.nr_map; i++) {

            if (mm_desc->type == RuntimeCode) {
                EfiRtCodeStart = mm_desc->phys_addr;
                EfiRtCodeEnd   = EfiRtCodeStart + mm_desc->num_pages * PAGE_SIZE;
                break;
            }

            mm_desc++;
        }

        debug_print("EfiRtCodeStart: %016lx\n", EfiRtCodeStart);
        debug_print("EfiRtCodeEnd: %016lx\n", EfiRtCodeEnd);

        void *ptr = search_in_physical_memory(EfiRtCodeStart, EfiRtCodeEnd,
            (BYTE *) SMM_CORE_PRIVATE_DATA_SIGNATURE,  strlen(SMM_CORE_PRIVATE_DATA_SIGNATURE));

        if (!ptr) {
            printf("SMM_CORE_PRIVATE_DATA was not found in the provided range\n");
            return NULL;
        }

        g_SMMCorePrivateAddr = (UINT64) ptr;

    #else // _WIN32

        // This is a lower boundary
        UINT32 FADT_addr = GetFADT_Address();

        // This is an upper boundary
        UINT32 HighestOsPABelow4G = get_highest_physical_range_below_4G();
        debug_print("Lower boundary: %08x\n", FADT_addr);
        debug_print("Upper boundary: %08x\n", HighestOsPABelow4G);
        void *ptr = search_in_physical_memory(FADT_addr, HighestOsPABelow4G,
            (BYTE *) SMM_CORE_PRIVATE_DATA_SIGNATURE,  strlen(SMM_CORE_PRIVATE_DATA_SIGNATURE));

        if (!ptr) {
            printf("SMM_CORE_PRIVATE_DATA was not found in the provided range\n");
            return NULL;
        }

        //printf("RES: %016llx\n", ptr);

        g_SMMCorePrivateAddr = (UINT64) ptr;

    #endif

    return g_SMMCorePrivateAddr;
}


void __smm_dump_s3_bootscript(void *ptr_smm_core_private, const char *output_filename) {

    NTSTATUS status;

    GUID gEfiSmmLockBoxCommunicationGuid;    
    to_guid("2a3cfebd-27e8-4d0a-8b79-d688c2a3e1c0", &gEfiSmmLockBoxCommunicationGuid );

    GUID mBootScriptDataGuid;
    to_guid("AEA6B965-DCF5-4311-B4B8-0F12464494D2", &mBootScriptDataGuid);
    

    const UINT alloc_size = PAGE_SIZE * 20;

    struct alloc_user_physmem user_page = {0};
    BOOL res = alloc_user_mem(alloc_size, &user_page);

    EFI_SMM_COMMUNICATE_HEADER *smm_call;
    UINT buffer_size;
    EFI_SMM_LOCK_BOX_PARAMETER_RESTORE *lockbox_call;
    SMM_CORE_PRIVATE_DATA *pSmmCorePrivateData;
    SW_SMI_CALL smi_call = {0};

    if (!res) {
        printf("Failed to allocate user physical mem\n");
        goto exit;
    }

    memset((void *) user_page.va, 0x00, alloc_size);

    smm_call = (EFI_SMM_COMMUNICATE_HEADER *) user_page.va;

    smm_call->HeaderGuid    = gEfiSmmLockBoxCommunicationGuid;
    smm_call->MessageLength = sizeof(EFI_SMM_LOCK_BOX_PARAMETER_RESTORE);

    buffer_size = sizeof(EFI_SMM_LOCK_BOX_PARAMETER_RESTORE) + sizeof(EFI_SMM_COMMUNICATE_HEADER);
    
    lockbox_call = (EFI_SMM_LOCK_BOX_PARAMETER_RESTORE *) smm_call->Data; 

    lockbox_call->Header.Command      = EFI_SMM_LOCK_BOX_COMMAND_RESTORE;
    lockbox_call->Header.DataLength   = sizeof(EFI_SMM_LOCK_BOX_PARAMETER_RESTORE);
    lockbox_call->Header.ReturnStatus = -1;
    lockbox_call->Buffer              = (EFI_PHYSICAL_ADDRESS) (user_page.pa + buffer_size);
    lockbox_call->Length              = alloc_size - buffer_size;
    lockbox_call->Guid                = mBootScriptDataGuid;
    

    pSmmCorePrivateData = (SMM_CORE_PRIVATE_DATA *) ptr_smm_core_private;    

    pSmmCorePrivateData->CommunicationBuffer = (VOID *) user_page.pa;
    pSmmCorePrivateData->BufferSize          = buffer_size;
    
    //debug_print("Memory before the SMI:\n");
    //print_memory(0, (char *) user_page.va, 0x200);

    //getchar();

    #ifdef __linux__
		status = ioctl(g_hDevice, IOCTL_ISSUE_SW_SMI, &smi_call);			
    #else //_WIN32        
        DWORD bytesReturned = 0;
        status = DeviceIoControl(g_hDevice, IOCTL_ISSUE_SW_SMI, &smi_call, sizeof(SW_SMI_CALL), NULL, 0, &bytesReturned, NULL);

        //printf("SMI Sent!\n");
    
    #endif		

    //debug_print("Memory After the SMI:\n");
    //print_memory(0, (char *) user_page.va, 0x200);

    if (lockbox_call->Header.ReturnStatus == EFI_SUCCESS) {
        FILE *f = fopen(output_filename, "wb");

        fwrite((void *) (user_page.va + buffer_size), alloc_size - buffer_size, 1, f);

        fclose(f);

        printf(" -> Written S3 bootscript into %s\n", output_filename);
    } else {
        printf(" - SMM communicate call failed!\n");
    }


    unmap_physical_memory((void *) user_page.va, user_page.size);

exit:
    return;
}



void smm_dump_s3_bootscript_manual(UINT64 rtcode_start, UINT64 rtcode_end, const char *output_filename) {

    void *ptr = search_in_physical_memory(rtcode_start, rtcode_end,
        (BYTE *) SMM_CORE_PRIVATE_DATA_SIGNATURE,  strlen(SMM_CORE_PRIVATE_DATA_SIGNATURE));

    if (!ptr) {
        printf("SMM_CORE_PRIVATE_DATA was not found in the provided range\n");
        return;
    }

    __smm_dump_s3_bootscript(ptr, output_filename);
     
    unmap_physical_memory((void *) ((UINT64)ptr & 0xFFFFFFFFFFFFF000), PAGE_SIZE);
    
}

void smm_dump_s3_bootscript(const char *output_filename) {
  

    void *ptr = (void *) _get_smm_core_private();
    if (!ptr) {
        printf(" - smm_dump_s3_bootscript: failed to find SMM_CORE_PRIVATE_DATA\n");
        return;
    }
    
    printf("g_SMMCorePrivateAddr: %016llx\n", g_SMMCorePrivateAddr);    

    __smm_dump_s3_bootscript(ptr, output_filename);
}