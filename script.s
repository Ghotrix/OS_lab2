section .data
	app0	db	"cat"
	app1	db	"grep"
	app2	db	"awk"
	app3	db	"sed"
	app4	db	"wc"
	app5	db	"sort"
	app6	db	"uniq"
	app7	db	"head"
	len:	equ	$-msg

section .text
	global _start

_start:
	mov	edx, len
	mov	ecx, msg
	mov	ebx, 1		; stdout
	mov	eax, 4		; write(2)
	int	0x80

	mov	ebx, 0
	mov	eax, 1		; exit(2)
	int	0x80
