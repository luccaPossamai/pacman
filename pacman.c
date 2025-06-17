#include <pacmanmap.c>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>

void readKeys(void);
long get_time_ms(void);
void initiateKeyHandler(void);

int lastDirection = -1; //direção do jogador
int keyPressed, pause = 0;
int playerPos = 21; //acabei deixando começar em nesse quadrado
Map map;

int main() {
	initiateKeyHandler();
	createMap(&map, 20);
    char *mapString = malloc(1e3 * sizeof(char));
    long tTime0 = get_time_ms();
    long tTime = tTime0;
    
	printMap(&map, mapString);
    while(!pause){
    	tTime = get_time_ms();
    	readKeys();
    	long delta = tTime - tTime0;
		if (delta >= 200) { // a cada 200ms o mapa atualiza, jogador se move, fantasmas se movem etc
			tTime0 = tTime;
	 		clear();
	 		if(lastDirection >= 0){
	 			playerPos = moveIndex(&map, playerPos, lastDirection);//move o jogador e atualiza a posição
	 		}
	 		printw(" Pressione 'esc' para sair\n");
			// ncurses nao deixa printar com printf
			// tive que fazer essa gambiarra aqui
			printMap(&map, mapString);
			printw("%s", mapString);
			
			printw("\n%d %d %d %ds", playerPos, lastDirection, keyPressed, (int)((tTime - tTime0)));
			// printa algumas informações
			refresh();
    	}
    }
    free(mapString);
    
    endwin(); 
    deleteMap(&map);
	return 0;
}

// funcao que mede tempo em milisegundo
// necessario para sincronizar ticks do jogo
long get_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

void initiateKeyHandler(){
	initscr(); // Inicia a tela do ncurses
    cbreak();// Captura tecla sem precisar de Enter
    noecho(); // Não imprime teclas digitadas
    nodelay(stdscr, TRUE);//não esperar pela tecla
    keypad(stdscr, TRUE); //necessario para ler as setas
    
}

// leitura das teclas de entrada
void readKeys(){
	keyPressed = getch();
	switch(keyPressed){
		case(27):
			pause = !pause;
			break;
		case(KEY_UP):
			lastDirection = UP;
			break;
		case(KEY_DOWN):
			lastDirection = DOWN;
			break;
		case(KEY_LEFT):
			lastDirection = LEFT;
			break;
		case(KEY_RIGHT):
			lastDirection = RIGHT;
			break;
			
	}
}

//função de checagem para ver se a alocação funcionou
void* safeMAlloc(unsigned int size) {
  void *ptr = malloc(size);
  if (ptr == NULL) {
      printf("Error: malloc got NULL!\n");
      exit(EXIT_FAILURE);
  }
  return ptr;
}
