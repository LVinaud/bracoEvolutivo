#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

//para compilar use gcc a_star.c -o a_star $(sdl2-config --cflags --libs) -lSDL2 -lm
//Para a parte gráfica, utilizei a biblioteca sdl2

int origemX, destinoX, origemY, destinoY;

#define SCREEN_LENGTH 640
#define NUM_SQUARES 80
#define MOSTRA_PROCESSO 0
#define SQUARE_SIZE (SCREEN_LENGTH/NUM_SQUARES)
#define MAX_VALUE (99999999999)
#define PESO_OBSTACULO -500

typedef struct item ITEM;
typedef struct FILA FILA;

struct item {
    float g, peso;
    int x, y;
    ITEM* pai;
};

typedef struct {
	SDL_Renderer *renderer;
	SDL_Window *window;
} Tela; //struct relacionada ao sdl.

void inicializaSDL(Tela* tela); //inicializa o renderizador e a janela
void mostraTela(Tela* tela);
void arrumaTela(Tela* tela);
void arrumaAtual(ITEM* atual, Tela* tela, int r, int g, int b);
void arrumaGrid(ITEM* grid[NUM_SQUARES][NUM_SQUARES], Tela* tela);
int XYparaIJ(int x);
int IJparaXY(int i);
void lerOrigemDestino(ITEM* grid[NUM_SQUARES][NUM_SQUARES], Tela* tela);
float distancia(int x1, int x2, int y1, int y2);
void mapValueToRGB(int value, int *red, int *green, int *blue);
void arrumaCaminho(ITEM* resposta, Tela* tela);

void arrumaAtual(ITEM* atual, Tela* tela, int r, int g, int b) {
    SDL_Rect rect = {atual->x - SQUARE_SIZE/2, atual->y - SQUARE_SIZE/2, SQUARE_SIZE, SQUARE_SIZE};
    SDL_SetRenderDrawColor(tela->renderer, r, g, b, 255);
    SDL_RenderFillRect(tela->renderer, &rect);
}

void arrumaCaminho(ITEM* resposta, Tela* tela) {
    if(resposta == NULL) return;
    SDL_Rect rect = {resposta->x - SQUARE_SIZE/2, resposta->y - SQUARE_SIZE/2, SQUARE_SIZE, SQUARE_SIZE};
    SDL_SetRenderDrawColor(tela->renderer, 0, 100, 200, 255);
    SDL_RenderFillRect(tela->renderer, &rect);
    arrumaCaminho(resposta->pai, tela);
}

void lerOrigemDestino(ITEM* grid[NUM_SQUARES][NUM_SQUARES], Tela* tela) {
    arrumaTela(tela);
    arrumaGrid(grid, tela);
    mostraTela(tela);
    int mouseClicado = 0;
    SDL_Event event;
    int destino_escolhido = 0, origem_escolhida = 0;
    int mouse_x, mouse_y;
    while(!destino_escolhido || !origem_escolhida)
    while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				exit(0);
				break;
            
            case SDL_KEYDOWN:
                SDL_Keycode keyPressed = event.key.keysym.sym;
                if(keyPressed == SDLK_d && !destino_escolhido) {//define o destino na grid e na variavel destino
                    SDL_GetMouseState(&mouse_x, &mouse_y);
                    destino_escolhido = 1;
                    destinoX = IJparaXY(XYparaIJ(mouse_x));
                    destinoY = IJparaXY(XYparaIJ(mouse_y));
                    arrumaTela(tela);
                    arrumaGrid(grid, tela);
                    mostraTela(tela);
                }
                else if(keyPressed == SDLK_o && !origem_escolhida) {//define o destino na grid e na variavel destino
                    SDL_GetMouseState(&mouse_x, &mouse_y);
                    origem_escolhida = 1;
                    origemX = IJparaXY(XYparaIJ(mouse_x));
                    origemY = IJparaXY(XYparaIJ(mouse_y));
                    arrumaTela(tela);
                    arrumaGrid(grid, tela);
                    mostraTela(tela);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                mouseClicado = 1;
                break;
            case SDL_MOUSEBUTTONUP:
                mouseClicado = 0;
                break;
            case SDL_MOUSEMOTION:
                if(!mouseClicado) break;
                SDL_GetMouseState(&mouse_x, &mouse_y);
                grid[XYparaIJ(mouse_y)][XYparaIJ(mouse_x)]->peso = PESO_OBSTACULO;
                arrumaTela(tela);
                arrumaGrid(grid, tela);
                mostraTela(tela);
                break;
            default:
				break;
		}
	}
}

