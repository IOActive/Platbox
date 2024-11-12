#include "PlatboxClient.h"

#pragma comment(lib, "platbox_lib.lib")


// extern "C" {
// 	void _run_attempt();
// }

void parse_handle_physmem_operation(int argc, char **argv) {
	// physmem r 0x000000 0x100--> reads 0x100 bytes from physical address 0
	if (!strcmp(argv[1], "r")) {
		if (argc == 4) {
			UINT64 address = strtoull(argv[2], NULL, 16);
			UINT32 size = strtoul(argv[3], NULL, 16);
			read_physical_memory(address, size, NULL, TRUE);
		}
		if (argc == 5) {
			// physmem r 0xFFAA0000 0x100 weird_region.bin
			UINT64 address = strtoull(argv[2], NULL, 16);
			UINT32 size = strtoul(argv[3], NULL, 16);
			read_physical_memory_into_file(address, size, argv[4]);
		}
	}
	if (!strcmp(argv[1], "wb")) {
		if (argc == 4) {
			UINT64 address = strtoull(argv[2], NULL, 16);
			UINT32 value = strtoul(argv[3], NULL, 16);
			write_byte_physical_memory(address, value);
		}
	}
	if (!strcmp(argv[1], "efimap")) {
		print_efi_memory_map();
	}
}


void parse_handle_map2user_operation(int argc, char **argv) {
	// map2user 0x0000000 --> maps the physical page 0 to usermode
	// argv[0] = map2user
	// argv[1] = 0x000000
	if (argc == 2) {
		UINT64 address = strtoull(argv[1], NULL, 16);
		DEBUG_MODE = 1;
		void *mapped_addr = map_physical_memory(address, PAGE_SIZE);
		DEBUG_MODE = 0;
	}
}

void parse_handle_alloc_operation(int argc, char **argv) {
	// alloc 0x0000000 --> allocs size
	// argv[0] = alloc
	// argv[1] = size
	if (argc == 2) {
		UINT32 size = strtoul(argv[1], NULL, 16);
		DEBUG_MODE = 1;
		alloc_user_mem(size, NULL);
		DEBUG_MODE = 0;
	}
}

void parse_handle_free_memory_operation(int argc, char **argv) {
	// alloc 0x0000000 --> allocs size
	// argv[0] = alloc
	// argv[1] = size
	if (argc == 2) {
		UINT64 address = strtoull(argv[1], NULL, 16);
		DEBUG_MODE = 1;
		unmap_physical_memory((void *) address, 0);
		DEBUG_MODE = 0;
	}
}


void parse_handle_mem_dump_operation(int argc, char **argv) {
	// dd 0x0000000 0x100 -> dumps the first 0x100 bytes of the vA 0x000
	// argv[0] = dd
	// argv[1] = addr
	// argv[2] = size
	if (argc == 3) {
		UINT64 address = strtoull(argv[1], NULL, 16);
		UINT64 size    = strtoul(argv[2], NULL, 16);
		print_memory(address, (char *) address, size);
	
	}
}


void parse_bus_device_function(char *line, UINT *bus, UINT *device, UINT *function) {
	char **subargs = parse_arguments(line, ':');
	int subargc = (int)(size_t)subargs[0];
	subargc--;
	if (subargc == 3) {
		*bus = strtoul(subargs[1], NULL, 10);
		*device = strtoul(subargs[2], NULL, 10);
		*function = strtoul(subargs[3], NULL, 10);
	}
	free(subargs);
}


void read_msr(int argc, char **argv) {
	/* rdmsr <msr> */
	if (argc == 2) {		
		UINT msr = strtoul(argv[1], NULL, 16);
		enable_debug_mode();
		do_read_msr(msr, NULL);
		restore_debug_mode();
	} else if (argc == 3 && !strcmp(argv[1], "all")) {
		UINT msr = strtoul(argv[2], NULL, 16);
		UINT64 val;
		for (int i = 0; i < get_number_of_cores(); i++) {
			do_read_msr_for_core(i, msr, &val);
			printf("-> Core-%d MSR-[%08x]: %016llx\n", i, msr, val);
		}
	}
}

