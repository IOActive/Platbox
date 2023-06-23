#include "_io.h"

/* IO port operations */

UINT8 io_inb(UINT16 port) {

    IO_PORT_CALL io_call = {0};
    io_call.port = port;
    io_call.size = IO_SIZE_BYTE;

    #ifdef __linux__ 
        int status = ioctl(g_hDevice, IOCTL_READ_IO_PORT, &io_call);
        debug_print("Status: %08x - IOCTL_READ_IO_PORT\n", status);
    #elif _WIN32
    
        DWORD bytesReturned = 0;
        NTSTATUS status = DeviceIoControl(
            g_hDevice,
            IOCTL_READ_IO_PORT,
            &io_call, sizeof(io_call),
            &io_call, sizeof(io_call),
            &bytesReturned,
            NULL
        );

        debug_print("Status: %08x - Error: %08x\n", status, GetLastError());
    #endif

    return (UINT8) io_call.data;
}



UINT16 io_inw(UINT16 port) {
    IO_PORT_CALL io_call = {0};
    io_call.port = port;
    io_call.size = IO_SIZE_WORD;

    #ifdef __linux__ 
        int status = ioctl(g_hDevice, IOCTL_READ_IO_PORT, &io_call);
        debug_print("Status: %08x - IOCTL_READ_IO_PORT\n", status);
    #elif _WIN32
    
        DWORD bytesReturned = 0;
        NTSTATUS status = DeviceIoControl(
            g_hDevice,
            IOCTL_READ_IO_PORT,
            &io_call, sizeof(io_call),
            &io_call, sizeof(io_call),
            &bytesReturned,
            NULL
        );

        debug_print("Status: %08x  bytes_returned: %08x - Error: %08x\n", status, bytesReturned, GetLastError());
    #endif

    return (UINT16) io_call.data;
}
UINT32 io_ind(UINT16 port) {
    IO_PORT_CALL io_call = {0};
    io_call.port = port;
    io_call.size = IO_SIZE_DWORD;

    #ifdef __linux__ 
        int status = ioctl(g_hDevice, IOCTL_READ_IO_PORT, &io_call);
        debug_print("Status: %08x - IOCTL_READ_IO_PORT\n", status);
    #elif _WIN32
    
        DWORD bytesReturned = 0;
        NTSTATUS status = DeviceIoControl(
            g_hDevice,
            IOCTL_READ_IO_PORT,
            &io_call, sizeof(io_call),
            &io_call, sizeof(io_call),
            &bytesReturned,
            NULL
        );

        debug_print("Status: %08x - Error: %08x\n", status, GetLastError());
    #endif

    return (UINT32) io_call.data;
}

void io_outb(UINT16 port, UINT8 val) {
    IO_PORT_CALL io_call = {0};
    io_call.port = port;
    io_call.data = val;
    io_call.size = IO_SIZE_BYTE;

    #ifdef __linux__ 
        int status = ioctl(g_hDevice, IOCTL_WRITE_IO_PORT, &io_call);
        debug_print("Status: %08x - IOCTL_WRITE_IO_PORT\n", status);
    #elif _WIN32
    
        DWORD bytesReturned = 0;
        NTSTATUS status = DeviceIoControl(
            g_hDevice,
            IOCTL_WRITE_IO_PORT,
            &io_call, sizeof(io_call),
            NULL, 0,
            &bytesReturned,
            NULL
        );

        debug_print("Status: %08x - Error: %08x\n", status, GetLastError());
    #endif
}

void io_outw(UINT16 port, UINT16 val) {
    IO_PORT_CALL io_call = {0};
    io_call.port = port;
    io_call.data = val;
    io_call.size = IO_SIZE_WORD;

    #ifdef __linux__ 
        int status = ioctl(g_hDevice, IOCTL_WRITE_IO_PORT, &io_call);
        debug_print("Status: %08x - IOCTL_WRITE_IO_PORT\n", status);
    #elif _WIN32
    
        DWORD bytesReturned = 0;
        NTSTATUS status = DeviceIoControl(
            g_hDevice,
            IOCTL_WRITE_IO_PORT,
            &io_call, sizeof(io_call),
            NULL, 0,
            &bytesReturned,
            NULL
        );

        debug_print("Status: %08x - Error: %08x\n", status, GetLastError());
    #endif
}

void io_outd(UINT16 port, UINT32 val) {
    IO_PORT_CALL io_call = {0};
    io_call.port = port;
    io_call.data = val;
    io_call.size = IO_SIZE_DWORD;

    #ifdef __linux__ 
        int status = ioctl(g_hDevice, IOCTL_WRITE_IO_PORT, &io_call);
        debug_print("Status: %08x - IOCTL_WRITE_IO_PORT\n", status);
    #elif _WIN32
    
        DWORD bytesReturned = 0;
        NTSTATUS status = DeviceIoControl(
            g_hDevice,
            IOCTL_WRITE_IO_PORT,
            &io_call, sizeof(io_call),
            NULL, 0,
            &bytesReturned,
            NULL
        );

        debug_print("Status: %08x - Error: %08x\n", status, GetLastError());
    #endif
}