void arrumaGrid(ITEM* grid[NUM_SQUARES][NUM_SQUARES], Tela* tela) {
    for(int i = 0; i < NUM_SQUARES; i++) {
        for(int j = 0; j < NUM_SQUARES; j++) {
            SDL_Rect rect_aux = {i*(SQUARE_SIZE), j*(SQUARE_SIZE), SQUARE_SIZE, SQUARE_SIZE};
            if(destinoX == grid[j][i]->x && destinoY == grid[j][i]->y) {
                SDL_SetRenderDrawColor(tela->renderer, 0, 0, 255, 255);
                SDL_RenderFillRect(tela->renderer, &rect_aux);
            } else if (origemX == grid[j][i]->x && origemY == grid[j][i]->y) {
                SDL_SetRenderDrawColor(tela->renderer, 255, 255, 0, 255);
                SDL_RenderFillRect(tela->renderer, &rect_aux);
            } else if(grid[j][i]->peso == PESO_OBSTACULO) {
                SDL_SetRenderDrawColor(tela->renderer, 0, 0, 0, 255);
                SDL_RenderFillRect(tela->renderer, &rect_aux);
            } else {
                if(grid[j][i]->peso < MAX_VALUE) {
                    SDL_SetRenderDrawColor(tela->renderer, 100, 100, 100, 255);
                    SDL_RenderFillRect(tela->renderer, &rect_aux);
                }
            }
        }
    }
}

void arrumaTela(Tela* tela) {
    SDL_SetRenderDrawColor(tela->renderer, 255, 255, 255, 255);
    SDL_RenderClear(tela->renderer);
}

void mostraTela(Tela* tela) {
    SDL_RenderPresent(tela->renderer);
}

void inicializaSDL(Tela* tela)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		exit(1);
	}

	tela->window = SDL_CreateWindow("A star", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_LENGTH, SCREEN_LENGTH, 0);

	if (!tela->window)
	{
		exit(1);
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	tela->renderer = SDL_CreateRenderer(tela->window, -1, SDL_RENDERER_ACCELERATED);

	if (!tela->renderer)
	{
		exit(1);
	}
}

ITEM* item_criar(float g, float peso, ITEM* pai, int x, int y) {
    ITEM* a = (ITEM*) malloc(sizeof(ITEM));
    a->g = g;
    a->pai = pai;
    a->x = x;
    a->y = y;
    a->peso = peso;
}

void item_apagar(ITEM** a) {
    if(*a != NULL) {
        free(*a);
        *a = NULL;
    }
}

struct FILA {
    ITEM** vetor;
    int tam, fim;
};

FILA *FILA_criar(void) {
    FILA *h;
    h = (FILA *) malloc(sizeof(FILA));
    if(h != NULL) {
        h->tam = 100;
        h->vetor = (ITEM**) malloc(sizeof(ITEM*) * h->tam);
        h->fim = -1;
        return h;
    }
    return NULL;
}

bool FILA_presente(FILA* h, ITEM* item) {
    for(int i = 0; i <= h->fim; i++) {
        if(h->vetor[i] == item) return true;
    }
    return false;
}

