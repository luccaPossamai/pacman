#include <pacmanmap.c>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>

#define MENU_ITENS 3

typedef void (*AcaoMenu)(void);

void readKeys(void);
long get_time_ms(void);
void initiateKeyHandler(void);

void iniciar_jogo_acao(void); //prototipos do menu
void carregar_jogo_acao(void);
void sair_acao(void);
void menu_loop(void);
void loadLastSave(void);
void saveLastLoad(void);
void tela_vitoria(void);
int pilulas_coletadas(Map *map);

int lastDirection = -1; //direção do jogador
int keyPressed, pause = 0, carregarJogo = 0;
int playerPos = 21; //acabei deixando começar em nesse quadrado
Map map;
FILE* load = NULL;
Score score;
char startString[128] = "Iniciar Novo Jogo";
char loadString[128] = "Carregar jogo";

typedef struct {
    char *texto;
    AcaoMenu acao;
} MenuItem; //menu



void iniciar_jogo_acao(void) {
    pause = 0; // sair do menu e iniciar jogo
    strcpy(startString, "Iniciar Novo Jogo (Iniciando...)");

}

void carregar_jogo_acao(void){
  load = fopen("save.dat", "r");
  if(load != NULL){

    carregarJogo = 1;
    pause = 0;
  } else {
    strcpy(loadString, "Carregar jogo (nenhum jogo salvo)");
  }

}

void sair_acao(void) {
    endwin();
    exit(0);
}

// Menu principal usando array
void menu_loop(void) {
	nodelay(stdscr, FALSE);
    MenuItem menu[MENU_ITENS] = {
        {startString, iniciar_jogo_acao},
        {loadString, carregar_jogo_acao},
        {"Sair", sair_acao}
    };
    int selecionado = 0;

    int ch;
    while (pause || selecionado == 0) {
        clear();
        mvprintw(2, 10, "=== PACMAN  - MENU INICIAL ==="); //func da ncurses para printar texto em local especifico da tela
        for (int i = 0; i < MENU_ITENS; i++) {
            if (i == selecionado)
                attron(A_REVERSE); //da enfoque no texto, o A_REVERSE inverte as cores do texto com o fundo
            mvprintw(5 + i, 10, "%s", menu[i].texto);
            if (i == selecionado)
                attroff(A_REVERSE);
        }
        mvprintw(8, 5, "Use as setas para navegar e Enter para selecionar.");
        refresh();

        ch = getch();
        switch (ch) {
            case KEY_UP:
                selecionado = (selecionado - 1 + MENU_ITENS) % MENU_ITENS;
                break;
            case KEY_DOWN:
                selecionado = (selecionado + 1) % MENU_ITENS;
                break;
            case '\n': // Enter
                if (menu[selecionado].acao)
                    menu[selecionado].acao();
                break;
            case 27: // ESC sai
                sair_acao();
                break;
        }
    }
	nodelay(stdscr, TRUE);
}

int main() {
	initiateKeyHandler();
	initScore(&score);
  	while (1) {
    pause = 1;       // pausa ativa ,"no menu"
    menu_loop();     // exibe menu até o usuário escolher iniciar ou sair

    if (pause == -1) { // usuário escolheu sair do menu
      break;        
    }
  
	//pause ==0 inicia o jogo
    if (map.pos != NULL) {
      deleteMap(&map);  // libere antes de recriar
    }
    createMap(&map, 20);
    if(carregarJogo) {
      loadLastSave();
    }
    char *mapString = malloc(1e3 * sizeof(char));
    long tTime0 = get_time_ms();
    long tTime = tTime0;
    
	  printMap(&map, mapString);
    while(!pause){
    	tTime = get_time_ms();
    	readKeys();
      if(pause) continue;
    	long delta = tTime - tTime0;
		if (delta >= 200) { // a cada 200ms o mapa atualiza, jogador se move, fantasmas se movem etc
			tTime0 = tTime;
	 		clear();
	 		if(lastDirection >= 0){
	 			playerPos = moveIndex(&map, playerPos, lastDirection, &score);//move o jogador e atualiza a posição
        if (pilulas_coletadas(&map)) {
            tela_vitoria();
            break;
        }

	 		}
	 		printw(" Pressione 'esc' para salvar e voltar ao menu\n");
			// ncurses nao deixa printar com printf
			// tive que fazer essa gambiarra aqui
			printMap(&map, mapString);
			printw("%s", mapString);

			printw("Score: %d\n", getPoints(&score)); //printa o score atual na tela, vindo da TAD  Score implementada em pacmanmap.c
			// printa algumas informações
			refresh();
    	}
    }
    saveLastLoad();
    strcpy(loadString, "Carregar jogo");
    strcpy(startString, "Iniciar Novo Jogo");

    free(mapString);
	  deleteMap(&map);
}   
    endwin(); 
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
int pilulas_coletadas(Map *map) {
    for (int i = 0; i < map->NSIZE; i++) {
        if (map->pos[i] == SCORE) {
            return 0;  // Ainda há pílulas
        }
    }
    return 1;  // Todas foram coletadas
}
void tela_vitoria() {
    clear();
    mvprintw(5, 10, " VC VENCEU!!! ");
    mvprintw(7, 10, "Score Final: %d", getPoints(&score));
    mvprintw(9, 10, "1 - Voltar ao Menu");
    mvprintw(10, 10, "2 - Sair");
    refresh();

    int ch;
    while (1) {
        ch = getch();
        if (ch == '1') {
            pause = 1;
            break;
        } else if (ch == '2') {
            pause = -1;
            break;
        }
    }
}



// leitura das teclas de entrada
void readKeys(){
	keyPressed = getch();
	switch(keyPressed){
		case(27): //esc
      pause = 1;
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

void loadLastSave(){
  FILE *load = fopen("save.dat", "r");
  char linha[1048];

  if (fgets(linha, sizeof(linha), load) != NULL) {
        score.pontos = atoi(linha);  // ou sscanf(linha, "%d", &score.pontos);
  }

  if (fgets(linha, sizeof(linha), load) != NULL) {
        playerPos = atoi(linha);  // ou sscanf(linha, "%d", &score.pontos);
  }

  
  
  if (fgets(linha, sizeof(linha), load) != NULL) {
    for (int i = 0; i < map.NSIZE; i++) {
      map.pos[i] = linha[i];
    }
  }

  fclose(load);

}
void saveLastLoad(void){
    FILE *load = fopen("save.dat", "w");
    if (load == NULL) return;


    fprintf(load, "%d\n", score.pontos);
    fprintf(load, "%d\n", playerPos);
    fwrite(map.pos, sizeof(char), map.NSIZE, load);

    fclose(load);
}

