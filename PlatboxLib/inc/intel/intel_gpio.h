#pragma once
#include "types.h"
#include "global.h"
#include "intel_pch_regs_gpio.h"

#define SBREG_BAR 0xFD000000


// Possible values of Pad Ownership
//
typedef enum {
  GpioPadOwnHost = 0x0,
  GpioPadOwnCsme = 0x1,
  GpioPadOwnIsh  = 0x2,
} GPIO_PAD_OWN;


#define INTEL_PCR_ID_OPIPHY     0xAC
#define INTEL_PCR_ID_MODPHY0    0xAB
#define INTEL_PCR_ID_MODPHY1    0xAA
#define INTEL_PCR_ID_USB2       0xCA
#define INTEL_PCR_ID_DMI        0x88
#define INTEL_PCR_ID_PSTH       0x89
#define INTEL_PCR_ID_DSP        0xD7
#define INTEL_PCR_ID_ESPISPI    0x72
#define INTEL_PCR_ID_FIA        0xCF
#define INTEL_PCR_ID_SPF        0x85
#define INTEL_PCR_ID_SPE        0x84
#define INTEL_PCR_ID_SPD        0x83
#define INTEL_PCR_ID_SPC        0x82
#define INTEL_PCR_ID_SPB        0x81
#define INTEL_PCR_ID_SPA        0x80
#define INTEL_PCR_ID_SERIALIO   0xCB
#define INTEL_PCR_ID_LPC        0xC7
#define INTEL_PCR_ID_SMB        0xC6
#define INTEL_PCR_ID_ITSS       0xC4
#define INTEL_PCR_ID_RTC_HOST   0xC3
#define INTEL_PCR_ID_PSF6       0x7F
#define INTEL_PCR_ID_PSF7       0x7E
#define INTEL_PCR_ID_PSF8       0x7D
#define INTEL_PCR_ID_PSF4       0xBD
#define INTEL_PCR_ID_PSF3       0xBC
#define INTEL_PCR_ID_PSF2       0xBB
#define INTEL_PCR_ID_PSF1       0xBA
#define INTEL_PCR_ID_GPIOCOM0   0x6E
#define INTEL_PCR_ID_GPIOCOM1   0x6D
#define INTEL_PCR_ID_GPIOCOM2   0x6C
#define INTEL_PCR_ID_GPIOCOM3   0x6B
#define INTEL_PCR_ID_GPIOCOM4   0x6A
#define INTEL_PCR_ID_CSME12     0x9C
#define INTEL_PCR_ID_CSME0      0x90
#define INTEL_PCR_ID_CSME_PSF   0x8F

///////////////////////////////////////
///////////////////////////////////////

#define GPIO_FAMBAR_OFFSET 0x08
#define GPIO_PADBAR_OFFSET 0x0C

/// GPIO Community 4

// Number of GPIOs in the PADs
#define GPIO_GPP_S_COUNT 8
#define GPIO_GPP_E_COUNT 22
#define GPIO_GPP_K_COUNT 12
#define GPIO_GPP_F_COUNT 24




#define GPIO_PAD_OWN_GPP_S_0 0xA0
#define GPIO_PAD_OWN_GPP_E_0 0xA4
#define GPIO_PAD_OWN_GPP_E_1 0xA8
#define GPIO_PAD_OWN_GPP_E_2 0xAC
#define GPIO_PAD_OWN_GPP_K_0 0xB0
#define GPIO_PAD_OWN_GPP_K_1 0xB4
#define GPIO_PAD_OWN_GPP_F_0 0xB8
#define GPIO_PAD_OWN_GPP_F_1 0xBC
#define GPIO_PAD_OWN_GPP_F_2 0xC0

