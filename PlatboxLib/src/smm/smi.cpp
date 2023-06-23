#include "common_chipset.h"
#include "smi.h"

UINT16 g_SMI_TRIGGER_PORT = 0x00;

UINT16 get_smi_port() {
    if (g_SMI_TRIGGER_PORT == 0) {
        g_SMI_TRIGGER_PORT = get_smi_trigger_port();
    }
    return g_SMI_TRIGGER_PORT;
}


int trigger_smi(SW_SMI_CALL *smi_call) {
    smi_call->TriggerPort = get_smi_port();
    return trigger_smi_ex(smi_call);
}

//
int trigger_smi_ex(SW_SMI_CALL *smi_call) {
    int status;
    #ifdef __linux__
        status = ioctl(g_hDevice, IOCTL_ISSUE_SW_SMI, &smi_call);           
    #else //_WIN32        
        DWORD bytesReturned = 0;
        status = DeviceIoControl(g_hDevice, IOCTL_ISSUE_SW_SMI, smi_call, sizeof(SW_SMI_CALL), NULL, 0, &bytesReturned, NULL);
    #endif    

    return status;  
}