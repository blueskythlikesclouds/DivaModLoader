; Same deal as StrArrayImp.asm

.code

?findOrCreateScoreImp@@YAPEAUScore@@PEAXH@Z proto

?implOfFindOrCreateScore@@YAPEAUScore@@PEAXI@Z:
    push rbx
    push rcx
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

    call ?findOrCreateScoreImp@@YAPEAUScore@@PEAXH@Z
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
    pop rcx
    pop rbx
    ret

public ?implOfFindOrCreateScore@@YAPEAUScore@@PEAXI@Z

?findScoreImp@@YAPEAUScore@@PEAXH@Z proto

?implOfFindScore@@YAPEAUScore@@PEAXI@Z:
    push rbx
    push rcx
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

    call ?findScoreImp@@YAPEAUScore@@PEAXH@Z
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
    pop rcx
    pop rbx
    ret

public ?implOfFindScore@@YAPEAUScore@@PEAXI@Z

?findModuleImp@@YAPEAUModule@@PEAXI@Z proto

?implOfFindModule@@YAPEAUModule@@PEAXI@Z:
    push rbx
    push rcx
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

    call ?findModuleImp@@YAPEAUModule@@PEAXI@Z
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
    pop rcx
    pop rbx
    ret

public ?implOfFindModule@@YAPEAUModule@@PEAXI@Z

?findCstmItemImp@@YAPEAUCstmItem@@PEAXI@Z proto

?implOfFindCstmItem@@YAPEAUCstmItem@@PEAXI@Z:
    push rbx
    push rcx
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

    call ?findCstmItemImp@@YAPEAUCstmItem@@PEAXI@Z
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
    pop rcx
    pop rbx
    ret

public ?implOfFindCstmItem@@YAPEAUCstmItem@@PEAXI@Z

end