#define GPIO_PADCFGLOCK_GPP_S_0   0x110
#define GPIO_PADCFGLOCKTX_GPP_S_0 0x114
#define GPIO_PADCFGLOCK_GPP_E_0   0x118
#define GPIO_PADCFGLOCKTX_GPP_E_0 0x11C
#define GPIO_PADCFGLOCK_GPP_K_0   0x120
#define GPIO_PADCFGLOCKTX_GPP_K_0 0x124
#define GPIO_PADCFGLOCK_GPP_F_0   0x128
#define GPIO_PADCFGLOCKTX_GPP_F_0 0x12C

#define GPIO_HOSTSW_OWN_GPP_S_0 0x150
#define GPIO_HOSTSW_OWN_GPP_E_0 0x154
#define GPIO_HOSTSW_OWN_GPP_K_0 0x158
#define GPIO_HOSTSW_OWN_GPP_F_0 0x15C

#define GPIO_GPI_IS_GPP_S_0 0x200
#define GPIO_GPI_IS_GPP_E_0 0x204
#define GPIO_GPI_IS_GPP_K_0 0x208
#define GPIO_GPI_IS_GPP_F_0 0x20C

#define GPIO_GPI_IE_GPP_S_0 0x220
#define GPIO_GPI_IE_GPP_E_0 0x224
#define GPIO_GPI_IE_GPP_K_0 0x228
#define GPIO_GPI_IE_GPP_F_0 0x22C

#define GPIO_GPI_GPE_STS_GPP_S_0 0x240
#define GPIO_GPI_GPE_STS_GPP_E_0 0x244
#define GPIO_GPI_GPE_STS_GPP_K_0 0x248
#define GPIO_GPI_GPE_STS_GPP_F_0 0x24C

#define GPIO_GPI_GPE_EN_GPP_S_0 0x260
#define GPIO_GPI_GPE_EN_GPP_E_0 0x264
#define GPIO_GPI_GPE_EN_GPP_K_0 0x268
#define GPIO_GPI_GPE_EN_GPP_F_0 0x26C

#define GPIO_GPI_SMI_STS_GPP_E_0 0x284
#define GPIO_GPI_SMI_EN_GPP_E_0 0x2A4
#define GPIO_GPI_NMI_STS_GPP_E_0 0x2C4
#define GPIO_GPI_NMI_EN_GPP_E_0 0x2E4

#define GPIO_PWMC 0x304


#define GPIO_PAD_CFG_DW0_GPP_S_0 0x700
#define GPIO_PAD_CFG_DW1_GPP_S_0 0x704
#define GPIO_PAD_CFG_DW0_GPP_S_1 0x710
#define GPIO_PAD_CFG_DW1_GPP_S_1 0x714
#define GPIO_PAD_CFG_DW0_GPP_S_2 0x720
#define GPIO_PAD_CFG_DW1_GPP_S_2 0x724
#define GPIO_PAD_CFG_DW0_GPP_S_3 0x730
#define GPIO_PAD_CFG_DW1_GPP_S_3 0x734
#define GPIO_PAD_CFG_DW0_GPP_S_4 0x740
#define GPIO_PAD_CFG_DW1_GPP_S_4 0x744
#define GPIO_PAD_CFG_DW0_GPP_S_5 0x750
#define GPIO_PAD_CFG_DW1_GPP_S_5 0x754
#define GPIO_PAD_CFG_DW0_GPP_S_6 0x760
#define GPIO_PAD_CFG_DW1_GPP_S_6 0x764
#define GPIO_PAD_CFG_DW0_GPP_S_7 0x770
#define GPIO_PAD_CFG_DW1_GPP_S_7 0x774

