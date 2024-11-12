#include "amd_psp.h"


#include "pci.h"
#include "Util.h"
#include "physmem.h"
#include "msr.h"
#include "UEFIVars.h"


UINT32 smu_read32(UINT32 reg) {
	write_pci_dword(0,0,0, SMU_INDEX_ADDR >> 2, reg);
	return read_pci_dword(0,0,0, SMU_DATA_ADDR >> 2);
}

void smu_write32(UINT32 reg, UINT32 val) {
	write_pci_dword(0,0,0, SMU_INDEX_ADDR >> 2, reg);
	write_pci_dword(0,0,0, SMU_DATA_ADDR >> 2, val);
}



UINT64 get_psp_mbox_addr() {
    UINT64 amd_psp_addr = 0;
    if (do_read_msr(MSR_AMD_PSP_ADDR, &amd_psp_addr) ) {
        return amd_psp_addr + PSP_MAILBOX_COMMAND_OFFSET;        
    }

    return 0;
}


UINT32 read_psp_psb_status() {
    return smu_read32(SMU_PSP_PUBLIC_BASE + PSP_MAILBOX_PSB_STATUS_OFFSET);
}

UINT32 read_psp_mbox_config2() {
    return smu_read32(SMU_PSP_PUBLIC_BASE + PSP_MAILBOX_CONFIG2_OFFSET);
}

UINT32 read_psp_feature_reg() {
    return smu_read32(SMU_PSP_PUBLIC_BASE + PSP_MAILBOX_FEATURE_REG);
}

UINT32 read_psp_mbox_cmd_status() {
    return smu_read32(SMU_PSP_PUBLIC_BASE + PSP_MAILBOX_COMMAND_OFFSET);
}

void set_psp_mbox_cmd_status(UINT32 status) {    
    smu_write32(SMU_PSP_PUBLIC_BASE + PSP_MAILBOX_COMMAND_OFFSET, status);
}

UINT64 read_psp_mbox_buffer_address() {    
    UINT64 addr = smu_read32(SMU_PSP_PUBLIC_BASE + PSP_MAILBOX_BUFFER_H_OFFSET) << 32;
    return addr | smu_read32(SMU_PSP_PUBLIC_BASE + PSP_MAILBOX_BUFFER_L_OFFSET);
}

void set_psp_mbox_buffer_address(UINT64 address) {    
    smu_write32(SMU_PSP_PUBLIC_BASE + PSP_MAILBOX_BUFFER_H_OFFSET, (UINT32) (address >> 32));
    smu_write32(SMU_PSP_PUBLIC_BASE + PSP_MAILBOX_BUFFER_L_OFFSET, (UINT32) address);
}


UINT16 get_psp_mbox_status() {
    union pspv2_mbox_command mbox_cmd;
    mbox_cmd.val = read_psp_mbox_cmd_status();
    return mbox_cmd._mbox_fields.status;
}

void write_psp_mbox_cmd(UINT8 cmd) {
    union pspv2_mbox_command mbox_cmd = {0};
    mbox_cmd._mbox_fields.command = cmd;
    set_psp_mbox_cmd_status(mbox_cmd.val);
}



void send_psp_msg(BYTE psp_cmd, UINT64 buffer_physaddr)
{

    UINT32 mailbox_status = 0x0;

    mailbox_status = read_psp_mbox_cmd_status();

    // Check if recovery flag is set
    if ((mailbox_status & 0x40000000) != 0)
    {
        printf("Recovery flag detected\n");
    }

    // Check if reset required flag is set
    if ((mailbox_status & 0x20000000) != 0)
    {
        printf("Reset required flag detected\n");
    }

    // Check if mailbox is ready
    while (((mailbox_status & 0x80000000) == 0) || ((mailbox_status & 0x00FF0000) != 0))
    {
        printf("Mailbox is not ready\n");
        mailbox_status = read_psp_mbox_cmd_status();
    }

    
    set_psp_mbox_buffer_address(buffer_physaddr);
    write_psp_mbox_cmd(psp_cmd);

    // Wait until done
    while ((read_psp_mbox_cmd_status() & 0x00FF0000) != 0)
        continue;

}



void armor_spi_transaction() {


    // Prepare PSP payload buffer
    const UINT alloc_size = PAGE_SIZE;
    struct alloc_user_physmem user_page = {0};
    BOOL res = alloc_user_mem(alloc_size, &user_page);

    if (!res) {
        return;
    }

    struct alloc_user_physmem aux_page = {0};
    res = alloc_user_mem(alloc_size, &aux_page);

    *(UINT32 *)aux_page.va = 0x41414141;

    ArmorSpiTransactionRecord *record = (ArmorSpiTransactionRecord *) user_page.va;
    record->record_size = sizeof(*record);
    record->flash_address = 0x10;
    record->trn_type = PSP_CMD_ARMOR_SPI_TRANSACTION_WRITE_OP;
    record->trn_length = 4;
    record->aux_memory_page = aux_page.pa;
    
    printf("psp message sent!\n");
    send_psp_msg(PSP_CMD_ARMOR_SPI_TRANSACTION, user_page.pa);

    unmap_physical_memory((void *) user_page.va, PAGE_SIZE);
    unmap_physical_memory((void *) aux_page.va, PAGE_SIZE);

}

