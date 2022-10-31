// Written by Enrique Nissim (IOActive) 06/2018

#include "PlatboxClient.h"
#include "Util.h"

/*
extern "C" {
	void _run_attempt();
}*/


void InitializeDeltaFuzz() {
	CreateFuzzerObject(&g_fuzzer, 0, TRUE, FALSE);
}


void open_platbox_device() {
	#ifdef __linux__ 
		char device_name[256];  
		memset(device_name, 0x00, sizeof(device_name));
  		sprintf(device_name, "/dev/%s0", KERNETIX_DEVICE_NAME);
		int fd = open(device_name, O_RDWR);
		if (fd < 0) 
		{
			printf("Can't open device file: %s\n", device_name);
			exit(-1);
		}
		printf("%s opened successfully: %d\n", device_name, fd);
		g_hDevice = fd;
		
	#else // _WIN32
		HANDLE h = CreateFileW(
			L"\\\\.\\PlatboxDev",
			FILE_READ_ACCESS | FILE_WRITE_ACCESS,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
		);

		if (h == NULL || (int)h == -1) {
			printf("Error: %08x\n", GetLastError());
			printf("Check Platbox driver is loaded!\n");
			exit(-1);
		}

		g_hDevice = h;

	#endif	
	
}

void close_platbox_device() {
	#ifdef __linux__ 
		close(g_hDevice);
	#else // _WIN32
		CloseHandle(g_hDevice);
	#endif	
}


char _ret_shellcode[] = {
	//"\xc3" // ret
	"\x53\x56\x57\xFA\x48\x31\xC9\x48\xC7\xC2\x0F\x00\x00\x00\x4D\x31"
	"\xC0\x48\x31\xC0\xB0\x01\xC1\xE0\x1F\xC1\xE1\x10\xC1\xE2\x0B\x41"
	"\xC1\xE0\x08\x09\xC8\x09\xD0\x44\x09\xC0\x48\x89\xC3\x48\x8B\x7C"
	"\x24\x40\xCC\x48\x31\xC9\x48\xF7\xD1\x48\xFF\xC1\x48\x89\xC8\xC1"
	"\xE0\x02\x09\xD8\x66\xBA\xF8\x0C\xEF\x48\x31\xC0\x66\xBA\xFC\x0C"
	"\xED\x89\x04\x8F\x80\xF9\x40\x75\xE0\xFB\x5F\x5E\x5B\xC3"
};

