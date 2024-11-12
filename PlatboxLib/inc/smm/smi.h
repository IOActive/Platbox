#include "global.h"


int trigger_smi(SW_SMI_CALL *smi_call);

int trigger_smi_ex(SW_SMI_CALL *smi_call);

UINT16 get_smi_port();

void print_smm_save_state(const struct SMM_SAVE_STATE* state);

void diff_smm_save_state(SMM_SAVE_STATE* ss_old, SMM_SAVE_STATE* ss_new);