void write_msr(int argc, char **argv) {
	/* wrmsr <msr> <value> */
	if (argc == 3) {	
		UINT msr = strtoul(argv[1], NULL, 16);
		UINT64 value = strtoull(argv[2], NULL, 16);
		enable_debug_mode();
		do_write_msr(msr, value);
		restore_debug_mode();
	}
}

void parse_handle_pci_operation(int argc, char **argv) {
	// pci r 0:2:0 -> reads the configuration space
	// pci rb 0:2:0 1 -> reads a byte from offset 
	// pci rw 0:2:0 1 -> reads a word from offset 1*4
	// pci rd 0:2:0 1 -> reads a dword from offset 1*4
	// pci wb 0:2:0 1 0x90 -> writes a byte to offset 1*4
	// pci ww 0:2:0 1 0x9090 -> writes a word to offset 1*4
	// pci wd 0:2:0 1 0x90909090 -> writes a dword to offset 1*4
	// pci r bar 0:2:0 0x10 -> read BAR info at offset 0x10
	UINT bus, device, function = 0;
	UINT offset = 0;
	UINT value = 0;
	if (!strcmp(argv[1], "r")) {
		if (argc == 3) {
			parse_bus_device_function(argv[2], &bus, &device, &function);
			read_pci_header(bus, device, function);
		}
		else if (argc == 5 && !strcmp(argv[2], "bar")) {
			parse_bus_device_function(argv[3], &bus, &device, &function);
			offset = strtoul(argv[4], NULL, 16);
			read_pci_bar_info(bus, device, function, offset >> 2);
		}
	}
	else if (!strcmp(argv[1], "rb")) {
		if (argc == 4) {
			parse_bus_device_function(argv[2], &bus, &device, &function);
			offset = strtoul(argv[3], NULL, 16);
			enable_debug_mode();
			read_pci_byte(bus, device, function, offset);
			restore_debug_mode();
		}
	}
	else if (!strcmp(argv[1], "rw")) {
		if (argc == 4) {
			parse_bus_device_function(argv[2], &bus, &device, &function);
			offset = strtoul(argv[3], NULL, 16);
			enable_debug_mode();
			read_pci_word(bus, device, function, offset);
			restore_debug_mode();
		}
	}
	else if (!strcmp(argv[1], "rd")) {
		if (argc == 4) {
			parse_bus_device_function(argv[2], &bus, &device, &function);
			offset = strtoul(argv[3], NULL, 16);
			enable_debug_mode();
			read_pci_dword(bus, device, function, offset);
			restore_debug_mode();
		}
	}
	else if (!strcmp(argv[1], "wb")) {
		if (argc == 5) {
			parse_bus_device_function(argv[2], &bus, &device, &function);
			offset = strtoul(argv[3], NULL, 16);
			value = strtoul(argv[4], NULL, 16);
			enable_debug_mode();
			write_pci_byte(bus, device, function, offset, value);
			restore_debug_mode();
		}
	}
	else if (!strcmp(argv[1], "ww")) {
		if (argc == 5) {
			parse_bus_device_function(argv[2], &bus, &device, &function);
			offset = strtoul(argv[3], NULL, 16);
			value = strtoul(argv[4], NULL, 16);
			enable_debug_mode();
			write_pci_word(bus, device, function, offset, value);
			restore_debug_mode();
		}
	}
	else if (!strcmp(argv[1], "wd")) {
		if (argc == 5) {
			parse_bus_device_function(argv[2], &bus, &device, &function);
			offset = strtoul(argv[3], NULL, 16);
			value = strtoul(argv[4], NULL, 16);
			enable_debug_mode();
			write_pci_dword(bus, device, function, offset, value);
			restore_debug_mode();
		}
	}
}