#define GPIO_PAD_CFG_DW0_GPP_E_0 0x780
#define GPIO_PAD_CFG_DW1_GPP_E_0 0x784
#define GPIO_PAD_CFG_DW0_GPP_E_1 0x790
#define GPIO_PAD_CFG_DW1_GPP_E_1 0x794
#define GPIO_PAD_CFG_DW0_GPP_E_2 0x7A0
#define GPIO_PAD_CFG_DW1_GPP_E_2 0x7A4
#define GPIO_PAD_CFG_DW0_GPP_E_3 0x7B0
#define GPIO_PAD_CFG_DW1_GPP_E_3 0x7B4
#define GPIO_PAD_CFG_DW0_GPP_E_4 0x7C0
#define GPIO_PAD_CFG_DW1_GPP_E_4 0x7C4
#define GPIO_PAD_CFG_DW0_GPP_E_5 0x7D0
#define GPIO_PAD_CFG_DW1_GPP_E_5 0x7D4
#define GPIO_PAD_CFG_DW0_GPP_E_6 0x7E0
#define GPIO_PAD_CFG_DW1_GPP_E_6 0x7E4
#define GPIO_PAD_CFG_DW0_GPP_E_7 0x7F0
#define GPIO_PAD_CFG_DW1_GPP_E_7 0x7F4
#define GPIO_PAD_CFG_DW0_GPP_E_8 0x800
#define GPIO_PAD_CFG_DW1_GPP_E_8 0x804
#define GPIO_PAD_CFG_DW0_GPP_E_9 0x810
#define GPIO_PAD_CFG_DW1_GPP_E_9 0x814
#define GPIO_PAD_CFG_DW0_GPP_E_10 0x820
#define GPIO_PAD_CFG_DW1_GPP_E_10 0x824
#define GPIO_PAD_CFG_DW0_GPP_E_11 0x830
#define GPIO_PAD_CFG_DW1_GPP_E_11 0x834
#define GPIO_PAD_CFG_DW0_GPP_E_12 0x840
#define GPIO_PAD_CFG_DW1_GPP_E_12 0x844
#define GPIO_PAD_CFG_DW0_GPP_E_13 0x850
#define GPIO_PAD_CFG_DW1_GPP_E_13 0x854
#define GPIO_PAD_CFG_DW0_GPP_E_14 0x860
#define GPIO_PAD_CFG_DW1_GPP_E_14 0x864
#define GPIO_PAD_CFG_DW0_GPP_E_15 0x870
#define GPIO_PAD_CFG_DW1_GPP_E_15 0x874
#define GPIO_PAD_CFG_DW0_GPP_E_16 0x880
#define GPIO_PAD_CFG_DW1_GPP_E_16 0x884
#define GPIO_PAD_CFG_DW0_GPP_E_17 0x890
#define GPIO_PAD_CFG_DW1_GPP_E_17 0x894
#define GPIO_PAD_CFG_DW0_GPP_E_18 0x8A0
#define GPIO_PAD_CFG_DW1_GPP_E_18 0x8A4
#define GPIO_PAD_CFG_DW0_GPP_E_19 0x8B0
#define GPIO_PAD_CFG_DW1_GPP_E_19 0x8B4
#define GPIO_PAD_CFG_DW0_GPP_E_20 0x8C0
#define GPIO_PAD_CFG_DW1_GPP_E_20 0x8C4
#define GPIO_PAD_CFG_DW0_GPP_E_21 0x8D0
#define GPIO_PAD_CFG_DW1_GPP_E_21 0x8D4

