; Same deal as StrArrayImp.asm

.code

?findOrCreateScoreImp@@YAPEAUScore@@PEAXH@Z proto

?implOfFindOrCreateScore@@YAPEAUScore@@PEAXI@Z:
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
    sub rsp, 20h
    call ?findOrCreateScoreImp@@YAPEAUScore@@PEAXH@Z
    add rsp, 20h
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

public ?implOfFindOrCreateScore@@YAPEAUScore@@PEAXI@Z

?findScoreImp@@YAPEAUScore@@PEAXH@Z proto

?implOfFindScore@@YAPEAUScore@@PEAXI@Z:
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
    sub rsp, 20h
    call ?findScoreImp@@YAPEAUScore@@PEAXH@Z
    add rsp, 20h
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

public ?implOfFindScore@@YAPEAUScore@@PEAXI@Z

end