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

; void _swsmi( smi_code_data, rax_value, rbx_value, rcx_value, rdx_value, rsi_value, rdi_value);
_swsmi	PROC PUBLIC
 
    push rbx
    push rsi
    push rdi

    ;   RAX will get partially overwritten (AX) by _smi_code_data (which is passed in RCX)
    ;   RDX will get partially overwritten (DX) by the value of APMC port (= 0x00B2)
    mov rax, rdx ; rax_value
    mov ax, cx   ; smi_code_data
    mov rdx, [rsp + 040h] ; rsp + 0x28 + 0x18
	mov dx, 0B2h
    mov rbx, r8  ; rbx_value
    mov rcx, r9  ; rcx_value
    mov rsi, [rsp + 048h] ; rsi_value
    mov rdi, [rsp + 050h] ; rdi_value

    ; this OUT instruction will write WORD value (smi_code_data) to ports 0xB2 and 0xB3 (SW SMI control and data ports)
    out dx, ax

    pop rdi
    pop rsi
    pop rbx
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