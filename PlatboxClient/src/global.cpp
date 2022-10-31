#include "global.h"

HANDLE g_hDevice = 0;

int DEBUG_MODE = 0;

void debug_print(const char *fmt, ...) {
    if (DEBUG_MODE) {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
    }
}

int _prev_debug_mode = 0;
void enable_debug_mode() {
    _prev_debug_mode = DEBUG_MODE;
    DEBUG_MODE = 1;
}

void restore_debug_mode() {
    DEBUG_MODE = _prev_debug_mode;
}

char _aux_print_buff[2048];

void print_green(const char *fmt, ...) {
    #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);

        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    #else

        memset(_aux_print_buff, 0x00, sizeof(_aux_print_buff));
        snprintf(_aux_print_buff, sizeof(_aux_print_buff), "\x1b[32m%s\x1b[0m", fmt);        

        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, _aux_print_buff, ap);
        va_end(ap);

    #endif
}

void print_red(const char *fmt, ...) {
    #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);

        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    #else

        memset(_aux_print_buff, 0x00, sizeof(_aux_print_buff));
        snprintf(_aux_print_buff, sizeof(_aux_print_buff), "\x1b[31m%s\x1b[0m", fmt);

        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, _aux_print_buff, ap);
        va_end(ap);

    #endif
}


#pragma comment(lib, "DeltaFuzzLib.lib")

FUZZER_OBJECT *g_fuzzer;


PCI_ECAM g_pci_ecam;