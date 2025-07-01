#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define RIGHT 		0
#define LEFT  		1
#define UP		2
#define DOWN		3

const char WALL    = '#';
const char PLAYER  = 'C';
const char ENEMY   = 'E';
const char SCORE   = '.';
const char POWERUP = 'O';
const char EMPTY   = ' ';



typedef struct {
    int pontos;
} Score;

void initScore(Score* s) {
    s->pontos = 0;
}

void addPoints(Score* s, int valor) {
    s->pontos += valor;
}
int getPoints(Score* s) {
    return s->pontos;
}


typedef struct Map {
	int LSIZE;
	int NSIZE;
	char* pos;
	int** neighbourMatrix;
} Map;

void createMap(Map*, int length);
int moveIndex(Map*, int, int, Score* score);
void deleteMap(Map*);
void printMap(Map*, char*);

void populateInitialPositions(Map* map);
void createMap(Map* map, int length){
	int size = length * length;
	map -> NSIZE = size;
	map -> LSIZE = length;
	map -> pos = malloc(size * sizeof(char));
	map -> neighbourMatrix = malloc(size * sizeof(int*));
    
    for(int i = 0; i < size; i++){
        map -> neighbourMatrix[i] = malloc(4 * sizeof(int));
    }
    //inicialização da matrix de vizinhos das posições
    //mapa periódico lados grudados
    for(int i = 0; i < size; i++){
        map -> neighbourMatrix[i][0] = i % length == (length - 1) ? i - length + 1 : i + 1;
        map -> neighbourMatrix[i][1] = i % length == 0       ? i + length - 1 : i - 1;
        map -> neighbourMatrix[i][2] = i < length ? i + size - length : i - length;
        map -> neighbourMatrix[i][3] = i >= (size - length) ? i - size + length : i + length;  
    }
    
    populateInitialPositions(map);
}

void deleteMap(Map *map){
	int size = map -> NSIZE;
	for(int i = 0; i < size; i++){
		free(map -> neighbourMatrix[i]);
	}
	free(map -> neighbourMatrix);
	free(map -> pos);
	map = NULL;
}

void populateInitialPositions(Map* map){
	char *str = "####################"
				"#>.................#"
				"#.##.###.##.###.##.#"
				"#....#...##...#....#"
				"##.#.#.#.##.#.#.#.##"
				"#..#...#....#...#..#"
				"#.####.######.####.#"
				"...................."
				"##.###.##..##.###.##"
				"##.#.#.######.#.#.##"
				"#..#....EEEE....#..#"
				"#.####.######.####.#"
				"#..................#"
				"###.###.####.###.###"
				"#...#..........#...#"
				"#.#.#.#.####.#.#.#.#"
				"#.#...#......#...#.#"
				"#.###.###..###.###.#"
				"#..................#"
				"####################";
	for(int i = 0; i < map->NSIZE; i++){
		map->pos[i] = str[i];
	}
}
int moveTo(Map* map, int from, int to, Score* score){
	if(map -> pos[to] == WALL) return from;

	 if (map->pos[to] == SCORE && score != NULL) {
        addPoints(score, 10); // soma 10 pontos ao comer '.'
    }
	map -> pos[to] = map -> pos[from];
	map -> pos[from] = EMPTY;
	return to;
}
int moveIndex(Map* map, int index, int direction, Score* score){
	return moveTo(map, index, map -> neighbourMatrix[index][direction], score);
}
void printMap(Map* map, char* str){
	char temp[200];str[0] = '\0';
	int length = map -> LSIZE;
	for(int i = 0; i < length; i++){
		for(int j = 0; j < length; j++){
			sprintf(temp, "%c ", map -> pos[j + length * i]);
			strcat(str, temp);
		}
		strcat(str, "\n");
	}
}

