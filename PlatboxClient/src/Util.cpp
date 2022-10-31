// Written by Enrique Nissim (IOActive) 06/2018

#include "Util.h"

char **parse_arguments(char *command_line, char arg_delim) {
	char delim[2] = { 0 };
	if (arg_delim == NULL) {
		delim[0] = ' ';
	}
	else {
		delim[0] = arg_delim;
	}
	char **args = (char **)calloc(1, 0x40);
	char *token;
	char **p = args;
	unsigned int argc = 1;

	p++;
	token = strtok(command_line, delim);
	while (token != NULL) {
		*p = token;
		token = strtok(NULL, delim);
		p++;
		argc++;
	}

	*(unsigned int *)&args[0] = argc;

	return args;
}

void get_user_input(char *input, int size) {
	memset(input, 0x00, size);
	fgets(input, size, stdin);

	// clean the trailing '\n'
	char *pos;
	if ((pos = strchr(input, '\n')) != NULL)
		*pos = '\0';
}

void hexstring_to_opcodes(char *hexstring, unsigned char **ppOpcodes, unsigned int *size) {
	char *pos = hexstring;
	unsigned int len = (strlen(hexstring) / 2) + 1;
	*size = len - 1;
	unsigned char *val = (unsigned char *) malloc(len);
	memset(val, 0x00, len);

	int i = 0;
	for (i = 0; i < *size; i++) {
		sscanf(pos, "%2hhx", &val[i]);
		pos += 2;
	}

	printf("0x");
	for (i = 0; i < *size; i++) {
		printf("%02x", val[i]);
	}
	printf("\n");

	*ppOpcodes = val;
}

// Economou function
void print_memory(unsigned long address, char *buffer, unsigned int bytes_to_print)
{
	unsigned int cont;
	unsigned int i;
	const unsigned short bytes = 16;

	/* Print the lines */
	for (cont = 0; cont < bytes_to_print; cont = cont + bytes)
	{
		printf("%p | ", (void *)address);
		address = address + bytes;

		for (i = 0; i < bytes; i++)
		{
			if (i < (bytes_to_print - cont))
			{
				printf("%.2x ", (unsigned char)buffer[i + cont]);
			}
			else
			{
				printf("   ");
			}
		}

		//Space between two columns
		printf("| ");

		//Print the characters
		for (i = 0; i < bytes; i++)
		{
			if (i < (bytes_to_print - cont))
			{
				printf("%c", (isgraph(buffer[i + cont])) ? buffer[i + cont] : '.');
			}
			else
			{
				printf(" ");
			}
		}
		printf("\n");
	}
}

void error(char *msg) {
	fprintf(stderr, "Error: %s\n", msg);
	exit(-1);
}

unsigned int u32_swap(unsigned int x) {
    int byte0 = x >> 0  & 0xff;
    int byte1 = x >> 8  & 0xff;
    int byte2 = x >> 16 & 0xff;
    int byte3 = x >> 24 & 0xff;
    return (byte0 << 24) | (byte1 << 16) | (byte2 << 8) | byte3;
}


#ifdef _WIN32
/* Return the first occurrence of NEEDLE in HAYSTACK.  */
void *
memmem (const void *haystack, size_t haystack_len, const void *needle,
	size_t needle_len) 
{
  const char *begin;
  const char *const last_possible
    = (const char *) haystack + haystack_len - needle_len;

  if (needle_len == 0)
    /* The first occurrence of the empty string is deemed to occur at
       the beginning of the string.  */
    return (void *) haystack;

  /* Sanity check, otherwise the loop might search through the whole
     memory.  */
  if ( haystack_len < needle_len, 0)
    return NULL;

  for (begin = (const char *) haystack; begin <= last_possible; ++begin)
    if (begin[0] == ((const char *) needle)[0] &&
	!memcmp ((const void *) &begin[1],
		 (const void *) ((const char *) needle + 1),
		 needle_len - 1))
      return (void *) begin;

  return NULL;
}
#endif

void to_guid(char *guid_str, GUID *guid_arg) {

    GUID guid;

	#ifdef __linux__
		sscanf(guid_str, "%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
        &guid.Data1, &guid.Data2, &guid.Data3,
        &guid.Data4[0], &guid.Data4[1], &guid.Data4[2],
        &guid.Data4[3], &guid.Data4[4], &guid.Data4[5],
        &guid.Data4[6], &guid.Data4[7]);
    #else //_WIN32
        sscanf_s(guid_str, "%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
        &guid.Data1, &guid.Data2, &guid.Data3,
        &guid.Data4[0], &guid.Data4[1], &guid.Data4[2],
        &guid.Data4[3], &guid.Data4[4], &guid.Data4[5],
        &guid.Data4[6], &guid.Data4[7]);
    
    #endif	
    

    memcpy(guid_arg, &guid, sizeof(GUID));
}


void display_guid(GUID *guid) {
	printf("{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",
		guid->Data1, guid->Data2, guid->Data3,
		guid->Data4[0], guid->Data4[1], guid->Data4[2], guid->Data4[3],
		guid->Data4[4], guid->Data4[5], guid->Data4[6], guid->Data4[7]);
}