bool FILA_enfileirar(FILA *h, ITEM* item) {
    if(h != NULL) {
        h->fim += 1;
        if(h->fim >= h->tam) {
            h->tam += 100;
            h->vetor = (ITEM**) realloc(h->vetor, sizeof(ITEM*) * h->tam);
            if(h->vetor == NULL) exit(1);
        }
        int i = h->fim;
        while(i > 0 && h->vetor[i-1]->peso < item->peso) {
            h->vetor[i] = h->vetor[i-1];
            i--;
        }
        h->vetor[i] = item;
        return true;
    }
    return false;
}

ITEM *FILA_desenfileirar(FILA *h) {
    if(h != NULL && h->fim >= 0) {

        ITEM* aux = h->vetor[h->fim];
        h->fim -= 1;
        
        return aux;
    }
    return NULL;
}

bool FILA_removerEspecifico(FILA *h, ITEM* item) {
    if(h != NULL && item != NULL) {
        for(int i = 0; i <= h->fim; i++) {
            if(h->vetor[i] == item) {
                for(int j = i; j < h->fim; j++){
                    h->vetor[j] = h->vetor[j+1];
                }
                h->fim -= 1;
                return true;
            }            
        }
    }
    return false;
}

bool FILA_vazia(FILA *h) {
    if(h != NULL)
        return ((h->fim == -1) ? true : false);
    return false;
}

bool FILA_apagar(FILA **h) {
    if(*h != NULL) {
        free((*h)->vetor);
        (*h)->vetor = NULL;
        free(*h);
        *h = NULL;
        return true;
    }
    return false;
}

int XYparaIJ(int x) {
    return x/SQUARE_SIZE;
}
int IJparaXY(int i) {
    return i * SQUARE_SIZE + SQUARE_SIZE / 2;
}

