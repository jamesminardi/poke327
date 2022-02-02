all: poke327

poke327: main.c
	gcc main.c -o poke327 -Wall -Werror
clean:
	rm -f poke327 *~ *.o core

