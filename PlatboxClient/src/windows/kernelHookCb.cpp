#include "__win.h"
#include "Util.h"

/// Windows only
#ifdef _WIN32 

	void hook_kernel_callback(PVOID target_address) {
		PATCH_CALLBACK p = { 0 };
		p.contentSize = 0;
		p.targetAddress = target_address;

		UINT64 hook_address = 0;

		DWORD bytesReturned = 0;
		NTSTATUS status = DeviceIoControl(g_hDevice, IOCTL_PATCH_CALLBACK,
			&p, sizeof(PATCH_CALLBACK),
			&hook_address, sizeof(UINT64),
			&bytesReturned, NULL);
		if (NT_SUCCESS(status)) {
			printf("-> address of new kernel hook object: %p\n", hook_address);
		}
		else {
			printf("-> error hooking kernel cb\n");
		}
	}

	void hook_kernel_callback_with_shellcode(PVOID target_address, char *shellcode) {
		PATCH_CALLBACK p = { 0 };
		unsigned char *pOpcodes = NULL;
		UINT32 shlen = 0;
		hexstring_to_opcodes(shellcode, &pOpcodes, &shlen);

		p.contentSize = shlen;
		p.patchContent = pOpcodes;
		p.targetAddress = target_address;

		UINT64 hook_address = 0;

		DWORD bytesReturned = 0;
		NTSTATUS status = DeviceIoControl(g_hDevice, IOCTL_PATCH_CALLBACK,
			&p, sizeof(PATCH_CALLBACK),
			&hook_address, sizeof(UINT64),
			&bytesReturned, NULL);
		if (NT_SUCCESS(status)) {
			printf("-> address of new kernel hook object: %p\n", hook_address);
		}
		else {
			printf("-> error hooking kernel cb\n");
		}

		free(pOpcodes);
	}

	void restore_kernel_callback(PVOID target_address) {
		RESTORE_CALLBACK p = { 0 };
		p.targetAddress = target_address;

		DWORD bytesReturned = 0;
		NTSTATUS status = DeviceIoControl(g_hDevice, IOCTL_RESTORE_CALLBACK,
			&p, sizeof(RESTORE_CALLBACK),
			NULL, NULL,
			&bytesReturned, NULL);
		if (NT_SUCCESS(status)) {
			printf("-> kernel hook at %p was removed\n", target_address);
		}
		else {
			printf("-> error restoring kernel cb\n");
		}
	}

	void remove_all_kernel_callbacks() {
		DWORD bytesReturned = 0;
		NTSTATUS status = DeviceIoControl(g_hDevice, IOCTL_REMOVE_ALL_CALLBACKS_HOOKS,
			NULL, NULL,
			NULL, NULL,
			&bytesReturned, NULL);
		if (NT_SUCCESS(status)) {
			printf("-> all kernel hooks were removed!\n");
		}
		else {
			printf("-> error removing kernel hooks\n");
		}
	}

	void parse_kernel_hook_operation(int argc, char **argv) {
		if (!strcmp(argv[1], "patch")) {
			// kernelhook patch ffff89868ba1ce30
			if (argc == 3) {
				UINT64 address = strtoull(argv[2], NULL, 16);
				hook_kernel_callback((PVOID)address);
			}
			if (argc == 4) {
				// kernelhook patch ffff89868ba1ce30 909090c3 
				UINT64 address = strtoull(argv[2], NULL, 16);
				hook_kernel_callback_with_shellcode((PVOID)address, argv[3]);
			}
		}
		if (!strcmp(argv[1], "restore")) {
			if (argc == 3) {
				UINT64 address = strtoull(argv[2], NULL, 16);
				restore_kernel_callback((PVOID)address);
			}
		}
		if (!strcmp(argv[1], "remove")) {		
			remove_all_kernel_callbacks();
		}
	}


#endif

