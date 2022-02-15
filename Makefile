all: poke327

poke327: main.o world.o map.o
	gcc main.o world.o map.o -o poke327

main.o: main.c world.h globals.h
	gcc -Wall -Werror -g -o main.o -c main.c

world.o: world.c world.h globals.h terrain.h pos.h map.h
	gcc -Wall -Werror -g -o world.o -c world.c

map.o: map.c map.h globals.h terrain.h pos.h
	gcc -Wall -Werror -g -o map.o -c map.c


clean:
	rm -f poke327 *~ *.o core