float distancia(int x1, int x2, int y1, int y2) {
    return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

int main(void) {
    Tela tela;
    SDL_Event event;
    inicializaSDL(&tela);
    FILA* fila = FILA_criar();
    ITEM* resposta = NULL;
    ITEM* grid[NUM_SQUARES][NUM_SQUARES];
    for(int i = 0; i < NUM_SQUARES; i++) 
        for(int j = 0; j < NUM_SQUARES; j++) 
            grid[j][i] = item_criar(MAX_VALUE, MAX_VALUE, NULL, IJparaXY(i), IJparaXY(j));
    lerOrigemDestino(grid, &tela);
    grid[XYparaIJ(origemY)][XYparaIJ(origemX)]->peso = distancia(origemX, destinoX, origemY, destinoY);
    grid[XYparaIJ(origemY)][XYparaIJ(origemX)]->g = 0;
    FILA_enfileirar(fila, grid[XYparaIJ(origemY)][XYparaIJ(origemX)]);
    int dx[] = {0, 1, 0, -1, -1, 1, 1, -1};
    int dy[] = {1, 0, -1, 0, 1, 1, -1, -1};
    int quantas_distancias = 0;
    while(!FILA_vazia(fila)){
        ITEM* atual = FILA_desenfileirar(fila);
        if(atual->x == destinoX && atual->y == destinoY) {
            resposta = atual;
            break;
        }
        for(int i = 0; i < 8; i++) {
            if(XYparaIJ(atual->y) + dy[i] >= 0 && XYparaIJ(atual->y) + dy[i] < NUM_SQUARES && (XYparaIJ(atual->x) + dx[i]) >= 0 && (XYparaIJ(atual->x) + dx[i]) < NUM_SQUARES) {
                ITEM* vizinho = grid[(XYparaIJ(atual->y) + dy[i])][(XYparaIJ(atual->x) + dx[i])];
                if(vizinho->peso != PESO_OBSTACULO) {
                    float tentativaG = atual->g + distancia(atual->x, vizinho->x, atual->y, vizinho->y);
                    quantas_distancias++;
                    if(tentativaG < vizinho->g) {
                        vizinho->pai = atual;
                        vizinho->g = tentativaG;
                        vizinho->peso = tentativaG + distancia(vizinho->x, destinoX, vizinho->y, destinoY);
                        quantas_distancias++;
                        FILA_removerEspecifico(fila, vizinho);
                        FILA_enfileirar(fila, vizinho);
                    }

                }
            }
        }
        if(MOSTRA_PROCESSO) {
            arrumaTela(&tela);
            arrumaGrid(grid, &tela);
            arrumaAtual(atual, &tela, 50, 50, 255);
            mostraTela(&tela);
        }
        while (SDL_PollEvent(&event))
            {
                switch (event.type)
                    {
                        case SDL_QUIT:
                        exit(0);
                    }
            }
        SDL_Delay(0);
    }
    arrumaTela(&tela);
    arrumaGrid(grid, &tela);
    arrumaCaminho(resposta, &tela);
    mostraTela(&tela);
    while(1)
    while (SDL_PollEvent(&event))
        {
            switch (event.type)
                {
                    case SDL_QUIT:
                        FILE* arquivo_res = fopen("resolucao.bin", "wb");
                        int resolucao = SCREEN_LENGTH;
                        fwrite(&resolucao, sizeof(int), 1, arquivo_res);
                        fclose(arquivo_res);
                        FILE* arquivo_ini = fopen("origem.bin", "wb");
                        fwrite(&origemX, sizeof(int), 1, arquivo_ini);
                        fwrite(&origemY, sizeof(int), 1, arquivo_ini);
                        fclose(arquivo_ini);
                        //ve quantos obstaculos haviam
                        int num_obstaculos = 0;
                        for(int i = 0; i < NUM_SQUARES; i++) {
                            for(int j = 0; j < NUM_SQUARES; j++) {
                                if(grid[i][j]->peso == PESO_OBSTACULO)
                                    num_obstaculos++;
                            }
                        }
                        FILE* arquivo_obs = fopen("obstaculos.bin", "wb");
                        fwrite(&num_obstaculos, sizeof(int), 1, arquivo_obs);
                        //agora escreve cada um deles
                        for(int i = 0; i < NUM_SQUARES; i++) {
                            for(int j = 0; j < NUM_SQUARES; j++) {
                                if(grid[i][j]->peso == PESO_OBSTACULO) {
                                    SDL_Rect novo;
                                    novo.h = SCREEN_LENGTH / NUM_SQUARES;
                                    novo.w = SCREEN_LENGTH / NUM_SQUARES;
                                    novo.x = IJparaXY(j) - ((SCREEN_LENGTH / NUM_SQUARES)/2);
                                    novo.y = IJparaXY(i) - ((SCREEN_LENGTH / NUM_SQUARES)/2);
                                    fwrite(&novo, sizeof(SDL_Rect), 1, arquivo_obs);
                                }
                            }
                        }
                        fclose(arquivo_obs);

                        FILE* arquivo_dest = fopen("destinos.bin", "wb");
                        ITEM* aux = resposta; //so pra percorrer o caminho resposta e ver quantos objetivos há
                        int num_destinos = 0;
                        while(aux->pai != NULL) {
                            num_destinos++;
                            aux = aux->pai;
                        }
                        fwrite(&num_destinos, sizeof(int), 1, arquivo_dest);
                        aux = resposta; 
                        while(aux->pai != NULL) {
                            SDL_Point novo;
                            novo.x = aux->x;
                            novo.y = aux->y;
                            fwrite(&novo, sizeof(SDL_Point), 1, arquivo_dest);
                            aux = aux->pai;
                        }
                        fclose(arquivo_dest);
                        system("./braco");
                        FILA_apagar(&fila);
                        for(int i = 0; i < NUM_SQUARES; i++)
                            for(int j = 0; j < NUM_SQUARES; j++) 
                                item_apagar(&(grid[i][j]));
                        exit(0);
                }
        }

}