void show_cli_help() {
	printf("CLI Commands:\n");
	printf("debug -> toogles DEBUG_MODE\n");
	printf("pci r 0:2:0	 -> reads the configuration space\n");
	printf("pci r bar 0:2:0 0x10 -> read BAR info at offset 0x10\n");
	printf("pci rb 0:2:0 0 -> reads a byte from offset 0\n");
	printf("pci rw 0:2:0 4 -> reads a word from offset 4\n");
	printf("pci rd 0:2:0 4 -> reads a dword from offset 4\n");
	printf("pci wb 0:2:0 4 0x90 -> writes a byte to offset 4\n");
	printf("pci ww 0:2:0 4 0x9090 -> writes a word to offset 4\n");
	printf("pci wd 0:2:0 0xC 0x90909090 -> writes a dword to offset C\n");
	printf("physmem r 0x00000000 0x100 -> reads 0x100 bytes from physical address 0\n");
	printf("physmem r 0x00000000 0x100 file.bin -> reads 0x100 bytes from physical address 0 into file.bin\n");
	printf("physmem efimap -> prints the EFI memory map\n");
	printf("map2user 0x00000000 -> maps the physical address 0 and returns a virtual address to usermode\n");
	printf("alloc 0x100 -> allocates 0x100 bytes in kernel memory and maps it to usermode\n");
	printf("free 0xFEA000 -> frees that memory (if it was mapped by PlatboxDrv)\n");
	printf("dd 0xFEA0000 0x100 -> dumps the first 0x100 bytes of the memory address\n");
	printf("rdmsr 0x1F2 -> reads MSR 0x1F2\n");
	printf("rdmsr all 0x1B -> reads MSR 0x1B on all cores\n");
	printf("wrmsr 0x1F2 0xA000 -> Writes 0xA0000 into MSR 0x1F2\n");
	printf("uefivars -> dump UEFI variables -> requires admin privileges\n");
	printf("fuzzsmi 0x44 -> fuzz SW SMI number 0x44\n");
	printf("fuzzsmi -> fuzz ALL SW SMI\n");
	printf("chipset -> throws information about several platform configuration registers\n");	
	printf("dump_spi <filename (optional)> -> dumps the SPI Flash content into <filename> or 'spi_flash.bin'\n");	
	printf("s3bootscript <filename (optional)> -> dumps the S3 bootscript into <filename> or 's3_bootscript.bin'\n");
	printf("s3bootscript <RT_CODE_START> <RT_CODE_END> <filename (optional)>\n");
	
	#ifdef _WIN32
		printf("kernelhook patch ffff89868ba1ce30 -> creates a -breakpoint-hook into the ffff89868ba1ce30 callback\n");
		printf("kernelhook patch ffff89868ba1ce30 909090C3 -> creates a NOP NOP NOP RET hook into the ffff89868ba1ce30 callback\n");
		printf("kernelhook restores ffff89868ba1ce30 -> restores the original callback for ffff89868ba1ce30\n");
		printf("kernelhook remove -> deletes all hooks\n");
	#endif
	printf("--\n");
}




