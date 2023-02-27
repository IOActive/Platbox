#include "smi_fuzz.h"

void print_smi_call(SW_SMI_CALL *smi) {
	printf("SwSmiNumber: %02x\n", smi->SwSmiNumber);
	printf("SwSmiData: %02x\n", smi->SwSmiData);
	printf("rax: %p\n", smi->rax);
	printf("rbx: %p\n", smi->rbx);
	printf("rcx: %p\n", smi->rcx);
	printf("rdx: %p\n", smi->rdx);
	printf("rsi: %p\n", smi->rsi);
	printf("rdi: %p\n", smi->rdi);	
}

void fuzz_smi_number(int sw_smi_num) {
	DWORD bytesReturned;
	SW_SMI_CALL smi_call = { 0 };	
	UINT64 tseg_base = 0;
	UINT32 tseg_size = 0;
    UINT64 tseg_end = 0;

	printf("--> Fuzzing SW SMI Handlers 00-FF IO B2h\n");
	get_tseg_region(&tseg_base, &tseg_size);
    tseg_end = tseg_base + tseg_size;
	printf("-> TSEG base: %016llx\n", tseg_base);
	printf("-> TSEG end : %016llx\n", tseg_end);
	printf("-> SEED: %08x", g_fuzzer->Seed);
	
	printf("\n--> SwSmiNumber: %02x\n", sw_smi_num);
	int j = 0;
	for (; ; ) {			

		g_fuzzer->fuzz_blob(g_fuzzer, (UCHAR *)&smi_call, sizeof(SW_SMI_CALL));

		if (tseg_size > 0) {
			UINT32 offset = g_fuzzer->get_random(g_fuzzer) % tseg_size;
			UINT32 ptr = tseg_base + offset;

			if (g_fuzzer->get_random(g_fuzzer) & 1)
				smi_call.rax = ptr;
			if (g_fuzzer->get_random(g_fuzzer) & 1)
				smi_call.rbx = ptr;
			if (g_fuzzer->get_random(g_fuzzer) & 1)
				smi_call.rcx = ptr;
			if (g_fuzzer->get_random(g_fuzzer) & 1)
				smi_call.rdx = ptr;
			if (g_fuzzer->get_random(g_fuzzer) & 1)
				smi_call.rsi = ptr;
			if (g_fuzzer->get_random(g_fuzzer) & 1) {
				smi_call.rdi = ptr;
			}
		}

		smi_call.SwSmiNumber = sw_smi_num;
		smi_call.SwSmiData = g_fuzzer->get_random(g_fuzzer) % 256;
		//printf("\r--> iteration: %d", j);
		print_smi_call(&smi_call);

		#ifdef __linux__
			int status = ioctl(g_hDevice, IOCTL_ISSUE_SW_SMI, &smi_call);			
		#else //_WIN32
			NTSTATUS status;
			status = DeviceIoControl(
                g_hDevice,
                IOCTL_ISSUE_SW_SMI,
                &smi_call,
                sizeof(SW_SMI_CALL),
                NULL,
                0,
                &bytesReturned,
                NULL);
		
		#endif		
		j++;
	}	
}


void fuzz_all_smi() {
	DWORD bytesReturned;
	SW_SMI_CALL smi_call = { 0 };
    UINT64 tseg_base = 0;
	UINT32 tseg_size = 0;
    UINT64 tseg_end = 0;
	
	printf("--> Fuzzing SW SMI Handlers 00-FF IO B2h\n");
	get_tseg_region(&tseg_base, &tseg_size);
    tseg_end = tseg_base + tseg_size;
	printf("-> TSEG base: %016llx\n", tseg_base);
	printf("-> TSEG end : %016llx\n", tseg_end);
	printf("-> SEED: %08x", g_fuzzer->Seed);


    for (int i = 0x00; i < 0x100; i++) {
        
		printf("\n--> SwSmiNumber: %02x\n", i);
		for (int j = 0; j < 10000; j++) {
			
			g_fuzzer->fuzz_blob(g_fuzzer, (UCHAR *)&smi_call, sizeof(SW_SMI_CALL));

			if (tseg_size > 0) {
				UINT32 offset = g_fuzzer->get_random(g_fuzzer) % tseg_size;
				UINT32 ptr = tseg_base + offset;

				if (g_fuzzer->get_random(g_fuzzer) & 1)
					smi_call.rax = ptr;
				if (g_fuzzer->get_random(g_fuzzer) & 1) {
					if (g_fuzzer->get_random(g_fuzzer) & 1)
						smi_call.rbx = ptr;
					else
						smi_call.rbx = 0x24483139;
				}
					
				if (g_fuzzer->get_random(g_fuzzer) & 1)
					smi_call.rcx = ptr;
				if (g_fuzzer->get_random(g_fuzzer) & 1)
					smi_call.rdx = ptr;
				if (g_fuzzer->get_random(g_fuzzer) & 1)
					smi_call.rsi = ptr;
				if (g_fuzzer->get_random(g_fuzzer) & 1) {
					smi_call.rdi = ptr;
				}
			}
			smi_call.SwSmiNumber = i;
			smi_call.SwSmiData = g_fuzzer->get_random(g_fuzzer) % 256;
			printf("\r--> iteration: %d", j);

			#ifdef __linux__ 
				int status = ioctl(g_hDevice, IOCTL_ISSUE_SW_SMI, &smi_call);
			#elif _WIN32				
				NTSTATUS status;
				status = DeviceIoControl(
                    g_hDevice,
                    IOCTL_ISSUE_SW_SMI,
                    &smi_call,
                    sizeof(SW_SMI_CALL),
                    NULL,
                    0,
                    &bytesReturned,
                    NULL);	

			#else
			#endif
			
		}
	}
}