#define GPIO_PAD_CFG_DW0_GPP_K_0 0x8F0
#define GPIO_PAD_CFG_DW1_GPP_K_0 0x8F4
#define GPIO_PAD_CFG_DW0_GPP_K_1 0x900
#define GPIO_PAD_CFG_DW1_GPP_K_1 0x904
#define GPIO_PAD_CFG_DW0_GPP_K_2 0x910
#define GPIO_PAD_CFG_DW1_GPP_K_2 0x914
#define GPIO_PAD_CFG_DW0_GPP_K_3 0x920
#define GPIO_PAD_CFG_DW1_GPP_K_3 0x924
#define GPIO_PAD_CFG_DW0_GPP_K_4 0x930
#define GPIO_PAD_CFG_DW1_GPP_K_4 0x934
#define GPIO_PAD_CFG_DW0_GPP_K_5 0x940
#define GPIO_PAD_CFG_DW1_GPP_K_5 0x944
#define GPIO_PAD_CFG_DW0_GPP_K_6 0x950
#define GPIO_PAD_CFG_DW1_GPP_K_6 0x954
#define GPIO_PAD_CFG_DW0_GPP_K_7 0x960
#define GPIO_PAD_CFG_DW1_GPP_K_7 0x964
#define GPIO_PAD_CFG_DW0_GPP_K_8 0x970
#define GPIO_PAD_CFG_DW1_GPP_K_8 0x974
#define GPIO_PAD_CFG_DW0_GPP_K_9 0x980
#define GPIO_PAD_CFG_DW1_GPP_K_9 0x984
#define GPIO_PAD_CFG_DW0_GPP_K_10 0x990
#define GPIO_PAD_CFG_DW1_GPP_K_10 0x994
#define GPIO_PAD_CFG_DW0_GPP_K_11 0x9a0
#define GPIO_PAD_CFG_DW1_GPP_K_11 0x9a4

#define GPIO_PAD_CFG_DW0_GPP_F_0 0x9E0
#define GPIO_PAD_CFG_DW1_GPP_F_0 0x9E4
#define GPIO_PAD_CFG_DW0_GPP_F_1 0x9F0
#define GPIO_PAD_CFG_DW1_GPP_F_1 0x9F4
#define GPIO_PAD_CFG_DW0_GPP_F_2 0xA00
#define GPIO_PAD_CFG_DW1_GPP_F_2 0xA04
#define GPIO_PAD_CFG_DW0_GPP_F_3 0xA10
#define GPIO_PAD_CFG_DW1_GPP_F_3 0xA14
#define GPIO_PAD_CFG_DW0_GPP_F_4 0xA20
#define GPIO_PAD_CFG_DW1_GPP_F_4 0xA24
#define GPIO_PAD_CFG_DW0_GPP_F_5 0xA30
#define GPIO_PAD_CFG_DW1_GPP_F_5 0xA34
#define GPIO_PAD_CFG_DW0_GPP_F_6 0xA40
#define GPIO_PAD_CFG_DW1_GPP_F_6 0xA44
#define GPIO_PAD_CFG_DW0_GPP_F_7 0xA50
#define GPIO_PAD_CFG_DW1_GPP_F_7 0xA54
#define GPIO_PAD_CFG_DW0_GPP_F_8 0xA60
#define GPIO_PAD_CFG_DW1_GPP_F_8 0xA64
#define GPIO_PAD_CFG_DW0_GPP_F_9 0xA70
#define GPIO_PAD_CFG_DW1_GPP_F_9 0xA74
#define GPIO_PAD_CFG_DW0_GPP_F_10 0xA80
#define GPIO_PAD_CFG_DW1_GPP_F_10 0xA84
#define GPIO_PAD_CFG_DW0_GPP_F_11 0xA90
#define GPIO_PAD_CFG_DW1_GPP_F_11 0xA94
#define GPIO_PAD_CFG_DW0_GPP_F_12 0xAA0
#define GPIO_PAD_CFG_DW1_GPP_F_12 0xAA4
#define GPIO_PAD_CFG_DW0_GPP_F_13 0xAB0
#define GPIO_PAD_CFG_DW1_GPP_F_13 0xAB4
#define GPIO_PAD_CFG_DW0_GPP_F_14 0xAC0
#define GPIO_PAD_CFG_DW1_GPP_F_14 0xAC4
#define GPIO_PAD_CFG_DW0_GPP_F_15 0xAD0
#define GPIO_PAD_CFG_DW1_GPP_F_15 0xAD4
#define GPIO_PAD_CFG_DW0_GPP_F_16 0xAE0
#define GPIO_PAD_CFG_DW1_GPP_F_16 0xAE4
#define GPIO_PAD_CFG_DW0_GPP_F_17 0xAF0
#define GPIO_PAD_CFG_DW1_GPP_F_17 0xAF4
#define GPIO_PAD_CFG_DW0_GPP_F_18 0xB00
#define GPIO_PAD_CFG_DW1_GPP_F_18 0xB04
#define GPIO_PAD_CFG_DW0_GPP_F_19 0xB10
#define GPIO_PAD_CFG_DW1_GPP_F_19 0xB14
#define GPIO_PAD_CFG_DW0_GPP_F_20 0xB20
#define GPIO_PAD_CFG_DW1_GPP_F_20 0xB24
#define GPIO_PAD_CFG_DW0_GPP_F_21 0xB30
#define GPIO_PAD_CFG_DW1_GPP_F_21 0xB34
#define GPIO_PAD_CFG_DW0_GPP_F_22 0xB40
#define GPIO_PAD_CFG_DW1_GPP_F_22 0xB44
#define GPIO_PAD_CFG_DW0_GPP_F_23 0xB50
#define GPIO_PAD_CFG_DW1_GPP_F_23 0xB54

