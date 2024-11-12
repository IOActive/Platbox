#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "global.h" // switch for types.h?

#define __ALIGN_MASK(x, mask)  (((x) + (mask)) & ~(mask))
#define ALIGN(x, a)  __ALIGN_MASK((x), ((a) - 1))


#ifdef _WIN32
	#include "__win.h"
#endif

#ifndef BIT0
#define BIT0                     0x0001
#define BIT1                     0x0002
#define BIT2                     0x0004
#define BIT3                     0x0008
#define BIT4                     0x0010
#define BIT5                     0x0020
#define BIT6                     0x0040
#define BIT7                     0x0080
#define BIT8                     0x0100
#define BIT9                     0x0200
#define BIT10                    0x0400
#define BIT11                    0x0800
#define BIT12                    0x1000
#define BIT13                    0x2000
#define BIT14                    0x4000
#define BIT15                    0x8000
#define BIT16                    0x00010000
#define BIT17                    0x00020000
#define BIT18                    0x00040000
#define BIT19                    0x00080000
#define BIT20                    0x00100000
#define BIT21                    0x00200000
#define BIT22                    0x00400000
#define BIT23                    0x00800000
#define BIT24                    0x01000000
#define BIT25                    0x02000000
#define BIT26                    0x04000000
#define BIT27                    0x08000000
#define BIT28                    0x10000000
#define BIT29                    0x20000000
#define BIT30                    0x40000000
#define BIT31                    0x80000000
#endif


/* Utils */
void memcpy4(void *dst, void *src, size_t size);
void print_memory(unsigned long address, char *buffer, unsigned int bytes_to_print);
void get_user_input(char *input, int size);
char **parse_arguments(char *command_line, char arg_delim);
void hexstring_to_opcodes(char *hexstring, unsigned char **pOpcodes, unsigned int *size);
unsigned int u32_swap(unsigned int x);

#ifdef _WIN32
void * memmem (const void *haystack, size_t haystack_len, const void *needle, size_t needle_len);
#endif

void display_guid(GUID *guid);

void to_guid(const char *guid_str, GUID *guid_arg);

