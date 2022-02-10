all: poke327

poke327: main.o map.o
	gcc main.o map.o -o poke327

main.o: main.c map.h globals.h
	gcc -Wall -Werror -g -o main.o -c main.c

map.o: map.c map.h globals.h terrain.h
	gcc -Wall -Werror -g -o map.o -c map.c


clean:
	rm -f poke327 *~ *.o core

