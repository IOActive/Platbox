global _swsmi
global _read_pci_compatible_configuration_space
global _read_pci_byte
global _read_pci_word
global _read_pci_dword
global _read_pci_bar_size
global _write_pci_byte
global _write_pci_word
global _write_pci_dword
global _rdmsr
global _wrmsr
global _store_savestate
global _cli
global _sti
global _swsmi_sinkclose

section .text


_store_savestate:
; 0x3200 -> RIP
; 0x3208 -> RSP
; 0x3210 -> RBP
; 0x3218 -> CR3
; 0x3220 -> Mutex (signal from core0)

	; Save CR3
    push rax
    mov rax, cr3
    mov [rdi + 18h], rax

    ; RIP is not saved here?
    mov rax, _exit_store_savestate
    mov [rdi], rax
    pop rax
    
    mov [rdi + 10h], rbp
    mov [rdi + 08h], rsp

	mov dword [rdi + 20h], 0 ; Set the Mutex to 0
_exit_store_savestate:
    ret

; void _swsmi(SW_SMI_CALL *smi_call);
_swsmi:
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push  r9
    push  r8
    push rsi
    push rdx
    push rbx
    push rcx
    push rdi

    ; set registers to state of provided structure
    mov r15, [rdi + 080h]
    mov r14, [rdi + 078h]
    mov r13, [rdi + 070h]
    mov r12, [rdi + 068h]
    mov r11, [rdi + 060h]
    mov r10, [rdi + 058h]
    mov  r9, [rdi + 050h]
    mov  r8, [rdi + 048h]
    ; rdi handled later
    mov rsi, [rdi + 038h]
    mov rdx, [rdi + 030h]
	mov rcx, [rdi + 028h]
    mov rbx, [rdi + 020h]

    ; The trigger port might be different than 0xB2 in AMD
    mov dx, [rdi + 010h] 

    mov rax,  [rdi + 08h] 
    shl rax, 8
    or rax,  [rdi] 

    mov rdi, [rdi + 040h] ; get rdi value just before the OUT instruction
	
	wbinvd
    ; this OUT instruction will write WORD value (smi_code_data) to ports 0xB2 and 0xB3 (SW SMI control and data ports)
    out dx, ax

    ;; write to structure the changes that could have happened in SMM

    push rax; save return RAX from SMI
    mov rax, [rsp + 08h]; rax points to original RDI

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

    pop rdi
    pop rcx
    pop rbx
    pop rdx
    pop rsi
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

_swsmi_sinkclose:
    ; The trigger port might be different than 0xB2 in AMD
    mov dx, [rdi] 
	xor rax, rax
	wbinvd
    out dx, ax
    xor rax, rax
    ret

; _read_pci_compatible_configuration_space(UINT8 Bus, UINT8 Device, UINT8 Function, PVOID pOut)
; Reads the 256 bytes of PCI Configuration data from a BDF into pOut
_read_pci_compatible_configuration_space:
	push rbx
	cli

	xor rax, rax
	mov al, 1
	shl eax, 1Fh
	shl edi, 10h
	shl esi, 0Bh
	shl edx, 08h
	or eax, edi
	or eax, esi
	or eax, edx
	mov rbx, rax
	mov rdi, rcx
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
	
	mov [rdi+rcx*4], eax
	
	cmp cl, 40h
	jnz _loop
	
	sti
	pop rbx
	ret


; void _read_pci_byte( UINT32 CF8, PVOID pOut);
_read_pci_byte:	
	cli
	xor rdx, rdx
	mov dx, 0CF8h
	mov eax, edi
	out dx, eax
	xor rax, rax
	mov dx, 0CFCh
	in al, dx
	mov [rsi], al
	sti
	ret


; void _read_pci_word( UINT32 CF8, PVOID pOut);
_read_pci_word:
	cli
	xor rdx, rdx
	mov dx, 0CF8h
	mov eax, edi
	out dx, eax
	xor rax, rax
	mov dx, 0CFCh
	in ax, dx
	mov [rsi], ax
	sti
	ret


; void _read_pci_dword( UINT32 CF8, PVOID pOut);
_read_pci_dword:

	cli
	xor rdx, rdx
	mov dx, 0CF8h
	mov eax, edi
	out dx, eax
	xor rax, rax
	mov dx, 0CFCh
	in eax, dx
	mov [rsi], eax
	sti
	ret


; void _write_pci_byte( UINT32 CF8, UINT32 value);
_write_pci_byte:

	cli
	xor rdx, rdx
	mov dx, 0CF8h
	mov eax, edi
	out dx, eax
	xor rax, rax
	mov eax, esi
	mov dx, 0CFCh	
	out dx, al	
	sti
	ret


; void _write_pci_word( UINT32 CF8, UINT32 value);
_write_pci_word:
	cli
	xor rdx, rdx
	mov dx, 0CF8h
	mov eax, edi
	out dx, eax
	xor rax, rax
	mov eax, esi
	mov dx, 0CFCh	
	out dx, ax	
	sti
	ret


; void _write_pci_dword( UINT32 CF8, UINT32 value);
_write_pci_dword:
	cli
	xor rdx, rdx
	mov dx, 0CF8h
	mov eax, edi
	out dx, eax
	xor rax, rax
	mov eax, esi
	mov dx, 0CFCh	
	out dx, eax	
	sti
	ret


; void _read_pci_bar_size( UINT32 CF8, PVOID pOut);
_read_pci_bar_size:
	push rbx
    push rdx
    push rcx
	mov rbx, rsi
	mov rcx, rdi
; read the current BAR value	
	call _read_pci_dword
	mov rsi, rdi

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
	mov eax, [rbx]
	out dx, eax	

; write the result into the output
	mov [rbx], rcx
	sti
    pop rcx
    pop rdx
	pop rbx
	ret


 
 ; _rdmsr(int MSR, PVOID pOut)
 _rdmsr:
    mov rcx, rdi    
    rdmsr
	shl rdx, 20h
	or rax, rdx
    mov [rsi], rax   
    ret


 ; _wrmsr(int MSR, UINT64 Value)
 _wrmsr:
	xor rax, rax
    mov rdx, rsi
    mov eax, edx
	shr rdx, 20h
    mov rcx, rdi
    wrmsr	
    ret


_cli:
	cli
	ret

_sti:
	sti
	ret