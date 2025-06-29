
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define RIGHT 		0
#define LEFT  		1
#define UP			2
#define DOWN		3

typedef enum {
  WALL    = 0,
  PLAYER  = 1,
  ENEMY   = 2,
  SCORE   = 3,
  POWERUP = 4,
  EMPTY   = 5
} ElementType;

typedef struct Map {
	int LSIZE;
	int NSIZE;
	char* pos;
	int** neighbourMatrix;
} Map;

extern const char* CHAR_ELEMENTS[EMPTY + 1];

void createMap(Map*, int length);
void loadMap(Map*, int length);
int moveIndex(Map*, int, int);
void deleteMap(Map*);
void printMap(Map*, char*);