typedef struct _GPIO_PAD_CFG {
    union {
        UINT32 dw0;
        struct {
            UINT32 GPIOTXSTATE: 1;
            UINT32 GPIORXSTATE: 1;
            UINT32 Resvd0: 6;
            UINT32 GPIOTXDIS: 1;
            UINT32 GPIORXDIS: 1;
            UINT32 PMODE: 2;
            UINT32 Resvd1: 5;
            UINT32 GPIROUTNMI: 1;
            UINT32 GPIROUTSMI: 1;
            UINT32 GPIROUTSCI: 1;
            UINT32 GPIROUTIOXAPIC: 1;
            UINT32 RXTXENCFG: 2;
            UINT32 RXINV: 1;
            UINT32 PREGFRXSEL: 1;
            UINT32 RXEVCFG: 2;
            UINT32 Resvd3: 1;
            UINT32 RXRAW1: 1;
            UINT32 RXPADSTSEL: 1;
            UINT32 PADRSTCFG: 2;
        };
    };
    union {
        UINT32 dw1;
        struct {
            UINT32 INTSEL: 8;
            UINT32 IOSTREM: 2;
            UINT32 TERM: 3;
            UINT32 IOSTATE: 4;
            UINT32 Resvd0: 15;
        };
    };
} GPIO_PAD_CFG;

// This struct holds the setting for a GPIO group
// It harcodes the number of GPIOs to 32
// The specific group could have less pins
struct GpioGroupSettings {
   UINT32 PadCfgLockTxState;
   UINT32 PadCfgLock;
   BYTE   PadOwnership[32];
   
   // A bitmap per GPIO that dictates the host status bit update mode
   // When the GPIO is under Host ownership   
   UINT32 HostSW_Own; 
   
   GPIO_PAD_CFG GpioConfig[32];
};

extern GPIO_PAD_CFG GPIO_PAD_CFG_GPP_S[8];
extern GPIO_PAD_CFG GPIO_PAD_CFG_GPP_E[22];
extern GPIO_PAD_CFG GPIO_PAD_CFG_GPP_K[12];
extern GPIO_PAD_CFG GPIO_PAD_CFG_GPP_F[24];


// extern struct GpioGroupSettings gGpioGroupF;

void retrieve_gpio_gpp_f(struct GpioGroupSettings *pad);
void print_pad_cfg_reg(GPIO_PAD_CFG *reg);

void print_gpio_community4_mem();


UINT32 PchSbMmioRead32(UINT32 community, UINT32 regOffset);
void PchSbMmioWrite32(UINT32 community, UINT32 regOffset, UINT32 val);

////////////////////////////////////////////////////



#define GPIO_GET_PAD_NUMBER(Pad)  ((Pad) & 0xFFFF)
#define GPIO_GET_GROUP_FROM_PAD(Pad)  ((Pad) >> 16)
#define GPIO_GET_PAD_POSITION(PadNumber)      ((PadNumber) & 0x1F)
#define GPIO_GET_DW_NUM(PadNumber)            ((PadNumber) >> 5)

