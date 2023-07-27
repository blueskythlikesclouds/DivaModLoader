.code

?spriteLoaderFixupInfoInSprite@@YAXXZ:
	mov [rdx + r11 + 8], rcx
	mov eax, [r8]
	and eax, 0F0000000h
	shl eax, 3
	mov [rdx + r11 + 10h], eax
	mov eax, [r8]
	and eax, 0FFFFFFFh
	or [rdx + r11 + 10h], eax
	ret

public ?spriteLoaderFixupInfoInSprite@@YAXXZ

end