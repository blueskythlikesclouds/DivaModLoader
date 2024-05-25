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

?spriteLoaderGetPvDifficultyStatesPtrImp@@YAPEA_NII@Z proto

?implOfSpriteLoaderGetPvDifficultyStatesPtr@@YAXXZ:
	pushaq
	mov rcx, [rdx]
	mov rdx, r8
	sub rsp, 20h
	call ?spriteLoaderGetPvDifficultyStatesPtrImp@@YAPEA_NII@Z
	add rsp, 20h
	popaq
	ret

public ?implOfSpriteLoaderGetPvDifficultyStatesPtr@@YAXXZ

?pvLoaderGetPvDifficultyStateImp@@YA_NIII@Z proto

?implOfPvLoaderGetPvDifficultyState@@YAXXZ:
	pushaq
	mov rcx, rdx
	mov rdx, r8
	mov r8, r10
	sub rsp, 20h
	call ?pvLoaderGetPvDifficultyStateImp@@YA_NIII@Z
	add rsp, 20h
	popaq
	cmp al, 0
	ret

public ?implOfPvLoaderGetPvDifficultyState@@YAXXZ

?originalPvLoaderGetPvExists@@3P6AXXZEA proto
?pvLoaderGetPvExistsImp@@YA_NI@Z proto

?implOfPvLoaderGetPvExists@@YAXXZ:
	pushaq
	mov rcx, rdx
	sub rsp, 20h
	call ?pvLoaderGetPvExistsImp@@YA_NI@Z
	add rsp, 20h
	popaq
	mov r9b, al
	lea rax, ?originalPvLoaderGetPvExists@@3P6AXXZEA
	mov rax, [rax]
	add rax, 0B7h
	jmp rax

public ?implOfPvLoaderGetPvExists@@YAXXZ

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
?pvLoaderLoopImp@@YAIXZ proto

?implOfPvLoaderParseLoop@@YAXXZ:
	pushaq
	sub rsp, 20h
	call ?pvLoaderLoopImp@@YAIXZ
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