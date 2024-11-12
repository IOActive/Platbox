#include "global.h"

//
// A lot from there was taken from Coreboot project
//

#define PSP_CMDRESP_RESP		BIT(31)
#define PSP_CMDRESP_ERR_MASK		0xffff

#define MAX_PSP_NAME_LEN		16


// Taken from linux kernel

#define PSP_CAPABILITY_SEV			BIT(0)
#define PSP_CAPABILITY_TEE			BIT(1)
#define PSP_CAPABILITY_PSP_SECURITY_REPORTING	BIT(7)

#define PSP_CAPABILITY_PSP_SECURITY_OFFSET	8
/*
 * The PSP doesn't directly store these bits in the capability register
 * but instead copies them from the results of query command.
 *	Q
 * The offsets from the query command are below, and shifted when used.
 */
#define PSP_SECURITY_FUSED_PART			0
#define PSP_SECURITY_DEBUG_LOCK_ON		2
#define PSP_SECURITY_TSME_STATUS		5
#define PSP_SECURITY_ANTI_ROLLBACK_STATUS	7
#define PSP_SECURITY_RPMC_PRODUCTION_ENABLED	8
#define PSP_SECURITY_RPMC_SPIROM_AVAILABLE	9
#define PSP_SECURITY_HSP_TPM_AVAILABLE		10
#define PSP_SECURITY_ROM_ARMOR_ENFORCED		11


#define MSR_AMD_PSP_ADDR	0xc00110a2
#define PSP_MAILBOX_COMMAND_OFFSET	0x10570 /* 4 bytes */
#define PSP_MAILBOX_BUFFER_L_OFFSET	0x10574 /* 4 bytes */
#define PSP_MAILBOX_BUFFER_H_OFFSET	0x10578 /* 4 bytes */


#define PSP_MAILBOX_FEATURE_REG 0x109FC
#define PSP_MAILBOX_INTEN_REG 0x10690
#define PSP_MAILBOX_INTSTS_REG 0x10694


#define SMU_INDEX_ADDR		0xb8 /* 32 bit */
#define SMU_DATA_ADDR		0xbc /* 32 bit */

UINT32 smu_read32(UINT32 reg);
void smu_write32(UINT32 reg, UINT32 val);

#pragma pack (push, 1)
union pspv2_mbox_command {
	UINT32 val;
	struct pspv2_mbox_cmd_fields {
		UINT16 status;
		UINT32 command;
		UINT32 reserved:6;
		UINT32 recovery:1;
		UINT32 ready:1;
	} _mbox_fields;
};

#pragma pack (pop)


#define SMU_PSP_PUBLIC_BASE			0x3800000

UINT32 read_psp_feature_reg();
UINT64 get_psp_mbox_addr();
UINT32 read_psp_mbox_cmd_status();
void set_psp_mbox_cmd_status(UINT32 status);
UINT64 read_psp_mbox_buffer_address();
void set_psp_mbox_buffer_address(UINT64 address);
void send_psp_msg(BYTE psp_cmd, UINT64 buffer_physaddr);

#define PSP_MAILBOX_ADDR        0xFDE00000 + PSP_MAILBOX_COMMAND_OFFSET
#define PSP_MAILBOX_PAGE        (PSP_MAILBOX_ADDR & 0xFFFFF000)
#define PSP_MAILBOX_OFFSET      (PSP_MAILBOX_ADDR & 0x00000FFF)

#define BUFFER_ADDR             0x1000

#define PSP_CMD_GET_CAPABILITIES    0x27
#define PSP_CMD_GET_HSTI_STATE      0x14

#define PSP_CMD_ARMOR_SPI_TRANSACTION 0x51

#define PSP_CMD_ARMOR_SPI_TRANSACTION_READ_OP 0x01
#define PSP_CMD_ARMOR_SPI_TRANSACTION_WRITE_OP 0x02
#define PSP_CMD_ARMOR_SPI_TRANSACTION_ERASE_OP 0x03


#pragma pack (push, 1)
typedef struct _ArmorSpiTransactionRecord { // 0x20
   /* 0x00 */ UINT32 record_size;
   /* 0x04 */ UINT32 unused;
   /* 0x08 */ UINT32 trn_type; // 2 Write | 3 Erase
   /* 0x0C */ UINT64 aux_memory_page;
   /* 0x14 */ UINT32 flash_address;
   /* 0x18 */ UINT32 trn_length; // For erase must be 0x1000
   /* 0x1C */ UINT32 unused2;
} ArmorSpiTransactionRecord;
#pragma pack (pop)

void armor_spi_transaction();



#define PSP_MAILBOX_PSB_STATUS_OFFSET 0x10994
#define PSP_MAILBOX_CONFIG2_OFFSET 0x10998


/* PSB Test Status and Error Codes (doc#56654) */
#define PSB_TEST_STATUS_PASS			0x00
#define PSB_TEST_STATUS_FUSE_READ_ERR		0x3e
#define PSB_TEST_STATUS_BIOS_KEY_BAD_USAGE	0x81
#define PSB_TEST_STATUS_BIOS_RTM_SIG_NOENT	0x82
#define PSB_TEST_STATUS_BIOS_RTM_COPY_ERR	0x83
#define PSB_TEST_STATUS_BIOS_RTM_BAD_SIG	0x84
#define PSB_TEST_STATUS_BIOS_KEY_BAD_SIG	0x85
#define PSB_TEST_STATUS_PLATFORM_BAD_ID		0x86
#define PSB_TEST_STATUS_BIOS_COPY_BIT_UNSET	0x87
#define PSB_TEST_STATUS_BIOS_CA_BAD_SIG		0x8a
#define PSB_TEST_STATUS_BIOS_CA_BAD_USAGE	0x8b
#define PSB_TEST_STATUS_BIOS_KEY_BAD_REVISION	0x8c

#define PSB_TEST_STATUS_MASK 0xFF







struct PspPsbStatus {
    UINT32 platformVendorId: 8;
	UINT32 platformModelId: 4;
	UINT32 biosKeyRevisionId: 4;
	UINT32 rootKeySelect: 4;
	UINT32 unknown: 4;
	UINT32 platformSecureBootEnable: 1;
	UINT32 disableBiosKeyAntiRollback: 1;
	UINT32 disableAmdKeyUsage: 1;
	UINT32 disableSecureDebugUnlock: 1;
	UINT32 customerKeyUnlock: 1;
	UINT32 unkown2: 4;
};

UINT32 read_psp_psb_status();
UINT32 read_psp_mbox_config2();
UINT32 get_psp_hsti_state();


void print_psp_security_configuration();