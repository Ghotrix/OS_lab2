CFLAGS=-g -c -std=c99

all: script

main.o: main.c
	gcc $(CFLAGS) main.c

map.o: map.c
	gcc $(CFLAGS) map.c

script: main.o map.o
	gcc main.o map.o -lm -o script

clean:
	rm -rf *.o script
