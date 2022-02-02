#include <stdio.h>
#define COLS 80
#define ROWS 21


void world_init(char world[ROWS][COLS]);
void world_print(char world[ROWS][COLS]);

int main(int argc, char *argv[]) {

  char world[ROWS][COLS];
  printf("Hello, World!\n");
  world_init(world);
  world_print(world);

  return 0;
  
}

void world_init(char world[ROWS][COLS]) {

  unsigned int i;
  unsigned int j;
  for (i = 0; i < ROWS; i++) {
    for (j = 0; j < COLS; j++) {
      
      world[i][j] = '.';
      
    }
  }
  
}

void world_print(char world[ROWS][COLS]) {
  unsigned int i;
  unsigned int j;
  
    for (i = 0; i < 21; i++) {
      for (j = 0; j < 80; j++) {
	
	printf("%c", world[i][j]);
      }
      printf("\n");
    }
}