#define GPIO_GET_CHIPSET_ID(Pad)              ((Pad) >> 24)

#define GPIO_GET_GROUP_INDEX(Group)           ((Group) & 0xFF)
#define GPIO_GET_GROUP_INDEX_FROM_PAD(Pad)    GPIO_GET_GROUP_INDEX (((Pad) >> 16))

#define GPIO_PADCFG_DW_REG_NUMBER  4

typedef UINT32 GPIO_PAD;
typedef UINT32 GPIO_GROUP;




typedef struct {
  /*0x00*/ UINT16  Community;
  /*0x02*/ UINT16       PadOwnOffset;
  /*0x04*/ UINT16       HostOwnOffset;
  /*0x06*/ UINT16       GpiIsOffset;
  /*0x08*/ UINT16       GpiIeOffset;
  /*0x0A*/ UINT16       GpiGpeStsOffset;
  /*0x0C*/ UINT16       GpiGpeEnOffset;
  /*0x0E*/ UINT16       SmiStsOffset;
  /*0x10*/ UINT16       SmiEnOffset;
  /*0x12*/ UINT16       NmiStsOffset;
  /*0x14*/ UINT16       NmiEnOffset;
  /*0x16*/ UINT16       PadCfgLockOffset;
  /*0x18*/ UINT16       PadCfgLockTxOffset;
  /*0x1A*/ UINT16       PadCfgOffset;
  /*0x1E*/ UINT16       PadPerGroup;
} GPIO_GROUP_INFO;


typedef enum {
  GpioDirDefault    = 0x0,                ///< Leave pad direction setting unmodified
  GpioDirInOut      = (0x1 | (0x1 << 3)), ///< Set pad for both output and input
  GpioDirInInvOut   = (0x1 | (0x3 << 3)), ///< Set pad for both output and input with inversion
  GpioDirIn         = (0x3 | (0x1 << 3)), ///< Set pad for input only
  GpioDirInInv      = (0x3 | (0x3 << 3)), ///< Set pad for input with inversion
  GpioDirOut        = 0x5,                ///< Set pad for output only
  GpioDirNone       = 0x7                 ///< Disable both output and input
} GPIO_DIRECTION;

/**
  GPIO configuration structure used for pin programming.
  Structure contains fields that can be used to configure pad.
**/
typedef struct {
  /**
  Pad Mode
  Pad can be set as GPIO or one of its native functions.
  When in native mode setting Direction (except Inversion), OutputState,
  InterruptConfig, Host Software Pad Ownership and OutputStateLock are unnecessary.
  Refer to definition of GPIO_PAD_MODE.
  Refer to EDS for each native mode according to the pad.
  **/
  UINT32 PadMode            : 5;
  /**
  Host Software Pad Ownership
  Set pad to ACPI mode or GPIO Driver Mode.
  Refer to definition of GPIO_HOSTSW_OWN.
  **/
  UINT32 HostSoftPadOwn     : 2;
  /**
  GPIO Direction
  Can choose between In, In with inversion, Out, both In and Out, both In with inversion and out or disabling both.
  Refer to definition of GPIO_DIRECTION for supported settings.
  **/
  UINT32 Direction           : 6;
  /**
  Output State
  Set Pad output value.
  Refer to definition of GPIO_OUTPUT_STATE for supported settings.
  This setting takes place when output is enabled.
  **/
  UINT32 OutputState         : 2;
  /**
  GPIO Interrupt Configuration
  Set Pad to cause one of interrupts (IOxAPIC/SCI/SMI/NMI).
  This setting is applicable only if GPIO is in GpioMode with input enabled.
  Refer to definition of GPIO_INT_CONFIG for supported settings.
  **/
  UINT32 InterruptConfig     : 9;
  /**
  GPIO Power Configuration.
  This setting controls Pad Reset Configuration.
  Refer to definition of GPIO_RESET_CONFIG for supported settings.
  **/
  UINT32 PowerConfig        : 8;
  /**
  GPIO Electrical Configuration
  This setting controls pads termination.
  Refer to definition of GPIO_ELECTRICAL_CONFIG for supported settings.
  **/
  UINT32 ElectricalConfig   : 9;
  /**
  GPIO Lock Configuration
  This setting controls pads lock.
  Refer to definition of GPIO_LOCK_CONFIG for supported settings.
  **/
  UINT32 LockConfig         : 4;
  /**
  Additional GPIO configuration
  Refer to definition of GPIO_OTHER_CONFIG for supported settings.
  **/
  UINT32 OtherSettings     :  9;
  UINT32 RsvdBits          : 10;    ///< Reserved bits for future extension
} GPIO_CONFIG;