void execute_shellcode() {
	DWORD bytesReturned = 0;
	
	char *shellcode = &_ret_shellcode[0];
	int size = 0x200;

	#ifdef __linux__ 		
		printf("execute_shellcode Unimplemented!");
	#elif _WIN32				
		NTSTATUS status;
		status = DeviceIoControl(g_hDevice, IOCTL_EXECUTE_SHELLCODE, shellcode, size, NULL, 0, &bytesReturned, NULL);
	#else
	#endif
	
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


void init_os_specifics() {
	#ifdef _WIN32 
		/* Set white foregorund*/
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

		resolve_nt_functions();
		find_pml4_idx();
	#endif
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
		
		else if (!strcmp(_argv[0], "uefivars")) {
			list_uefi_variables();			
		}		

		#ifdef _WIN32
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


void start_service() {
	#ifdef __linux__ 
		printf("start_service Unimplemented!");
		exit(-1);

	#else // _WIN32
		SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

		if (!hSCM) {
			printf("-> OpenSCManager failed! error: %08x\n", GetLastError());
			return;
		}

		SC_HANDLE hSvc = OpenServiceA(hSCM, SVC_NAME, SERVICE_START | DELETE | SERVICE_STOP);

		if (hSvc == NULL) {
			printf("-> could not open service %s - error: %08x\n", SVC_NAME, GetLastError());
		}
		else {
			if (StartService(hSvc, 0, NULL)) {
				printf("-> service started successfully!\n");
			}
			else {
				DWORD error = GetLastError();
				if (error == ERROR_SERVICE_ALREADY_RUNNING) {
					printf("-> service has already been started!\n");
				}
				else {
					printf("-> could not start service %s - error: %08x\n", SVC_NAME, error);
				}
			}

			CloseServiceHandle(hSCM);
		}

		CloseServiceHandle(hSCM);
	#endif	
}	

char service_exe[512];
void install_driver(char *driverNameArg) {
	memset(service_exe, 0x00, sizeof(service_exe));

	#ifdef __linux__ 
		printf("install_driver Unimplemented!");
		exit(-1);

	#else // _WIN32

		char driverName[128] = "\\Platbox.sys";

		char *pServiceExe = &service_exe[0];

		if (driverNameArg != NULL) {
			pServiceExe = driverNameArg;
		} else {
			GetCurrentDirectoryA(sizeof(service_exe), pServiceExe);
			strcat(pServiceExe, driverName);
		}

		printf("-> About to install the service: %s\n", pServiceExe);


		SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

		if (!hSCM) {
			printf("-> OpenSCManager failed! error: %08x\n", GetLastError());
			return;
		}


		SC_HANDLE hSvc = CreateServiceA(
			hSCM,
			SVC_NAME,
			SVC_NAME,
			SERVICE_START | DELETE | SERVICE_STOP,
			SERVICE_KERNEL_DRIVER,
			SERVICE_DEMAND_START,
			SERVICE_ERROR_IGNORE,
			pServiceExe,            
			NULL,                  
			NULL,                  
			NULL,                  
			NULL,                  
			NULL                  
		);


		if (hSvc == NULL) {
			DWORD error_code = GetLastError();
			if (error_code == ERROR_SERVICE_EXISTS) {
				start_service();
			}
			else {
				printf("-> error creating the service - error: %08x\n", error_code);
			}
		}
		else {
			SERVICE_STATUS status;
			if (QueryServiceStatus(hSvc, &status)) {
				if (status.dwCurrentState == SERVICE_RUNNING) {
					printf("-> %s service created and started successfully!\n", SVC_NAME);
				}			
			}
			CloseServiceHandle(hSvc);
		}

		CloseServiceHandle(hSCM);


	#endif
}

void stop_service() {
	#ifdef __linux__ 
		printf("stop_service Unimplemented!");
		exit(-1);

	#else // _WIN32
		SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

		if (!hSCM) {
			printf("-> OpenSCManager failed! error: %08x\n", GetLastError());
			return;
		}

		SC_HANDLE hSvc = OpenServiceA(hSCM, SVC_NAME, SERVICE_ALL_ACCESS);
		SERVICE_STATUS status;
		if (ControlService(hSvc, SERVICE_CONTROL_STOP, &status)) {
			printf("-> service stopped successfully!\n");
		}
		else {
			printf("-> error stopping service: %08x\n", GetLastError());
		}

		CloseServiceHandle(hSvc);
		CloseServiceHandle(hSCM);

	#endif
}

void remove_driver() {
	#ifdef __linux__ 
		printf("remove_driver Unimplemented!");
		exit(-1);

	#else // _WIN32
		SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

		if (!hSCM) {
			printf("-> OpenSCManager failed! error: %08x\n", GetLastError());
			return;
		}

		SC_HANDLE hSvc = OpenServiceA(hSCM, SVC_NAME, SERVICE_START | DELETE | SERVICE_STOP);

		if (hSvc == NULL) {
			printf("-> service can't be opened - error: %08x\n", GetLastError());
		}		
		else {
			if (DeleteService(hSvc)) {
				printf("-> %s service was removed successfully\n", SVC_NAME);
			}
			else {
				printf("-> service can't be removed - error: %08x\n", GetLastError());
			}
			CloseServiceHandle(hSvc);
		}
		CloseServiceHandle(hSCM);

	#endif
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

