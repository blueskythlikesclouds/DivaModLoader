.code

; THIS FUNCTION IS CURSED.
; The original function barely utilizes any registers, so functions calling it don't care about their temporary registers getting corrupted.
; The hook changes them, so we run into crashes.

; Push every register known to mankind to overcome this issue.

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

    call ?getStrImp@@YAPEBDH@Z

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
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov rdx, r14
    call ?getModuleNameImp@@YAPEBDHH@Z
    mov r8, 0FFFFFFFFFFFFFFFFh

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

    mov rdx, rbx
    call ?getCustomizeNameImp@@YAPEBDHH@Z
    mov r8, 0FFFFFFFFFFFFFFFFh

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

end