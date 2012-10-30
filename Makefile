CFLAGS=-g -c -std=c99

all: script

main.o: main.c
	gcc $(CFLAGS) main.c

map.o: map.c
	gcc $(CFLAGS) map.c

pipe.o: pipe.c
	gcc $(CFLAGS) pipe.c

script: main.o map.o pipe.o
	gcc main.o map.o pipe.o -o script

clean:
	rm -rf *.o script