char command_line[512];
void run_cli() {

	InitializeDeltaFuzz();
	open_platbox_device();

	init_os_specifics();
	
	char **args;
	BOOL exit = FALSE;
	while (exit != TRUE) {
		printf(">>> ");
		fflush(stdout);
		get_user_input(command_line, sizeof(command_line));

		if (strlen(command_line) == 0) {
			continue;
		}
		args = parse_arguments(command_line, ' ');
		int _argc = (int)(size_t)args[0];
		_argc--;
		char **_argv = &args[1];

		if (!strcmp(_argv[0], "debug")) {
			DEBUG_MODE = DEBUG_MODE ^ 1;
			if (DEBUG_MODE) {
				printf("-> Debug: enabled\n");
			} else {
				printf("-> Debug: disabled\n");
			}
		}
		else if (!strcmp(_argv[0], "pci")) {
			parse_handle_pci_operation(_argc, _argv);
		}
		else if (!strcmp(_argv[0], "physmem")) {
			parse_handle_physmem_operation(_argc, _argv);
		}
		else if (!strcmp(_argv[0], "map2user")) {
			parse_handle_map2user_operation(_argc, _argv);
		}
		else if (!strcmp(_argv[0], "alloc")) {
			parse_handle_alloc_operation(_argc, _argv);
		}
		else if (!strcmp(_argv[0], "free")) {
			parse_handle_free_memory_operation(_argc, _argv);
		}
		else if (!strcmp(_argv[0], "dd")) {
			parse_handle_mem_dump_operation(_argc, _argv);
		}
		else if (!strcmp(_argv[0], "rdmsr")) {
			read_msr(_argc, _argv);
		}
		else if (!strcmp(_argv[0], "wrmsr")) {
			write_msr(_argc, _argv);
		}
		else if (!strcmp(_argv[0], "fuzzsmi")) {
			if (_argc > 1) {
				fuzz_smi_number(strtoul(_argv[1], NULL, 16));
			}
			else { // fuzzsmi
				fuzz_all_smi();
			}
		}
		else if (!strcmp(_argv[0], "chipset")) {
			get_chipset_information();
		}
		else if (!strcmp(_argv[0], "dump_spi")) {
			if (_argc > 1) {
				dump_spi_flash(_argv[1]);				
			} else {
				dump_spi_flash("spi_flash.bin");	
			}
		}

		else if (!strcmp(_argv[0], "s3bootscript")) {
			if (_argc == 1) {
				smm_dump_s3_bootscript("s3_bootscript.bin");					
			} else if (_argc == 2) {
				smm_dump_s3_bootscript(_argv[1]);				
			} else if (_argc > 2) {
				UINT64 rt_start = strtoull(_argv[1], NULL, 16);
				UINT64 rt_end   = strtoull(_argv[2], NULL, 16);
				if (_argc > 3) {
					smm_dump_s3_bootscript_manual(rt_start, rt_end, _argv[1]);
				} else {
					smm_dump_s3_bootscript_manual(rt_start, rt_end, "s3_bootscript.bin");
				}				
			}
		}	
		#ifdef _WIN32

			else if (!strcmp(_argv[0], "uefivars")) {
				list_uefi_variables();			
			}		

			
			else if (!strcmp(_argv[0], "kernelhook")) {
				if (_argc > 1) {
					parse_kernel_hook_operation(_argc, _argv);
				}
			}

		#endif

		else if (!strcmp(_argv[0], "?") || !strcmp(_argv[0], "help")) {
			show_cli_help();
		}
		else if (!strcmp(_argv[0], "exit")) {
			exit = TRUE;
		}
		free(args);
	}

	close_platbox_device();
}



void show_usage(char *exename) {	
	printf("> %s install -> installs the platbox driver (requires privileges)\n", exename);
	printf("> %s start -> starts the platbox driver (requires privileges)\n", exename);
	printf("> %s cli -> runs the CLI\n", exename);
	printf("> %s stop -> stops (unloads) the platbox driver\n", exename);
	printf("> %s remove -> removes the platbox driver\n", exename);
	printf("--\n");
}


int main(int argc, char **argv)
{	

	if (argc < 2) {
		show_usage(argv[0]);
		exit(-1);
	}
	if (!strcmp(argv[1], "cli")) {
		run_cli();
	}
	if (!strcmp(argv[1], "install")) {
		if (argc == 3) {
			install_driver(argv[2]);
		} else{
			install_driver(NULL);
		}
	}
	if (!strcmp(argv[1], "start")) {
		start_service();
	}
	if (!strcmp(argv[1], "stop")) {
		stop_service();
	}
	if (!strcmp(argv[1], "remove")) {
		remove_driver();
	}	

    return 0;
}