///
/// GPIO Power Configuration
/// GPIO_RESET_CONFIG allows to set GPIO Reset (used to reset the specified
/// Pad Register fields).
///
typedef enum {
  GpioResetDefault   = 0x0,           ///< Leave value of pad reset unmodified
  GpioResetPwrGood   = 0x1,           ///< Powergood reset
  GpioResetDeep      = 0x3,           ///< Deep GPIO Reset
  GpioResetNormal    = 0x5,           ///< GPIO Reset
  GpioResetResume    = 0x7            ///< Resume Reset (applicable only for GPD group)
} GPIO_RESET_CONFIG;


typedef enum {
  GpioResetDefaultX   = 0x00,        ///< Leave value of pad reset unmodified
  /**
  Resume Reset (RSMRST)
    GPP: PadRstCfg = 00b = "Powergood"
    GPD: PadRstCfg = 11b = "Resume Reset"
  Pad setting will reset on:
  - DeepSx transition
  - G3
  Pad settings will not reset on:
  - S3/S4/S5 transition
  - Warm/Cold/Global reset
  **/
  GpioResumeReset      = 0x01,
  /**
  Host Deep Reset
    PadRstCfg = 01b = "Deep GPIO Reset"
  Pad settings will reset on:
  - Warm/Cold/Global reset
  - DeepSx transition
  - G3
  Pad settings will not reset on:
  - S3/S4/S5 transition
  **/
  GpioHostDeepReset    = 0x03,
  /**
  Platform Reset (PLTRST)
    PadRstCfg = 10b = "GPIO Reset"
  Pad settings will reset on:
  - S3/S4/S5 transition
  - Warm/Cold/Global reset
  - DeepSx transition
  - G3
  **/
  GpioPlatformReset    = 0x05,
  /**
  Deep Sleep Well Reset (DSW_PWROK)
    GPP: not applicable
    GPD: PadRstCfg = 00b = "Powergood"
  Pad settings will reset on:
  - G3
  Pad settings will not reset on:
  - S3/S4/S5 transition
  - Warm/Cold/Global reset
  - DeepSx transition
  **/
  GpioDswReset         = 0x07
} GPIO_RESET_CONFIG2;


#define NO_REGISTER_FOR_PROPERTY 0xFFFF

//
// Possible registers to be accessed using GpioReadReg()/GpioWriteReg() functions
//
typedef enum {
  GpioHostOwnershipRegister = 0,
  GpioGpeEnableRegister,
  GpioGpeStatusRegister,
  GpioSmiEnableRegister,
  GpioSmiStatusRegister,
  GpioNmiEnableRegister,
  GpioPadConfigLockRegister,
  GpioPadLockOutputRegister
} GPIO_REG;




///
/// GPIO Pad Mode
///
typedef enum {
  GpioPadModeGpio     = 0x1,
  GpioPadModeNative1  = 0x3,
  GpioPadModeNative2  = 0x5,
  GpioPadModeNative3  = 0x7,
  GpioPadModeNative4  = 0x9
} GPIO_PAD_MODE;

