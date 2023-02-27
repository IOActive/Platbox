#include "CbHooks.h"

char breakpoint[] = {
	0x90, 0x90, 0x90, // NOPs
	0xCC, // int 3
};

char jmp_to_rax[] = {
	0x48, 0xb8, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, // movabs rax,0x4141414141414141
	0xff, 0xe0 // jmp    rax
};

HookCallback *g_root_CbHookList = NULL;


void patch_address(PVOID _where, PVOID _what) {
	PMDL mdl = IoAllocateMdl(_where, sizeof(PVOID), FALSE, FALSE, NULL);
	MmProbeAndLockPages(mdl, KernelMode, IoWriteAccess);
	PVOID kernel_addr = MmMapLockedPagesSpecifyCache(mdl, KernelMode, MmNonCached, NULL, FALSE, NormalPagePriority);
	*(PVOID *)(kernel_addr) = _what;
	MmUnmapLockedPages(kernel_addr, mdl);
	MmUnlockPages(mdl);
	IoFreeMdl(mdl);

}

PVOID CreateCbHook(PPATCH_CALLBACK pPatchCallback) {
	// This memory is for the callback hooks that could be set by the client
	UINT32 size;
	if (pPatchCallback->contentSize > 0) {
		size = sizeof(HookCallback) + pPatchCallback->contentSize + sizeof(jmp_to_rax);
	}
	else {
		size = sizeof(HookCallback) + sizeof(breakpoint) + sizeof(jmp_to_rax);
	}

	char *mem = (char *)ExAllocatePool(NonPagedPoolExecute, size);
	HookCallback *new_hookCb = mem;
	new_hookCb->targetAddress = pPatchCallback->targetAddress;
	new_hookCb->originalCbValue = *(PVOID *)(pPatchCallback->targetAddress);
	new_hookCb->next = g_root_CbHookList;
	g_root_CbHookList = new_hookCb;

	if (pPatchCallback->contentSize > 0) {
		__try {
			//ProbeForRead(pPatchCallback->patchContent, pPatchCallback->contentSize, 0);
			memcpy(&(new_hookCb->hookCode[0]), pPatchCallback->patchContent, pPatchCallback->contentSize);
			mem = (char *)&(new_hookCb->hookCode[0]) + pPatchCallback->contentSize;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			memcpy(&(new_hookCb->hookCode[0]), breakpoint, sizeof(breakpoint));
			mem = (char *)&(new_hookCb->hookCode[0]) + sizeof(breakpoint);
		}
	}
	else {
		memcpy(&(new_hookCb->hookCode[0]), breakpoint, sizeof(breakpoint));
		mem = (char *)&(new_hookCb->hookCode[0]) + sizeof(breakpoint);
	}

	// Always end in jmp_to_rax patched with the OriginalCbValue
	memcpy(mem, jmp_to_rax, sizeof(jmp_to_rax));
	*(PVOID *)(mem + 2) = new_hookCb->originalCbValue;

	// finally patch the address
	patch_address(pPatchCallback->targetAddress, &new_hookCb->hookCode[0]);
	
	return new_hookCb;
}


void RemoveCbHook(PRESTORE_CALLBACK pRestoreCallback) {
	// This memory is for the callback hooks that could be set by the client
	if (g_root_CbHookList != NULL) {
		HookCallback *curr = g_root_CbHookList;
		HookCallback *prev = curr;
		while (curr != NULL && curr->targetAddress != pRestoreCallback->targetAddress) {
			prev = curr;
			curr = curr->next;
		}
		if (curr && curr->targetAddress == pRestoreCallback->targetAddress) {
			// Restore the original callback
			patch_address(curr->targetAddress, curr->originalCbValue);
			if (prev == curr) {
				// prev == curr == g_CbHookList
				if (curr->next == NULL) {
					g_root_CbHookList = NULL;
				}
				else {
					g_root_CbHookList = curr->next;
				}
			}
			prev->next = curr->next;
			ExFreePool(curr);
		}
	}
}

void RemoveAllCbHooks() {
	if (g_root_CbHookList != NULL) {
		HookCallback *curr = g_root_CbHookList;
		HookCallback *next;
		while (curr != NULL) {
			patch_address(curr->targetAddress, curr->originalCbValue);
			next = curr->next;
			ExFreePool(curr);
			curr = next;
		}
		g_root_CbHookList = NULL;
	}
}