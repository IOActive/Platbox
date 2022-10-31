/*
**
** Definitions for kernetix Driver
**
*/

#include <linux/types.h>
#include <linux/ioctl.h>

#define KERNETIX_BLOCK_SIZE 0x200
#define KERNETIX_BUFFER_SIZE 0x1000
#define KERNETIX_DEVICE_NAME "KernetixDriver"

#define KERNETIX_MAJOR 0

extern int kernetix_major;

struct _kernel_read 
{
  void *address;
  void *value;
};

struct _kernel_write 
{
  void *address;
  void *value;
  void *old_value;  
};



typedef unsigned int UINT;
typedef UINT UINT32;
typedef char CHAR;
typedef unsigned char UCHAR;
typedef int BOOL;
typedef unsigned char BYTE; 
typedef BYTE* PBYTE;
typedef BYTE UINT8;
typedef BYTE BOOLEAN;

#ifndef PLATBOX_CLIENT
  typedef __u64 UINT64;
#endif

typedef void * PVOID;
typedef unsigned short WORD;
typedef unsigned short USHORT;
typedef unsigned long ULONG;
typedef ULONG* PULONG;
typedef unsigned int DWORD;
typedef unsigned int DWORD32;
typedef UINT64 DWORD64;
typedef int HANDLE;

typedef short WCHAR;

typedef int NTSTATUS;

#define FALSE 0
#define TRUE 1
#define CHAR_BIT 8
#define UCHAR_MAX 255

void _swsmi(short smi_code_data,
   __u64 rax_value, __u64 rbx_value, __u64 rcx_value, __u64 rdx_value, 
    __u64 rsi_value, __u64 rdi_value);
void _read_pci_compatible_configuration_space(UINT8 Bus,
     UINT8 Device, UINT8 Function, PVOID pOut);
void _read_pci_byte( UINT32 CF8, PVOID pOut);
void _read_pci_word( UINT32 CF8, PVOID pOut);
void _read_pci_dword( UINT32 CF8, PVOID pOut);
void _write_pci_byte( UINT32 CF8, UINT32 value);
void _write_pci_word( UINT32 CF8, UINT32 value);
void _write_pci_dword( UINT32 CF8, UINT32 value);
void _read_pci_bar_size( UINT32 CF8, PVOID pOut);
void _rdmsr(int MSR, PVOID pOut);
void _wrmsr(int MSR, UINT64 Value);



typedef struct _SW_SMI_CALL {
	UINT8 SwSmiNumber; // 0xb2
	UINT8 SwSmiData; // 0xb3
	UINT64 rax;
	UINT64 rbx;
	UINT64 rcx;
	UINT64 rdx;
	UINT64 rsi;
	UINT64 rdi;
} SW_SMI_CALL, *PSW_SMI_CALL;



typedef struct _READ_PCI_CONFIGURATION_SPACE_CALL {
	UINT8 bus;
	UINT8 device;
	UINT8 function;

	PVOID buffer;
} READ_PCI_CONFIGURATION_SPACE_CALL, *PREAD_PCI_CONFIGURATION_SPACE_CALL;


typedef struct _READ_PCI_DATA_CALL {
	UINT8 bus;
	UINT8 device;
	UINT8 function;
	UINT8 offset;

  	UINT64 *result;
} READ_PCI_DATA_CALL, *PREAD_PCI_DATA_CALL;


typedef struct _WRITE_PCI_DATA_CALL {
	UINT8 bus;
	UINT8 device;
	UINT8 function;
	UINT8 offset;
	UINT32 value;
} WRITE_PCI_DATA_CALL, *PWRITE_PCI_DATA_CALL;



typedef struct _READ_PHYSICAL_MEM_CALL {
	__u64 address;
} READ_PHYSICAL_MEM_CALL, *PREAD_PHYSICAL_MEM_CALL;

typedef struct _WRITE_PHYSICAL_MEM_CALL {
	__u64 address;
	UINT32 size;
	UCHAR data[0x1000];
} WRITE_PHYSICAL_MEM_CALL, *PWRITE_PHYSICAL_MEM_CALL;


typedef struct _READ_MSR_CALL {
	UINT32 msr;
  UINT64 *result;
} READ_MSR_CALL, *PREAD_MSR_CALL;

typedef struct _WRITE_MSR_CALL {
	UINT32 msr;
	UINT64 value;
} WRITE_MSR_CALL, *PWRITE_MSR_CALL;



/* Use 'n' as magic number */
#define KERNETIX_IOC_MAGIC  'n'

#define KERNETIX_ABW _IOR(KERNETIX_IOC_MAGIC,  0, struct _kernel_write)
#define KERNETIX_ABR _IOR(KERNETIX_IOC_MAGIC,  1, struct _kernel_read)
#define KERNETIX_CR3 _IOR(KERNETIX_IOC_MAGIC,  2, unsigned int)


#define IOCTL_ISSUE_SW_SMI  _IOR(KERNETIX_IOC_MAGIC,  3, void *)
#define IOCTL_EXECUTE_SHELLCODE  _IOWR(KERNETIX_IOC_MAGIC,  4, void *)
#define IOCTL_READ_PCI_HEADER  _IOWR(KERNETIX_IOC_MAGIC,  5, void *)
#define IOCTL_READ_PCI_BYTE  _IOWR(KERNETIX_IOC_MAGIC,  6, void *)
#define IOCTL_READ_PCI_WORD  _IOWR(KERNETIX_IOC_MAGIC,  7, void *)
#define IOCTL_READ_PCI_DWORD _IOWR(KERNETIX_IOC_MAGIC,  8, void *)
#define IOCTL_WRITE_PCI_BYTE  _IOWR(KERNETIX_IOC_MAGIC,  9, void *)
#define IOCTL_WRITE_PCI_WORD _IOWR(KERNETIX_IOC_MAGIC,  10, void *)
#define IOCTL_WRITE_PCI_DWORD  _IOWR(KERNETIX_IOC_MAGIC,  11, void *)
#define IOCTL_GET_PCI_BAR_SIZE _IOWR(KERNETIX_IOC_MAGIC,  12, void *)

//#define IOCTL_READ_PHYSICAL_MEM _IOWR(KERNETIX_IOC_MAGIC,  13, void *)
//#define IOCTL_WRITE_PHYSICAL_MEM _IOWR(KERNETIX_IOC_MAGIC,  14, void *)

#define IOCTL_READ_MSR _IOR(KERNETIX_IOC_MAGIC,  15, void *)
#define IOCTL_WRITE_MSR _IOR(KERNETIX_IOC_MAGIC,  16, void *)
#define IOCTL_PATCH_CALLBACK _IOWR(KERNETIX_IOC_MAGIC,  17, void *)
#define IOCTL_RESTORE_CALLBACK _IOWR(KERNETIX_IOC_MAGIC,  18, void *)
#define IOCTL_REMOVE_ALL_CALLBACKS_HOOKS _IOWR(KERNETIX_IOC_MAGIC,  19, void *)

#define IOCTL_GET_EFI_MEMMAP_ADDRESS _IOWR(KERNETIX_IOC_MAGIC,  20, void *)

