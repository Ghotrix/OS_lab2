CFLAGS=-g -c -std=c99

all: script

asm: script.asm

script.asm: script.o
	ld -o script script.o

script.o: script.s
	nasm -f elf64 script.s -o script.o

main.o: main.c
	gcc $(CFLAGS) main.c

script: main.o
	gcc main.o -o script

clean:
	rm -rf *.o script script.asm
