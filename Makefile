all: poke327

poke327: main.o world.o map.o heap.o
	gcc main.o world.o map.o heap.o -o poke327

main.o: main.c world.h globals.h
	gcc -g -o main.o -c main.c

world.o: world.c world.h globals.h colors.h components.h map.h heap.h
	gcc -g -o world.o -c world.c

map.o: map.c map.h globals.h colors.h components.h
	gcc -g -o map.o -c map.c

heap.o: heap.c heap.h
	gcc -Wall -Werror -g -o heap.o -c heap.c

clean:
	rm -f poke327 *~ *.o core

