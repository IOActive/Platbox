
#define S_GPIO_PCR_PADCFG               0x10

//
// Pad Configuration Register DW0
//

//Pad Reset Config
#define B_GPIO_PCR_RST_CONF             (BIT31 | BIT30)
#define N_GPIO_PCR_RST_CONF             30
#define V_GPIO_PCR_RST_CONF_POW_GOOD    0x00
#define V_GPIO_PCR_RST_CONF_DEEP_RST    0x01
#define V_GPIO_PCR_RST_CONF_GPIO_RST    0x02
#define V_GPIO_PCR_RST_CONF_RESUME_RST  0x03  // Only for GPD Group

//RX Pad State Select
#define B_GPIO_PCR_RX_PAD_STATE         BIT29
#define N_GPIO_PCR_RX_PAD_STATE         29
#define V_GPIO_PCR_RX_PAD_STATE_RAW     0x00
#define V_GPIO_PCR_RX_PAD_STATE_INT     0x01

//RX Raw Overrride to 1
#define B_GPIO_PCR_RX_RAW1              BIT28
#define N_GPIO_PCR_RX_RAW1              28
#define V_GPIO_PCR_RX_RAW1_DIS          0x00
#define V_GPIO_PCR_RX_RAW1_EN           0x01

//RX Level/Edge Configuration
#define B_GPIO_PCR_RX_LVL_EDG           (BIT26 | BIT25)
#define N_GPIO_PCR_RX_LVL_EDG           25
#define V_GPIO_PCR_RX_LVL_EDG_LVL       0x00
#define V_GPIO_PCR_RX_LVL_EDG_EDG       0x01
#define V_GPIO_PCR_RX_LVL_EDG_0         0x02
#define V_GPIO_PCR_RX_LVL_EDG_RIS_FAL   0x03

//RX Invert
#define B_GPIO_PCR_RXINV                BIT23
#define N_GPIO_PCR_RXINV                23
#define V_GPIO_PCR_RXINV_NO             0x00
#define V_GPIO_PCR_RXINV_YES            0x01

//GPIO Input Route IOxAPIC
#define B_GPIO_PCR_RX_APIC_ROUTE        BIT20
#define N_GPIO_PCR_RX_APIC_ROUTE        20
#define V_GPIO_PCR_RX_APIC_ROUTE_DIS    0x00
#define V_GPIO_PCR_RX_APIC_ROUTE_EN     0x01

//GPIO Input Route SCI
#define B_GPIO_PCR_RX_SCI_ROUTE         BIT19
#define N_GPIO_PCR_RX_SCI_ROUTE         19
#define V_GPIO_PCR_RX_SCI_ROUTE_DIS     0x00
#define V_GPIO_PCR_RX_SCI_ROUTE_EN      0x01

//GPIO Input Route SMI
#define B_GPIO_PCR_RX_SMI_ROUTE         BIT18
#define N_GPIO_PCR_RX_SMI_ROUTE         18
#define V_GPIO_PCR_RX_SMI_ROUTE_DIS     0x00
#define V_GPIO_PCR_RX_SMI_ROUTE_EN      0x01

//GPIO Input Route NMI
#define B_GPIO_PCR_RX_NMI_ROUTE         BIT17
#define N_GPIO_PCR_RX_NMI_ROUTE         17
#define V_GPIO_PCR_RX_NMI_ROUTE_DIS     0x00
#define V_GPIO_PCR_RX_NMI_ROUTE_EN      0x01

//GPIO Pad Mode
#define B_GPIO_PCR_PAD_MODE             (BIT12 | BIT11 | BIT10)
#define N_GPIO_PCR_PAD_MODE             10
#define V_GPIO_PCR_PAD_MODE_GPIO        0
#define V_GPIO_PCR_PAD_MODE_NAT_1       1
#define V_GPIO_PCR_PAD_MODE_NAT_2       2
#define V_GPIO_PCR_PAD_MODE_NAT_3       3
#define V_GPIO_PCR_PAD_MODE_NAT_4       4 // SPT-H only

//GPIO RX Disable
#define B_GPIO_PCR_RXDIS                BIT9
#define N_GPIO_PCR_RXDIS                9
#define V_GPIO_PCR_RXDIS_EN             0x00
#define V_GPIO_PCR_RXDIS_DIS            0x01

//GPIO TX Disable
#define B_GPIO_PCR_TXDIS                BIT8
#define N_GPIO_PCR_TXDIS                8
#define V_GPIO_PCR_TXDIS_EN             0x00
#define V_GPIO_PCR_TXDIS_DIS            0x01

//GPIO RX State
#define B_GPIO_PCR_RX_STATE             BIT1
#define N_GPIO_PCR_RX_STATE             1
#define V_GPIO_PCR_RX_STATE_LOW         0x00
#define V_GPIO_PCR_RX_STATE_HIGH        0x01

//GPIO TX State
#define B_GPIO_PCR_TX_STATE             BIT0
#define N_GPIO_PCR_TX_STATE             0
#define V_GPIO_PCR_TX_STATE_LOW         0x00
#define V_GPIO_PCR_TX_STATE_HIGH        0x01

//
// Pad Configuration Register DW1
//

//Padtol
#define B_GPIO_PCR_PADTOL               BIT25
#define N_GPIO_PCR_PADTOL               25
#define V_GPIO_PCR_PADTOL_NONE          0x00
#define V_GPIO_PCR_PADTOL_CLEAR         0x00
#define V_GPIO_PCR_PADTOL_SET           0x01

//Termination
#define B_GPIO_PCR_TERM                (BIT13 | BIT12 | BIT11 | BIT10)
#define N_GPIO_PCR_TERM                 10
#define V_GPIO_PCR_TERM_WPD_NONE        0x00
#define V_GPIO_PCR_TERM_WPD_5K          0x02
#define V_GPIO_PCR_TERM_WPD_20K         0x04
#define V_GPIO_PCR_TERM_WPU_NONE        0x08
#define V_GPIO_PCR_TERM_WPU_1K          0x09
#define V_GPIO_PCR_TERM_WPU_2K          0x0B
#define V_GPIO_PCR_TERM_WPU_5K          0x0A
#define V_GPIO_PCR_TERM_WPU_20K         0x0C
#define V_GPIO_PCR_TERM_WPU_1K_2K       0x0D
#define V_GPIO_PCR_TERM_NATIVE          0x0F

//Interrupt number
#define B_GPIO_PCR_INTSEL               0x7F
#define N_GPIO_PCR_INTSEL               0

//
//Debounce
#define B_GPIO_PCR_DEBOUNCE             (BIT4 | BIT3 | BIT2 | BIT1)
#define N_GPIO_PCR_DEBOUNCE              1

//Debounce Enable
#define B_GPIO_PCR_DEBEN                 BIT0
#define N_GPIO_PCR_DEBEN                 0

//
// Ownership
//
#define V_GPIO_PCR_OWN_GPIO             0x01
#define V_GPIO_PCR_OWN_ACPI             0x00

//
// GPE
//
#define V_GPIO_PCR_GPE_EN               0x01
#define V_GPIO_PCR_GPE_DIS              0x00
//
// SMI
//
#define V_GPIO_PCR_SMI_EN               0x01
#define V_GPIO_PCR_SMI_DIS              0x00
//
// NMI
//
#define V_GPIO_PCR_NMI_EN               0x01
#define V_GPIO_PCR_NMI_DIS              0x00
