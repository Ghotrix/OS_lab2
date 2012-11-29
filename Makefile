CFLAGS=-g -c -std=c99

all: script.asm

c: script

script.asm: script.o
	gcc -nostartfiles -o script script.o

script.o: script.s
	nasm -g -f elf64 script.s -l script.lst -o script.o

main.o: main.c
	gcc $(CFLAGS) main.c

script: main.o
	gcc main.o -o script

clean:
	rm -rf *.o script
