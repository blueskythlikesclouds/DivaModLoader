.code

; THIS FUNCTION IS CURSED.
; The original function barely utilizes any registers, so functions calling it don't care about their temporary registers getting corrupted.
; The hook changes them, so we run into crashes.

; Push every register known to mankind to overcome this issue.

; Additionally add 32 bytes of shadow space and align the stack pointer to 16 bytes to prevent further crashes.

?getStrImp@@YAPEBDH@Z proto

?implOfGetStr@@YAPEBDH@Z:
    push rbx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov r15, rsp
    sub rsp, 20h
    and rsp, 0FFFFFFFFFFFFFFF0h

    call ?getStrImp@@YAPEBDH@Z
    mov rsp, r15

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rbx

    ret

public ?implOfGetStr@@YAPEBDH@Z

?getModuleNameImp@@YAPEBDHH@Z proto

?implOfGetModuleName@@YAPEBDH@Z:
    push rbx
    push rdx
    push rsi
    push rdi
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov r15, rsp
    sub rsp, 20h
    and rsp, 0FFFFFFFFFFFFFFF0h

    mov rdx, r14
    call ?getModuleNameImp@@YAPEBDHH@Z
    mov r8, 0FFFFFFFFFFFFFFFFh

    mov rsp, r15

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop rdi
    pop rsi
    pop rdx
    pop rbx

    ret

public ?implOfGetModuleName@@YAPEBDH@Z

?getCustomizeNameImp@@YAPEBDHH@Z proto

?implOfGetCustomizeName@@YAPEBDH@Z:
    push rbx
    push rdx
    push rsi
    push rdi
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov r15, rsp
    sub rsp, 20h
    and rsp, 0FFFFFFFFFFFFFFF0h

    mov rdx, rbx
    call ?getCustomizeNameImp@@YAPEBDHH@Z
    mov r8, 0FFFFFFFFFFFFFFFFh

    mov rsp, r15

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop rdi
    pop rsi
    pop rdx
    pop rbx

    ret

public ?implOfGetCustomizeName@@YAPEBDH@Z

?getBtnSeNameImp@@YAPEBDHH@Z proto

?implOfGetBtnSeName@@YAPEBDH@Z:
    push rbx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov r14, rsp
    sub rsp, 20h
    and rsp, 0FFFFFFFFFFFFFFF0h

    mov rdx, [r8+r15]
    call ?getBtnSeNameImp@@YAPEBDHH@Z

    mov rsp, r14

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rbx

    movsxd rcx, r14d
    lea rcx, [rcx+rcx*8]

    ret

public ?implOfGetBtnSeName@@YAPEBDH@Z

?getSlideSeNameImp@@YAPEBDHH@Z proto

?implOfGetSlideSeName@@YAPEBDH@Z:
    push rbx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov r14, rsp
    sub rsp, 20h
    and rsp, 0FFFFFFFFFFFFFFF0h

    mov rdx, [r8+r15]
    call ?getSlideSeNameImp@@YAPEBDHH@Z
    
    mov rsp, r14

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rbx

    movsxd rcx, r14d
    lea rcx, [rcx+rcx*8]

    ret

public ?implOfGetSlideSeName@@YAPEBDH@Z

?getChainSlideSeNameImp@@YAPEBDHH@Z proto

?implOfGetChainSlideSeName@@YAPEBDH@Z:
    push rbx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov r15, rsp
    sub rsp, 20h
    and rsp, 0FFFFFFFFFFFFFFF0h

    imul rax, r14, 0A8h
    mov rdx, [rax+r8]
    call ?getChainSlideSeNameImp@@YAPEBDHH@Z

    mov rsp, r15

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbx

    movsxd rcx, r12d
    imul rdx, rcx, 0A8h

    ret

public ?implOfGetChainSlideSeName@@YAPEBDH@Z

?getSliderTouchSeNameImp@@YAPEBDHH@Z proto

?implOfGetSliderTouchSeName@@YAPEBDH@Z:
    push rbx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov r14, rsp
    sub rsp, 20h
    and rsp, 0FFFFFFFFFFFFFFF0h

    mov rdx, [r8+r15]
    call ?getSliderTouchSeNameImp@@YAPEBDHH@Z

    mov rsp, r14

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rbx

    movsxd rcx, r14d
    lea rcx, [rcx+rcx*8]

    ret

public ?implOfGetSliderTouchSeName@@YAPEBDH@Z

end