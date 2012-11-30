;extern scanf, sprintf, printf, dprintf

section .data
	app0:				db	'/usr/bin/cat',0
	app1:				db	'/usr/bin/grep',0
	app2:				db	'/usr/bin/awk',0
	app3:				db	'/bin/sed',0
	app4:				db	'/usr/bin/wc',0
	app5:				db	'/usr/bin/sort',0
	app6:				db	'/usr/bin/uniq',0
	app7:				db	'/usr/bin/head',0
	arg0:				db	'log.txt',0
	arg1:				db	'10/Oct/2006',0
	arg2:				db	'{print $12}',0
	arg3:				db	's/"//g',0
	arg4:				db	's/\/.*//g',0
	arg5:				db	'-v',0
	arg6:				db	'-l',0
	arg7:				db	'-',0
	arg8				db	'-c',0
	arg9				db	'-rnk 1',0
	arg10				db	'-n 10',0
	printf_line			db	'{printf "%s - %d - %0.f%\n", $2, $1, $1 * 100 /                   }',0
	scanf_arg:			db	'%d',0xa,0
	pipe_err:			db	'Cannot pipe...',0xa,0
	pipe_err_len:		equ	$-pipe_err-1
	dup_stdin_err:		db	'Cannot dup2 stdin...',0xa,0
	dup_stdin_err_len:	equ	$-dup_stdin_err-1
	dup_stdout_err:		db	'Cannot dup2 stdout...',0xa,0
	dup_stdout_err_len:	equ	$-dup_stdout_err-1
	fork_err:			db	'Cannot fork, exiting...',0xa,0
	fork_err_len:		equ	$-fork_err-1
	local_buf_len		dd	8
	
section .bss
	pipes			resd	16*2
	child_pids		resd	16
	app_names		resd	16
	app_args		resd	16
	command			resd	4
	num				resd	1
	tmp_num			resd	1
	counter			resw	1
	local_buf		resb	8
	figure_len		resd	1
	is_first_part	resb	1
	awk_line		resb	128

section .text
	global _start

_start:
	; preparing app names array for iterating over it
	mov eax, app0
	mov [app_names + 0], eax
	mov eax, app1
	mov [app_names + 4], eax
	mov [app_names + 20], eax
	mov eax, app2
	mov [app_names + 8], eax
	mov eax, app3
	mov [app_names + 12], eax
	mov [app_names + 16], eax
	mov eax, app4
	mov [app_names + 24], eax

	; preparing arg names array
	mov eax, arg0
	mov [app_args + 0], eax
	mov eax, arg1
	mov [app_args + 4], eax
	mov eax, arg2
	mov [app_args + 8], eax
	mov eax, arg3
	mov [app_args + 12], eax
	mov eax, arg4
	mov [app_args + 16], eax
	mov eax, arg5
	mov [app_args + 20], eax
	mov eax, arg6
	mov [app_args + 24], eax

	xor esi, esi	; clearing esi for loop

piping:
	mov eax, 42		; pipe
	mov ebx, pipes
	push rsi
	imul esi, 8
	add	ebx, esi
	pop rsi
	int 80h
	cmp eax, -1
	je	piping_error
	inc esi
	cmp esi, 7		; number of processes need to pipe and run
	jne piping

	xor esi, esi

forking:
	mov [counter], si
	mov eax, 2		; fork
	int 80h
	cmp eax, 0
	je child_proc
	cmp eax, -1
	je cannot_fork
	jne	parent

child_proc:
	xor esi, esi
	mov si, [counter]
	cmp esi, 0
	je dup2_stdout

dup2_stdin:
	mov eax, 63		; dup2
	mov ebx, pipes ; dup2 stdin
	push rsi
	dec esi
	imul esi, 8
	add ebx, esi
	mov ebx, [ebx]
	pop rsi
	mov ecx, 0
	int 80h

	cmp eax, -1		; checking if dup2 was successful
	je	dup2_stdin_error

dup2_stdout:
	;cmp esi, 6
	;je	before_close_loop
	mov eax, 63		; dup2
	mov ebx, pipes	; dup2 stdout
	push rsi
	imul esi, 8
	add esi, 4
	add ebx, esi
	mov ebx, [ebx]
	pop rsi
	mov ecx, 1
	int 80h

	cmp eax, -1		; checking if dup2 was successful
	je	dup2_stdout_error

