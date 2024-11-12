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
        status = ioctl(g_hDevice, IOCTL_ISSUE_SW_SMI, smi_call);           
    #else //_WIN32        
        DWORD bytesReturned = 0;
        status = DeviceIoControl(g_hDevice, IOCTL_ISSUE_SW_SMI, smi_call, sizeof(SW_SMI_CALL), smi_call, sizeof(SW_SMI_CALL), &bytesReturned, NULL);
    #endif    

    return status;  
}


void print_smm_save_state(const struct SMM_SAVE_STATE* state) {
    printf("es.selector: %04x\n", state->es.selector);
    printf("es.descriptor: %016llx\n", state->es.descriptor);

    printf("cs.selector: %04x\n", state->cs.selector);
    printf("cs.descriptor: %016llx\n", state->cs.descriptor);

    printf("ss.selector: %04x\n", state->ss.selector);
    printf("ss.descriptor: %016llx\n", state->ss.descriptor);

    printf("ds.selector: %04x\n", state->ds.selector);
    printf("ds.descriptor: %016llx\n", state->ds.descriptor);

    printf("fs.selector: %04x\n", state->fs.selector);
    printf("fs.descriptor: %016llx\n", state->fs.descriptor);

    printf("gs.selector: %04x\n", state->gs.selector);
    printf("gs.descriptor: %016llx\n", state->gs.descriptor);

    printf("io_restart_rip: %016llx\n", state->io_restart_rip);
    printf("io_restart_rcx: %016llx\n", state->io_restart_rcx);
    printf("io_restart_rsi: %016llx\n", state->io_restart_rsi);
    printf("io_restart_rdi: %016llx\n", state->io_restart_rdi);
    printf("smm_io_trap_offset: %08x\n", state->smm_io_trap_offset);
    printf("local_smi_status: %08x\n", state->local_smi_status);
    printf("smm_io_restart_byte: %02x\n", state->smm_io_restart_byte);
    printf("auto_halt_restart_offset: %02x\n", state->auto_halt_restart_offset);
    printf("nmi_mask: %02x\n", state->nmi_mask);
    printf("efer: %016llx\n", state->efer);
    printf("smm_svm_state: %016llx\n", state->smm_svm_state);
    printf("guest_vmcb_physical_address: %016llx\n", state->guest_vmcb_physical_address);
    printf("svm_virtual_interrupt_control: %016llx\n", state->svm_virtual_interrupt_control);
    printf("smm_revision_identifier: %08x\n", state->smm_revision_identifier);
    printf("smm_base: %08x\n", state->smm_base);
    printf("guest_pat: %016llx\n", state->guest_pat);
    printf("host_efer: %016llx\n", state->host_efer);
    printf("host_cr4: %016llx\n", state->host_cr4);
    printf("nested_cr3: %016llx\n", state->nested_cr3);
    printf("host_cr0: %016llx\n", state->host_cr0);
    printf("cr4: %016llx\n", state->cr4);
    printf("cr3: %016llx\n", state->cr3);
    printf("cr0: %016llx\n", state->cr0);
    printf("dr7: %016llx\n", state->dr7);
    printf("dr6: %016llx\n", state->dr6);
    printf("rflags: %016llx\n", state->rflags);
    printf("rip: %016llx\n", state->rip);
    printf("r15: %016llx\n", state->r15);
    printf("r14: %016llx\n", state->r14);
    printf("r13: %016llx\n", state->r13);
    printf("r12: %016llx\n", state->r12);
    printf("r11: %016llx\n", state->r11);
    printf("r10: %016llx\n", state->r10);
    printf("r9: %016llx\n", state->r9);
    printf("r8: %016llx\n", state->r8);
    printf("rdi: %016llx\n", state->rdi);
    printf("rsi: %016llx\n", state->rsi);
    printf("rbp: %016llx\n", state->rbp);
    printf("rsp: %016llx\n", state->rsp);
    printf("rbx: %016llx\n", state->rbx);
    printf("rdx: %016llx\n", state->rdx);
    printf("rcx: %016llx\n", state->rcx);
    printf("rax: %016llx\n", state->rax);
}


