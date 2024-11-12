PUBLIC _ret
PUBLIC _haltcore
PUBLIC _disable_interrupts
PUBLIC _disable_interrupts
PUBLIC _store_savestate
PUBLIC _swsmi
PUBLIC _read_pci_compatible_configuration_space
PUBLIC _read_pci_byte
PUBLIC _read_pci_word
PUBLIC _read_pci_dword
PUBLIC _read_pci_bar_size
PUBLIC _write_pci_byte
PUBLIC _write_pci_word
PUBLIC _write_pci_dword
PUBLIC _rdmsr
PUBLIC _wrmsr


.code

; void _ret();
_ret PROC PUBLIC
    ret
_ret ENDP

; void _haltcore();
_haltcore PROC PUBLIC
    hlt
_haltcore ENDP

; void disable_interrupts();
_disable_interrupts PROC PUBLIC
    cli
_disable_interrupts ENDP

; void _enable_interrupts();
_enable_interrupts PROC PUBLIC
    sti
_enable_interrupts ENDP

; void _store_savestate(PSMM_SAVE_STATE SmmSaveState);
_store_savestate:
; 0x3200 -> RIP
; 0x3208 -> RSP
; 0x3210 -> RBP
; 0x3218 -> CR3

	; Save CR3
    push rax
    mov rax, cr3
    mov [rcx + 18h], rax
    
    ; Save RIP
    mov rax, _exit_store_savestate
    mov [rcx], rax
    pop rax
    
    ; Save RBP and RSP
    mov [rcx + 10h], rbp
    mov [rcx + 08h], rsp
_exit_store_savestate:
    ret


; void _swsmi(PSW_SMI_CALL regs);
_swsmi	PROC PUBLIC

; store registers on stack
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push  r9
    push  r8
    push rdi
    push rsi
    push rdx
    push rbx
    push rcx
 

    ; set registers to state of provided structure
    mov r15, [rcx + 080h]
    mov r14, [rcx + 078h]
    mov r13, [rcx + 070h]
    mov r12, [rcx + 068h]
    mov r11, [rcx + 060h]
    mov r10, [rcx + 058h]
    mov  r9, [rcx + 050h]
    mov  r8, [rcx + 048h]
    mov rdi, [rcx + 040h]
    mov rsi, [rcx + 038h]
    mov rdx, [rcx + 030h]
    ; rcx handled later
    mov rbx, [rcx + 020h]

    ; The trigger port might be different than 0xB2 in AMD
    mov dx, word ptr [rcx + 010h] 

    mov rax,  [rcx + 08h] 
    shl rax, 8
    or rax,  [rcx] 

    mov rcx, [rcx + 028h] ; get rcx value just before the OUT instruction
    
    ; this OUT instruction will write WORD value (smi_code_data) to ports 0xB2 and 0xB3 (SW SMI control and data ports)
    
    push rsi
    pushfq

    wbinvd
    out dx, ax

    popfq
    pop rsi

    ;; write to structure the changes that could have happened in SMM

    push rax; save return RAX from SMI
    mov rax, [rsp + 08h]; rax points to original RCX

    mov [rax + 080h], r15
    mov [rax + 078h], r14
    mov [rax + 070h], r13
    mov [rax + 068h], r12
    mov [rax + 060h], r11
    mov [rax + 058h], r10
    mov [rax + 050h],  r9
    mov [rax + 048h],  r8
    mov [rax + 040h], rdi
    mov [rax + 038h], rsi
    mov [rax + 030h], rdx
    mov [rax + 028h], rcx
    mov [rax + 020h], rbx

    pop r15 ; retrieve original rax
    mov [rax + 018h], r15

    pop rcx
    pop rbx
    pop rdx
    pop rsi
    pop rdi
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15

    xor rax, rax

    ret

_swsmi	ENDP

; _read_pci_compatible_configuration_space(UINT8 Bus, UINT8 Device, UINT8 Function, PVOID pOut)
; Reads the 256 bytes of PCI Configuration data from a BDF into pOut
_read_pci_compatible_configuration_space	PROC PUBLIC
    push rbx
    push rdi
    cli
    
    xor rax, rax
    mov al, 1
    shl eax, 1Fh
    shl ecx, 10h
    shl edx, 0Bh
    shl r8d, 08h
    or eax, ecx
    or eax, edx
    or eax, r8d
    mov rbx, rax
    mov rdi, r9
    xor rcx, rcx
    not rcx	
