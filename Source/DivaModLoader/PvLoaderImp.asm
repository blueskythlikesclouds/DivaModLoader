pushaq macro
	push rbp
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
endm

popaq macro
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
	pop rbp
endm

.code 

?originalPvLoaderParseStart@@3P6AXXZEA proto
?pvLoaderParseStartImp@@YAIPEBD_K@Z proto

?implOfPvLoaderParseStart@@YAXXZ:
	pushaq
	mov rcx, rdi
	mov rdx, rbx
	sub rsp, 20h
	call ?pvLoaderParseStartImp@@YAIPEBD_K@Z
	add rsp, 20h
	popaq
	mov r14d, eax
	mov r13, 4325c53ef368ebh
	ret

public ?implOfPvLoaderParseStart@@YAXXZ

?originalPvLoaderParseLoop@@3P6AXXZEA proto
?pvLoaderParseLoopImp@@YAIXZ proto

?implOfPvLoaderParseLoop@@YAXXZ:
	pushaq
	sub rsp, 20h
	call ?pvLoaderParseLoopImp@@YAIXZ
	add rsp, 20h
	popaq
	cmp eax, 0FFFFFFFFh
	jz finish
	mov r14d, eax
	lea rax, ?originalPvLoaderParseStart@@3P6AXXZEA
	mov rax, [rax]
	add rax, 0Fh
	jmp rax
finish:
	lea rax, ?originalPvLoaderParseLoop@@3P6AXXZEA
	mov rax, [rax]
	add rax, 10h
	jmp rax

public ?implOfPvLoaderParseLoop@@YAXXZ

end