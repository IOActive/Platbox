#pragma once
#include "global.h"

#define PAGE_BITS	12
#define PAGE_SIZE 	0x1000
#define PAGE_MASK 	0xFFFFFFFFFFFFF000
#define PAGE_OFFSET 0xFFF

#define PAGE_PRESENT 63
#define PAGE_SWAPPED 62

#define PFN(x) (x >> PAGE_BITS)
#define PFN_TO_PAGE(x) (x << PAGE_BITS)

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
	UINT64 type;
	UINT64 pad;
	UINT64 phys_addr;
	UINT64 virt_addr;
	UINT64 num_pages;
	UINT64 attribute;
} efi_memory_desc_t;


struct efi_memory_map {
	UINT64 phys_map;
	/*void */ UINT64 map;
	/*void */ UINT64 map_end;
	unsigned long nr_map;
	unsigned long desc_version;
	unsigned long desc_size;
#define EFI_MEMMAP_LATE (1UL << 0)
#define EFI_MEMMAP_MEMBLOCK (1UL << 1)
#define EFI_MEMMAP_SLAB (1UL << 2)
	unsigned long flags;
};

static void print_struct_efi_memory_map(struct efi_memory_map *s) {
	printf("phys_map: %016llx\n", s->phys_map);
	printf("map: %016llx\n", s->map);
	printf("map_end: %016llx\n", s->map_end);
	printf("nr_map: %016llx\n", s->nr_map);
	printf("desc_version: %016llx\n", s->desc_version);
	printf("desc_size: %016llx\n", s->desc_size);
	printf("flags: %016llx\n", s->flags);
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
void unmap_physical_memory_unaligned(void *ptr, UINT32 size);
BOOL alloc_user_mem(UINT32 size, struct alloc_user_physmem *alloc_mem);

void read_physical_memory(UINT64 address, UINT32 size, PVOID buffer, BOOL bPrint);
void write_byte_physical_memory(UINT64 address, UINT32 value);
void read_physical_memory_into_file(UINT64 address, UINT32 size, const char *filename);

void * search_in_physical_memory(UINT64 start, UINT64 end, BYTE *needle, UINT needle_size);

int do_kernel_va_read(UINT64 va, UINT32 size, char *buff);

int read_efi_memory_map();
void print_efi_memory_map();

#ifdef _WIN32
UINT32 get_highest_physical_range_below_4G();
#endif

void _map_pci_ecam();



UINT64 virt_to_phys(UINT64 va);
