
#include "msr.h"

BOOL do_read_msr(UINT msr, UINT64 *res) {
	READ_MSR_CALL msr_call = { 0 };
	msr_call.msr = msr;

	UINT64 result = 0;

	#ifdef __linux__		
		msr_call.result = &result;

		int status = -1;		
		status = ioctl(g_hDevice, IOCTL_READ_MSR, &msr_call);
		if (status != 0) {
			debug_print("-> failed reading MSR:[%08x]\n", msr_call.msr);
			return FALSE;
		} else {
			debug_print("-> MSR:[%08x]: %lx\n", msr_call.msr, result);
		}
		

	#elif _WIN32
		DWORD bytesReturned = 0;
		NTSTATUS status = DeviceIoControl(g_hDevice, IOCTL_READ_MSR,
			&msr_call, sizeof(READ_MSR_CALL),
			&result, sizeof(UINT64),
			&bytesReturned, NULL);
		if (NT_SUCCESS(status)) {
			debug_print("-> MSR:[%08x]: %p\n", msr_call.msr, result);
		}
		else {
			debug_print("-> failed reading MSR:[%08x]\n", msr_call.msr);
			return FALSE;
		}
	#endif
	
	if (res != NULL) {
		*res = result;
	}

	return TRUE;
}


BOOL do_write_msr(UINT msr, UINT64 value) {
	WRITE_MSR_CALL msr_call = { 0 };
	msr_call.msr = msr;
	msr_call.value = value;

	#ifdef __linux__
		int status = -1;		
		status = ioctl(g_hDevice, IOCTL_WRITE_MSR, &msr_call);
		if (status != 0) {
			debug_print("-> failed writing to MSR:[%08x]\n", msr_call.msr);
			return FALSE;
		} else {
			debug_print("-> Attempted to write %016lx into MSR[%08x]\n", value, msr_call.msr);
		}
		
	#elif _WIN32
		DWORD bytesReturned = 0;
		NTSTATUS status = DeviceIoControl(g_hDevice, IOCTL_WRITE_MSR,
			&msr_call, sizeof(WRITE_MSR_CALL),
			NULL, NULL,
			&bytesReturned, NULL);
		if (NT_SUCCESS(status)) {
			debug_print("-> Attempted to write %016lx into MSR[%08x]\n", value, msr_call.msr);
		} else {
			debug_print("-> failed writing to MSR:[%08x]\n", msr_call.msr);
			return FALSE;
		}
	#endif
	
	return TRUE;
}

void read_msr(int argc, char **argv) {
	/* rdmsr <msr> */
	if (argc == 2) {		
		UINT msr = strtoul(argv[1], NULL, 16);
		enable_debug_mode();
		do_read_msr(msr, NULL);
		restore_debug_mode();
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