void diff_smm_save_state(SMM_SAVE_STATE* ss_old, SMM_SAVE_STATE* ss_new)
{
    // Check descriptors
    if (ss_old->es.selector != ss_new->es.selector)
    {
        printf("es.selector has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->es.selector, ss_new->es.selector);
    }

    if (ss_old->es.descriptor != ss_new->es.descriptor)
    {
        printf("es.descriptor has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->es.descriptor, ss_new->es.descriptor);
    }

    if (ss_old->cs.selector != ss_new->cs.selector)
    {
        printf("cs.selector has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->cs.selector, ss_new->cs.selector);
    }

    if (ss_old->cs.descriptor != ss_new->cs.descriptor)
    {
        printf("cs.descriptor has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->cs.descriptor, ss_new->cs.descriptor);
    }

    if (ss_old->ss.selector != ss_new->ss.selector)
    {
        printf("ss.selector has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->ss.selector, ss_new->ss.selector);
    }

    if (ss_old->ss.descriptor != ss_new->ss.descriptor)
    {
        printf("ss.selector has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->ss.descriptor, ss_new->ss.descriptor);
    }

    if (ss_old->ds.selector != ss_new->ds.selector)
    {
        printf("ds.selector has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->ds.selector, ss_new->ds.selector);
    }

    if (ss_old->ds.descriptor != ss_new->ds.descriptor)
    {
        printf("ds.descriptor has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->ds.descriptor, ss_new->ds.descriptor);
    }

    if (ss_old->fs.selector != ss_new->fs.selector)
    {
        printf("fs.selector has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->fs.selector, ss_new->fs.selector);
    }

    if (ss_old->fs.descriptor != ss_new->fs.descriptor)
    {
        printf("fs.descriptor has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->fs.descriptor, ss_new->fs.descriptor);
    }

    if (ss_old->gs.selector != ss_new->gs.selector)
    {
        printf("gs.selector has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->gs.selector, ss_new->gs.selector);
    }

    if (ss_old->gs.descriptor != ss_new->gs.descriptor)
    {
        printf("gs.descriptor has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->gs.descriptor, ss_new->gs.descriptor);
    }

    //

    if (ss_old->io_restart_rip != ss_new->io_restart_rip)
    {
        printf("io_restart_rip has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->io_restart_rip, ss_new->io_restart_rip);
    }

    if (ss_old->io_restart_rcx != ss_new->io_restart_rcx)
    {
        printf("io_restart_rcx has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->io_restart_rcx, ss_new->io_restart_rcx);
    }

    if (ss_old->io_restart_rsi != ss_new->io_restart_rsi)
    {
        printf("io_restart_rsi has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->io_restart_rsi, ss_new->io_restart_rsi);
    }

    if (ss_old->io_restart_rdi != ss_new->io_restart_rdi)
    {
        printf("io_restart_rdi has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->io_restart_rdi, ss_new->io_restart_rdi);
    }

    if (ss_old->smm_io_trap_offset != ss_new->smm_io_trap_offset)
    {
        printf("smm_io_trap_offset has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->smm_io_trap_offset, ss_new->smm_io_trap_offset);
    }

    if (ss_old->local_smi_status != ss_new->local_smi_status)
    {
        printf("local_smi_status has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->local_smi_status, ss_new->local_smi_status);
    }

    if (ss_old->smm_io_restart_byte != ss_new->smm_io_restart_byte)
    {
        printf("smm_io_restart_byte has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->smm_io_restart_byte, ss_new->smm_io_restart_byte);
    }

    if (ss_old->auto_halt_restart_offset != ss_new->auto_halt_restart_offset)
    {
        printf("auto_halt_restart_offset has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->auto_halt_restart_offset, ss_new->auto_halt_restart_offset);
    }

    if (ss_old->nmi_mask != ss_new->nmi_mask)
    {
        printf("nmi_mask has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->nmi_mask, ss_new->nmi_mask);
    }

    if (ss_old->efer != ss_new->efer)
    {
        printf("efer has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->efer, ss_new->efer);
    }

    if (ss_old->smm_svm_state != ss_new->smm_svm_state)
    {
        printf("smm_svm_state has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->smm_svm_state, ss_new->smm_svm_state);
    }

    if (ss_old->guest_vmcb_physical_address != ss_new->guest_vmcb_physical_address)
    {
        printf("guest_vmcb_physical_address has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->guest_vmcb_physical_address, ss_new->guest_vmcb_physical_address);
    }

    if (ss_old->svm_virtual_interrupt_control != ss_new->svm_virtual_interrupt_control)
    {
        printf("svm_virtual_interrupt_control has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->svm_virtual_interrupt_control, ss_new->svm_virtual_interrupt_control);
    }

    if (ss_old->smm_revision_identifier != ss_new->smm_revision_identifier)
    {
        printf("smm_revision_identifier has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->smm_revision_identifier, ss_new->smm_revision_identifier);
    }

    if (ss_old->smm_base != ss_new->smm_base)
    {
        printf("smm_base has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->smm_base, ss_new->smm_base);
    }

    if (ss_old->guest_pat != ss_new->guest_pat)
    {
        printf("guest_pat has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->guest_pat, ss_new->guest_pat);
    }

    if (ss_old->host_efer != ss_new->host_efer)
    {
        printf("host_efer has changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->host_efer, ss_new->host_efer);
    }

    if (ss_old->host_cr4 != ss_new->host_cr4)
    {
        printf("host_cr4 changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->host_cr4, ss_new->host_cr4);
    }

    if (ss_old->nested_cr3 != ss_new->nested_cr3)
    {
        printf("nested_cr3 changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->nested_cr3, ss_new->nested_cr3);
    }

    if (ss_old->host_cr0 != ss_new->host_cr0)
    {
        printf("host_cr0 changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->host_cr0, ss_new->host_cr0);

    }

    if (ss_old->cr4 != ss_new->cr4)
    {
        printf("cr4 changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->cr4, ss_new->cr4);
    }

    if (ss_old->cr3 != ss_new->cr3)
    {
        printf("cr3 changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->cr3, ss_new->cr3);
    }

    if (ss_old->cr0 != ss_new->cr0)
    {
        printf("cr0 changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->cr0, ss_new->cr0);
    }

    if (ss_old->dr7 != ss_new->dr7)
    {
        printf("dr7 changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->dr7, ss_new->dr7);
    }

    if (ss_old->dr6 != ss_new->dr6)
    {
        printf("dr6 changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->dr6, ss_new->dr6);
    }

    if (ss_old->rflags != ss_new->rflags)
    {
        printf("rflags changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->rflags, ss_new->rflags);
    }

    if (ss_old->rip != ss_new->rip)
    {
        printf("rip changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->rip, ss_new->rip);
    }
    if (ss_old->r15 != ss_new->r15)
    {
        printf("r15 changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->r15, ss_new->r15);
    }

    if (ss_old->r14 != ss_new->r14)
    {
        printf("r14 changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->r14, ss_new->r14);
    }

    if (ss_old->r13 != ss_new->r13)
    {
        printf("r13 changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->r13, ss_new->r13);
    }

    if (ss_old->r12 != ss_new->r12)
    {
        printf("r12 changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->r12, ss_new->r12);
    }

    if (ss_old->r11 != ss_new->r11)
    {
        printf("r11 changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->r11, ss_new->r11);
    }

    if (ss_old->r10 != ss_new->r10)
    {
        printf("r10 changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->r10, ss_new->r10);
    }

    if (ss_old->r9 != ss_new->r9)
    {
        printf("r9 changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->r9, ss_new->r9);
    }

    if (ss_old->r8 != ss_new->r8)
    {
        printf("r8 changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->r8, ss_new->r8);
    }

    if (ss_old->rdi != ss_new->rdi)
    {
        printf("rdi changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->rdi, ss_new->rdi);
    }

    if (ss_old->rsi != ss_new->rsi)
    {
        printf("rsi changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->rsi, ss_new->rsi);
    }

    if (ss_old->rbp != ss_new->rbp)
    {
        printf("rbp changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->rbp, ss_new->rbp);
    }

    if (ss_old->rsp != ss_new->rsp)
    {
        printf("rsp changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->rsp, ss_new->rsp);
    }

    if (ss_old->rbx != ss_new->rbx)
    {
        printf("rbx changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->rbx, ss_new->rbx);
    }

    if (ss_old->rdx != ss_new->rdx)
    {
        printf("rdx changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->rdx, ss_new->rdx);
    }

    if (ss_old->rcx != ss_new->rcx)
    {
        printf("rcx changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->rcx, ss_new->rcx);
    }

    if (ss_old->rax != ss_new->rax)
    {
        printf("rax changed\n");
        printf("old: %016llx, new: %016llx\n", ss_old->rax, ss_new->rax);
    }
}