_loop:
    inc rcx
    mov rax, rcx
    shl eax, 02h
    or eax, ebx
        
    mov dx, 0CF8h	
    out dx, eax
    xor rax, rax
    mov dx, 0CFCh
    in eax, dx
    
    mov dword ptr [rdi+rcx*4], eax
    
    cmp cl, 40h
    jnz _loop
    
    sti
    pop rdi
    pop rbx
    ret

_read_pci_compatible_configuration_space	ENDP

; void _read_pci_byte( UINT32 CF8, PVOID pOut);
_read_pci_byte	PROC PUBLIC
    push rdx
    cli
    xor rdx, rdx
    mov dx, 0CF8h
    mov rax, rcx
    out dx, eax
    xor rax, rax
    mov dx, 0CFCh
    in al, dx
    pop rdx
    mov byte ptr[rdx], al
    sti
    ret
_read_pci_byte	ENDP

; void _read_pci_word( UINT32 CF8, PVOID pOut);
_read_pci_word	PROC PUBLIC
    push rdx
    cli
    xor rdx, rdx
    mov dx, 0CF8h
    mov rax, rcx
    out dx, eax
    xor rax, rax
    mov dx, 0CFCh
    in ax, dx
    pop rdx
    mov word ptr[rdx], ax
    sti
    ret
_read_pci_word	ENDP

; void _read_pci_dword( UINT32 CF8, PVOID pOut);
_read_pci_dword	PROC PUBLIC
    push rdx
    cli
    xor rdx, rdx
    mov dx, 0CF8h
    mov rax, rcx
    out dx, eax
    xor rax, rax
    mov dx, 0CFCh
    in eax, dx
    pop rdx
    mov dword ptr[rdx], eax
    sti
    ret
_read_pci_dword	ENDP

; void _write_pci_byte( UINT32 CF8, UINT32 value);
_write_pci_byte	PROC PUBLIC
    push rdx
    cli
    xor rdx, rdx
    mov dx, 0CF8h
    mov rax, rcx
    out dx, eax
    xor rax, rax
    pop rdx
    mov rax, rdx
    mov dx, 0CFCh	
    out dx, al	
    sti
    ret
_write_pci_byte	ENDP

; void _write_pci_word( UINT32 CF8, UINT32 value);
_write_pci_word	PROC PUBLIC
    push rdx
    cli
    xor rdx, rdx
    mov dx, 0CF8h
    mov rax, rcx
    out dx, eax
    xor rax, rax
    pop rdx
    mov rax, rdx
    mov dx, 0CFCh	
    out dx, ax	
    sti
    ret
_write_pci_word	ENDP

; void _write_pci_dword( UINT32 CF8, UINT32 value);
_write_pci_dword	PROC PUBLIC
    push rdx
    cli
    xor rdx, rdx
    mov dx, 0CF8h
    mov rax, rcx
    out dx, eax
    xor rax, rax
    pop rdx
    mov rax, rdx
    mov dx, 0CFCh	
    out dx, eax	
    sti
    ret
_write_pci_dword	ENDP

; void _read_pci_bar_size( UINT32 CF8, PVOID pOut);
_read_pci_bar_size	PROC PUBLIC
    push rbx
    push rsi
    mov rbx, rdx
    mov rsi, rcx
; read the current BAR value	
    call _read_pci_dword

; writes 0xFFFFFFFF to the BAR
    cli
    xor rdx, rdx
    mov dx, 0CF8h
    mov rax, rsi
    out dx, eax	
    xor rax, rax
    not rax
    mov dx, 0CFCh	
    out dx, eax	

; reads the size
    xor rax, rax
    in eax, dx
    mov rcx, rax

; restore the BAR with saved value
    mov dx, 0CFCh
    mov eax, dword ptr[rbx]
    out dx, eax	

; write the result into the output
    mov qword ptr[rbx], rcx
    sti
    pop rsi
    pop rbx
    ret

_read_pci_bar_size	ENDP
 
 ; _rdmsr(int MSR, PVOID pOut)
 _rdmsr PROC PUBLIC
    push rbx
    mov rbx, rdx    
    rdmsr
    shl rdx, 20h
    or rax, rdx
    mov qword ptr [rbx], rax   
    pop rbx
    ret
_rdmsr ENDP

 ; _wrmsr(int MSR, UINT64 Value)
 _wrmsr PROC PUBLIC
    xor rax, rax
    mov eax, edx
    shr rdx, 20h
    wrmsr	
    ret
_wrmsr ENDP

END