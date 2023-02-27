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


section .text

; void _swsmi( smi_code_data, rax_value, rbx_value, rcx_value, rdx_value, rsi_value, rdi_value);
_swsmi:
    push rbx

    ;   RAX will get partially overwritten (AX) by _smi_code_data (which is passed in RCX)
    ;   RDX will get partially overwritten (DX) by the value of APMC port (= 0x00B2)
    mov rax, rsi ; rax_value
    mov ax, di   ; smi_code_data
    mov rbx, rdx ; rbx value
    ; rcx already in its right value
    mov rdx, r8  ; rdx value
    mov rsi, r9  ; rsi value
    mov rdi, [rsp + 010h] ; rdi_value
	mov dx, 0B2h

    ; this OUT instruction will write WORD value (smi_code_data) to ports 0xB2 and 0xB3 (SW SMI control and data ports)
    out dx, ax

    pop rbx
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
