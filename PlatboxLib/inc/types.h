#pragma once
#include <stdint.h>
#include <stdlib.h>
#ifdef __linux__ 
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
        #define PLATBOX_CLIENT
    #endif
    
    typedef uint64_t UINT64;
    typedef void * PVOID;
    typedef unsigned short WORD;
    typedef unsigned short USHORT;
    typedef USHORT UINT16;
    typedef unsigned long ULONG;
    typedef ULONG* PULONG;
    typedef unsigned int DWORD;
    typedef unsigned int DWORD32;
    typedef UINT64 DWORD64;
    typedef int HANDLE;
    
    typedef void VOID;

    typedef short WCHAR;

    typedef int NTSTATUS;
    
    #define FALSE 0
    #define TRUE 1
    #define CHAR_BIT 8
    #define UCHAR_MAX 255
    
    

    #include <string.h>
    #include <fcntl.h>		
    #include <unistd.h>		
    #include <sys/ioctl.h>
    #include <cpuid.h>
    #include <sys/mman.h>
    #include <sys/stat.h>

    #pragma pack(push, 1)
    typedef struct _GUID {
        UINT32  Data1;
        UINT16  Data2;
        UINT16  Data3;
        UINT8   Data4[8];
    } GUID;
    #pragma pack(pop)
    
#elif _WIN32
    #include <windows.h>
    #include <ntstatus.h>

#else

#endif

#ifndef NT_SUCCESS
	#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#endif