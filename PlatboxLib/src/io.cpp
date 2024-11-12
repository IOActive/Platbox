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



UINT8 io_index_byte_read_byte(
    UINT16 cmd_port,
    UINT16 data_port,
    UINT8  idx
) {

    UINT8 res = 0;
    #ifdef __linux__ 
        printf("io_index_rb not implemented!\n");
        exit(-1);
    #elif _WIN32
        IO_PORT_INDEX_CALL req;

        req.operation = INDEX_IO_BYTE_READ_BYTE;
        req.cmd_port  = cmd_port;
        req.data_port = data_port;
        req.index     = idx;

        DWORD bytesReturned = 0;
        NTSTATUS status = DeviceIoControl(
            g_hDevice,
            IOCTL_INDEX_IO_OPERATION,
            &req, sizeof(req),
            &req, sizeof(req),
            &bytesReturned,
            NULL
        );

        if (NT_SUCCESS(status)) {
            res = req.data;
        } else {
            debug_print("Status: %08x - Error: %08x\n", status, GetLastError());
        }        
    #endif

    return res;
}

UINT8 io_index_word_read_byte(
    UINT16 cmd_port,
    UINT16 data_port,
    UINT16  idx
) {

    UINT8 res = 0;
    #ifdef __linux__ 
        printf("io_index_rb not implemented!\n");
        exit(-1);
    #elif _WIN32
        IO_PORT_INDEX_CALL req;

        req.operation = INDEX_IO_WORD_READ_BYTE;
        req.cmd_port  = cmd_port;
        req.data_port = data_port;
        req.index     = idx;

        DWORD bytesReturned = 0;
        NTSTATUS status = DeviceIoControl(
            g_hDevice,
            IOCTL_INDEX_IO_OPERATION,
            &req, sizeof(req),
            &req, sizeof(req),
            &bytesReturned,
            NULL
        );

        if (NT_SUCCESS(status)) {
            res = req.data;
        } else {
            debug_print("Status: %08x - Error: %08x\n", status, GetLastError());
        }        
    #endif

    return res;
}

UINT16 io_index_byte_read_word(
    UINT16 cmd_port,
    UINT16 data_port,
    UINT8  idx
) {

    UINT16 res = 0;
    #ifdef __linux__ 
        printf("io_index_rb not implemented!\n");
        exit(-1);
    #elif _WIN32
        IO_PORT_INDEX_CALL req;

        req.operation = INDEX_IO_BYTE_READ_WORD;
        req.cmd_port  = cmd_port;
        req.data_port = data_port;
        req.index     = idx;

        DWORD bytesReturned = 0;
        NTSTATUS status = DeviceIoControl(
            g_hDevice,
            IOCTL_INDEX_IO_OPERATION,
            &req, sizeof(req),
            &req, sizeof(req),
            &bytesReturned,
            NULL
        );

        if (NT_SUCCESS(status)) {
            res = req.data;
        } else {
            debug_print("Status: %08x - Error: %08x\n", status, GetLastError());
        }        
    #endif

    return res;
}

UINT16 io_index_word_read_word(
    UINT16 cmd_port,
    UINT16 data_port,
    UINT16  idx
) {
    UINT16 res = 0;

    #ifdef __linux__ 
        printf("io_index_rb not implemented!\n");
        exit(-1);
    #elif _WIN32
        IO_PORT_INDEX_CALL req;

        req.operation = INDEX_IO_WORD_READ_WORD;
        req.cmd_port  = cmd_port;
        req.data_port = data_port;
        req.index     = idx;

        DWORD bytesReturned = 0;
        NTSTATUS status = DeviceIoControl(
            g_hDevice,
            IOCTL_INDEX_IO_OPERATION,
            &req, sizeof(req),
            &req, sizeof(req),
            &bytesReturned,
            NULL
        );

        if (NT_SUCCESS(status)) {
            res = req.data;
        } else {
            debug_print("Status: %08x - Error: %08x\n", status, GetLastError());
        }        
    #endif

    return res;
}



void io_index_byte_write_byte(
    UINT16 cmd_port,
    UINT16 data_port,
    UINT8  idx,
    UINT8  data
) {
    #ifdef __linux__ 
        printf("io_index_rb not implemented!\n");
        exit(-1);
    #elif _WIN32
        IO_PORT_INDEX_CALL req;

        req.operation = INDEX_IO_BYTE_WRITE_BYTE;
        req.cmd_port  = cmd_port;
        req.data_port = data_port;
        req.index     = idx;
        req.data      = data;

        DWORD bytesReturned = 0;
        NTSTATUS status = DeviceIoControl(
            g_hDevice,
            IOCTL_INDEX_IO_OPERATION,
            &req, sizeof(req),
            &req, sizeof(req),
            &bytesReturned,
            NULL
        );
    #endif
}

void io_index_word_write_byte(
    UINT16 cmd_port,
    UINT16 data_port,
    UINT16 idx,
    UINT8  data
) {
    #ifdef __linux__ 
        printf("io_index_rb not implemented!\n");
        exit(-1);
    #elif _WIN32
        IO_PORT_INDEX_CALL req;

        req.operation = INDEX_IO_WORD_WRITE_BYTE;
        req.cmd_port  = cmd_port;
        req.data_port = data_port;
        req.index     = idx;
        req.data      = data;

        DWORD bytesReturned = 0;
        NTSTATUS status = DeviceIoControl(
            g_hDevice,
            IOCTL_INDEX_IO_OPERATION,
            &req, sizeof(req),
            &req, sizeof(req),
            &bytesReturned,
            NULL
        );
    #endif
}

void io_index_byte_write_word(
    UINT16 cmd_port,
    UINT16 data_port,
    UINT8  idx,
    UINT16 data
) {
    #ifdef __linux__ 
        printf("io_index_rb not implemented!\n");
        exit(-1);
    #elif _WIN32
        IO_PORT_INDEX_CALL req;

        req.operation = INDEX_IO_BYTE_WRITE_WORD;
        req.cmd_port  = cmd_port;
        req.data_port = data_port;
        req.index     = idx;
        req.data      = data;

        DWORD bytesReturned = 0;
        NTSTATUS status = DeviceIoControl(
            g_hDevice,
            IOCTL_INDEX_IO_OPERATION,
            &req, sizeof(req),
            &req, sizeof(req),
            &bytesReturned,
            NULL
        );
    #endif
}

void io_index_word_write_word(
    UINT16 cmd_port,
    UINT16 data_port,
    UINT16 idx,
    UINT16 data
) {
    #ifdef __linux__ 
        printf("io_index_rb not implemented!\n");
        exit(-1);
    #elif _WIN32
        IO_PORT_INDEX_CALL req;

        req.operation = INDEX_IO_WORD_WRITE_WORD;
        req.cmd_port  = cmd_port;
        req.data_port = data_port;
        req.index     = idx;
        req.data      = data;

        DWORD bytesReturned = 0;
        NTSTATUS status = DeviceIoControl(
            g_hDevice,
            IOCTL_INDEX_IO_OPERATION,
            &req, sizeof(req),
            &req, sizeof(req),
            &bytesReturned,
            NULL
        );
    #endif
}