before_close_loop:
	xor edi, edi	; clearing edi for inner loop
	
close_loop:
	mov eax, 6		; close
	mov ebx, pipes
	push rdi
	imul edi, 8
	add ebx, edi
	mov ebx, [ebx]
	int 80h

	mov eax, 6		; close
	mov ebx, pipes
	add edi, 4
	add ebx, edi
	mov ebx, [ebx]
	pop rdi
	int 80h

	inc edi
	cmp edi, 7
	jne	close_loop

execing:
	mov eax, app_names
	push rsi
	imul esi, 4
	add eax, esi
	mov eax, [eax]
	mov [command], eax
	mov eax, app_args
	add eax, esi
	mov eax, [eax]
	mov [command + 4], eax
	pop rsi
	cmp esi, 5
	jne end_list

	mov eax, arg7
	mov [command + 8], eax
	mov eax, 0
	mov [command + 12], eax

	jmp actual_execing

end_list:
	mov eax, 0
	mov [command + 8], eax

actual_execing:
	mov eax, 11		; execve
	mov ebx, [command]
	mov ecx, command
	mov edx, 0
	int 80h

	jmp err_exit

parent:
	mov [child_pids+esi*4], eax
	inc esi
	cmp esi, 7
	jne	forking

	xor esi, esi
	
parent_close_loop:
	mov eax, 6		; close
	mov ebx, pipes
	push rsi
	imul esi, 8
	add ebx, esi
	mov ebx, [ebx]
	int 80h
	mov eax, 6		; close
	mov ebx, pipes
	add esi, 4
	add ebx, esi
	mov ebx, [ebx]
	pop rsi
	int 80h

	inc esi
	cmp esi, 6
	jne	parent_close_loop

closing_reading:
	mov eax, 6		; close
	mov ebx, pipes
	add ebx, 6*8+1*4
	mov ebx, [ebx]
	int 80h

	mov eax, 63		; dup2
	mov ebx, pipes ; dup2 stdin
	add ebx, 6*8+0*4
	mov ebx, [ebx]
	mov ecx, 0
	int 80h

	cmp eax, -1		; checking if dup2 was successful
	je	dup2_stdin_error

	mov eax, 6		; close
	mov ebx, pipes
	add ebx, 6*8+0*4
	mov ebx, [ebx]
	int 80h

	mov eax, 3		; read
	mov ebx, 0
	mov ecx, local_buf
	mov edx, [local_buf_len]
	int 80h

	mov edi, -1
	xor eax, eax

scan_loop:
	inc edi
	mov al, [local_buf+edi]
	cmp al, 0xa
	jne scan_loop

	mov [figure_len], edi
	
	xor edi, edi
copy_loop:
	mov eax, [printf_line+edi*4]
	mov [awk_line+edi*4], eax
	;xor rax, rax
	;mov rax, num
	;push rax
	;push scanf_arg
	;call scanf

	;push qword [num]
	;push scanf_arg
	;call printf

	xor edi, edi

waitpid_loop:
	mov eax, 7
	mov ebx, [child_pids+edi*4]
	mov ecx, 0
	mov edx, 0
	int 80h

	inc edi
	cmp edi, 7
	jne waitpid_loop

	jmp normal_exit

cannot_fork:
	mov eax, 4		; write
	mov ebx, 2		; stderr
	mov ecx, fork_err
	mov edx, fork_err_len
	int 80h
	jmp err_exit

piping_error:
	mov eax, 4		; write
	mov ebx, 2		; stderr
	mov ecx, pipe_err
	mov edx, pipe_err_len
	int 80h
	jmp	err_exit
; pipe_error end

dup2_stdin_error:
	mov eax, 4		; write
	mov ebx, 2		; stderr
	mov ecx, dup_stdin_err
	mov edx, dup_stdin_err_len
	int 80h
	jmp err_exit
; dup_stdin_error end

dup2_stdout_error:
	mov eax, 4		; write
	mov ebx, 2		; stderr
	mov ecx, dup_stdout_err
	mov edx, dup_stdout_err_len
	int 80h
; dup_stdout_error end

err_exit:
	mov eax, 1		; exit
	mov ebx, 1
	int 80h

normal_exit:
	mov	eax, 1		; exit(2)
	mov	ebx, 0
	int	80h
