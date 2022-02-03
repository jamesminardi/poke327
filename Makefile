all: poke327

poke327: main.o world.o
	gcc main.o world.o -o poke327

main.o: main.c world.h globals.h
	gcc -Wall -Werror -g -o main.o -c main.c

world.o: world.c world.h globals.h tiles.h
	gcc -Wall -Werror -g -o world.o -c world.c


clean:
	rm -f poke327 *~ *.o core