static const char *psb_test_status_to_string(UINT32 status)
{
	switch (status) {
	case PSB_TEST_STATUS_PASS:
		return "Psb Test Status PASS";
	case PSB_TEST_STATUS_FUSE_READ_ERR:
		return "Error reading fuse info";
	case PSB_TEST_STATUS_BIOS_KEY_BAD_USAGE:
		return "OEM BIOS signing key usage flag violation";
	case PSB_TEST_STATUS_BIOS_RTM_SIG_NOENT:
		return "BIOS RTM signature entry not found";
	case PSB_TEST_STATUS_BIOS_RTM_COPY_ERR:
		return "BIOS copy to DRAM failed";
	case PSB_TEST_STATUS_BIOS_RTM_BAD_SIG:
		return "BIOS RTM signature verification failed";
	case PSB_TEST_STATUS_BIOS_KEY_BAD_SIG:
		return "OEM BIOS signing key failed signature verification";
	case PSB_TEST_STATUS_PLATFORM_BAD_ID:
		return "Platform vendor id and/or model id binding violation";
	case PSB_TEST_STATUS_BIOS_COPY_BIT_UNSET:
		return "BIOS copy bit unset for reset image";
	case PSB_TEST_STATUS_BIOS_CA_BAD_SIG:
		return "OEM BIOS signing CA key failed signature verification";
	case PSB_TEST_STATUS_BIOS_CA_BAD_USAGE:
		return "OEM BIOS signing CA key usage flag violation";
	case PSB_TEST_STATUS_BIOS_KEY_BAD_REVISION:
		return "OEM BIOS signing key revision violation";
	default:
		return "Unknown failure";
	}
}

void print_psp_security_configuration() {
    UINT32 config = read_psp_psb_status();

    printf("PSP Config at +10994h: %08x\n", config);
    printf(" - Platform vendor ID: %02x\n", config & 0xFF);
	printf(" - Platform model ID: %02x\n", (config >> 8) & 0xF);
	printf(" - BIOS key revision ID: %04x\n", (config >> 12) & 0xFFFF);
	printf(" - Root key select: %02x\n", (config >> 16) & 0xF);
	printf(" - Platform Secure Boot Enable: %d\n", (config >> 24) & 1);
	printf(" - Disable BIOS key anti-rollback:  %d\n", (config >> 25) & 1);
	printf(" - Disable AMD key usage:  %d\n", (config >> 26) & 1);
	printf(" - Disable secure debug unlock:  %d\n", (config >> 27) & 1);
	printf(" - Customer key unlock:  %d\n", (config >> 28) & 1);
    printf("\n");


    config = read_psp_mbox_config2();

    printf("PSP Config at +10998h: %08x\n", config);
    printf("- PSB status: %02x\n", config & 0xFF);
    printf("- PSB fusing readiness: %d\n", (config >> 8) & 1);
    printf("- HSTI state: %02x\n", config >> 28);
    printf("\n");


    printf("HSTI State: %x\n", get_psp_hsti_state());
    config = read_psp_feature_reg();
    printf("PSP Feature REG: %x\n", config);
    config = config >> PSP_CAPABILITY_PSP_SECURITY_OFFSET;
    printf("PSP Feature REG (Security Capabilities): %x\n", config);
    printf(" - PSP Security Fused Part: %x\n", (config >> PSP_SECURITY_FUSED_PART) & 1);
	printf(" - PSP Security Debug Lock On: %x\n", (config >> PSP_SECURITY_DEBUG_LOCK_ON) & 1);
	printf(" - PSP Security TSME STATUS: %x\n", (config >> PSP_SECURITY_TSME_STATUS) & 1);
	printf(" - PSP Security Anti-Rollback Status: %x\n", (config >> PSP_SECURITY_ANTI_ROLLBACK_STATUS) & 1);
	printf(" - PSP Security RPMC Production Enabled: %x\n", (config >> PSP_SECURITY_RPMC_PRODUCTION_ENABLED) & 1);
	printf(" - PSP Security RPMC SPIROM Available:  %x\n", (config >> PSP_SECURITY_RPMC_SPIROM_AVAILABLE) & 1);
	printf(" - PSP Security HSP TPM Available:  %x\n", (config >> PSP_SECURITY_HSP_TPM_AVAILABLE) & 1);
	printf(" - PSP Security ROM ARMOR Enforced:  %x\n", (config >> PSP_SECURITY_ROM_ARMOR_ENFORCED) & 1);

}

UINT32 get_psp_hsti_state() {
	struct alloc_user_physmem aux_page = {0};
    bool res = alloc_user_mem(PAGE_SIZE, &aux_page);

	char *buffer = (char *) aux_page.va;

    memset(buffer, 0, 16);
    *(UINT32 *)(buffer + 0x0) = 0xC;
    *(UINT32 *)(buffer + 0x4) = 0x0;
    *(UINT32 *)(buffer + 0x8) = 0x0;

    send_psp_msg(PSP_CMD_GET_HSTI_STATE, aux_page.pa);

    UINT32 hsti = *(UINT32 *)(buffer + 8);
    unmap_physical_memory((void *)buffer, PAGE_SIZE);

    return hsti;
}