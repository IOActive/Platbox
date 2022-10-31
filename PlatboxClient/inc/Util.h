#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "global.h" // switch for types.h?

/* Utils */
void print_memory(unsigned long address, char *buffer, unsigned int bytes_to_print);
void get_user_input(char *input, int size);
char **parse_arguments(char *command_line, char arg_delim);
void hexstring_to_opcodes(char *hexstring, unsigned char **pOpcodes, unsigned int *size);
unsigned int u32_swap(unsigned int x);

#ifdef _WIN32
void * memmem (const void *haystack, size_t haystack_len, const void *needle, size_t needle_len);
#endif

void display_guid(GUID *guid);

void to_guid(char *guid_str, GUID *guid_arg);