//
// Structure for native pin data
//
typedef struct {
  GPIO_PAD       Pad;
  GPIO_PAD_MODE  Mode;
} GPIO_PAD_NATIVE_FUNCTION;

//
// Below defines are based on GPIO_CONFIG structure fields
//
#define B_GPIO_PAD_MODE_MASK                            0xF
#define N_GPIO_PAD_MODE_BIT_POS                         0
#define B_GPIO_HOSTSW_OWN_MASK                          0x3
#define N_GPIO_HOSTSW_OWN_BIT_POS                       0
#define B_GPIO_DIRECTION_MASK                           0x1F
#define B_GPIO_DIRECTION_DIR_MASK                       0x7
#define N_GPIO_DIRECTION_DIR_BIT_POS                    0
#define B_GPIO_DIRECTION_INV_MASK                       0x18
#define N_GPIO_DIRECTION_INV_BIT_POS                    3
#define B_GPIO_OUTPUT_MASK                              0x3
#define N_GPIO_OUTPUT_BIT_POS                           0
#define N_GPIO_INT_CONFIG_INT_SOURCE_BIT_POS            0
#define N_GPIO_INT_CONFIG_INT_TYPE_BIT_POS              5
#define B_GPIO_RESET_CONFIG_RESET_MASK                  0x3F
#define N_GPIO_RESET_CONFIG_OLD_RESET_TYPE              BIT1
#define B_GPIO_RESET_CONFIG_OLD_RESET_MASK              0xF
#define N_GPIO_RESET_CONFIG_RESET_BIT_POS               0
#define B_GPIO_RESET_CONFIG_GPD_RESET_MASK              (BIT5 | BIT4)
#define B_GPIO_RESET_CONFIG_GPP_RESET_MASK              (BIT3 | BIT2)
#define N_GPIO_ELECTRICAL_CONFIG_TERMINATION_BIT_POS    0
#define N_GPIO_OTHER_CONFIG_RXRAW_BIT_POS               0



bool GpioGetPadConfig(GPIO_PAD GpioPad, GPIO_CONFIG  *GpioData);
UINT32 GpioReadPadCfgReg (GPIO_PAD GpioPad, UINT8 DwReg);
void GpioWritePadCfgReg (GPIO_PAD GpioPad, UINT8 DwReg, UINT32 val);

void GpioWrite(GPIO_PAD GpioPad, UINT8 val);

////////////////////////////////////////

#define REG_PCH_GPIO_PAD_OWN      0x20
#define REG_PCH_GPIO_HOSTSW_OWN   0xd0
#define REG_PCH_GPIO_GPI_IS       0x100
#define REG_PCH_GPIO_GPI_IE       0x120
#define REG_PCH_GPIO_GPE_STS      0x140
#define REG_PCH_GPIO_GPE_EN       0x160
#define REG_PCH_GPIO_SMI_STS      0x184
#define REG_PCH_GPIO_SMI_EN       0x1a4
#define REG_PCH_GPIO_NMI_STS      0x1c4
#define REG_PCH_GPIO_NMI_EN       0x1e4

#define REG_PCH_GPIO_DW0_PMODE    0x1600
#define REG_PCH_GPIO_DW0_RXDIS    0x0200
#define REG_PCH_GPIO_DW0_TXDIS    0x0100
#define REG_PCH_GPIO_DW0_RXSTATE  0x0002
#define REG_PCH_GPIO_DW0_TXSTATE  0x0001

#define REG_PCH_GPIO_DW1_TERM_NONE    0x0
#define REG_PCH_GPIO_DW1_TERM_5K_PD   0x2
#define REG_PCH_GPIO_DW1_TERM_20K_PD  0x4
#define REG_PCH_GPIO_DW1_TERM_5K_PU   0xa
#define REG_PCH_GPIO_DW1_TERM_20K_PU  0xc
#define REG_PCH_GPIO_DW1_TERM_NATIVE  0xf