CFLAGS=-g -c -std=c99

all: script

main.o: main.c
	gcc $(CFLAGS) main.c

script: main.o
	gcc main.o -o script

clean:
	rm -rf *.o script
