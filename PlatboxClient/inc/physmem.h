#pragma once
#include "global.h"

#define PAGE_SIZE 0x1000
#define PAGE_PRESENT 63
#define PAGE_SWAPPED 62

typedef enum {
	Reserved = 0,
	LoaderCode,
	LoaderData,
	BootCode,
	BootData,
	RuntimeCode,
	RuntimeData,
	Conventional,
	Unusable,
	ACPIReclaim,
	ACPIMemNVS,
	MMIO,
	MMIOPort,
	PALCode,
	Persistent
} EFI_MEMORY_TYPE;

typedef struct {
	/* 0x00 */ UINT32 type;
	/* 0x04 */ UINT32 pad;
	/* 0x08 */ UINT64 phys_addr;
	/* 0x10 */ UINT64 virt_addr;
	/* 0x18 */ UINT64 num_pages;
	/* 0x20 */ UINT64 attribute;
} efi_memory_desc_t;

struct efi_memory_map {
	UINT64 phys_map;
	/*void */ UINT64 map;
	/*void */ UINT64 map_end;
	int nr_map;
	unsigned long desc_version;
	unsigned long desc_size;
#define EFI_MEMMAP_LATE (1UL << 0)
#define EFI_MEMMAP_MEMBLOCK (1UL << 1)
#define EFI_MEMMAP_SLAB (1UL << 2)
	unsigned long flags;
};

struct wrap_efi_memmap {
    UINT64 kernel_vaddr; // retrieved from OS somehow
    UINT64 user_vaddr;   // Mapped - not used currently
    struct efi_memory_map efi_memmap;
    efi_memory_desc_t *mm_desc_array; // memory descriptors array
};

extern struct wrap_efi_memmap g_EFI_memmap;


/* Iterate through an efi_memory_map */
#define for_each_efi_memory_desc_in_map(m, md)				   \
	for ((md) = (m)->map;						   \
	     (md) && ((void *)(md) + (m)->desc_size) <= (m)->map_end;	   \
	     (md) = (void *)(md) + (m)->desc_size)


#define for_each_efi_memory_desc(md) \
	for_each_efi_memory_desc_in_map(&efi.memmap, md)


/* physmem */

void * map_physical_memory(UINT64 address, UINT32 size);
void unmap_physical_memory(void *ptr, UINT32 size);
BOOL alloc_user_mem(UINT32 size, struct alloc_user_physmem *alloc_mem);

void read_physical_memory(UINT64 address, UINT32 size, PVOID buffer, BOOL bPrint);
void write_byte_physical_memory(UINT64 address, UINT32 value);
void read_physical_memory_into_file(UINT64 address, UINT32 size, const char *filename);

void * search_in_physical_memory(UINT64 start, UINT64 end, BYTE *needle, UINT needle_size);

void do_kernel_va_read(UINT64 va, UINT32 size, char *buff);

void read_efi_memory_map();
void print_efi_memory_map();

#ifdef _WIN32
UINT32 get_highest_physical_range_below_4G();
#endif

void _map_pci_ecam();



void parse_handle_physmem_operation(int argc, char **argv);
void parse_handle_map2user_operation(int argc, char **argv);
void parse_handle_alloc_operation(int argc, char **argv);
void parse_handle_free_memory_operation(int argc, char **argv);
void parse_handle_mem_dump_operation(int argc, char **argv);