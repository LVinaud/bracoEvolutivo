#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>

//para compilar use gcc braco_mecanico.c -o braco $(sdl2-config --cflags --libs) -lSDL2 -lm
//Para a parte gráfica, utilizei a biblioteca sdl2
//Como utilizar o programa:
//Ao iniciar, o programa espera que o usuário desenhe os obstáculos, a cada dois cliques um obstaculo retangular é gerado.
//Quando terminar de desenhar os obstaculos, aperte enter para definir o ponto inicial dos braços.
//Após esse clique, o programa começa.
//Agora, basta definir seu objetivo durante a execução do programa, esse objetivo pode ser modificado com um clique.
//A tecla Q causa um genocídio, inclusive do melhor histórico.
//A tecla Z aumenta a mutação, enquanto a X diminui. O valor mínimo da mutação é 1.
//Mude o define MOSTRA_POPULACAO para 0 caso nao queria ver todos os individuos.

#define DELAY 1 //quantos milissegundos de delay entre uma iteração e outra.
#define SCREEN_WIDTH 1100   //Constantes relacionadas ao tamanho da janela
#define SCREEN_HEIGHT 650
#define MOSTRA_POPULACAO 1 //Escolha se quer ver todos os individuos, ou apenas o melhor.
#define GERACOES_REPETIDAS 200 //Quantas gerações com o melhor repetido para que a mutação varie
#define NUM_JUNTAS 30   //Em quantas juntas o comprimento total será dividido, é também o tamanho do vetor de angulos
#define COMPRIMENTO_TOTAL 1100  //Comprimento do braço
#define NUM_INDIVIDUOS 5  //Quantos individuos serão gerados e examinados por iteração
#define TAXA_MUTACAO 1  //Taxa de mutação inicial, durante a execução será na prática alterada pelo usuário
#define DISTANCIA_MIN 16

typedef struct {
	SDL_Renderer *renderer;
	SDL_Window *window;
} Tela; //struct relacionada ao sdl.

typedef struct {
    float angulos[NUM_JUNTAS]; //vetor de angulos de cada uma das juntas
    float pontuacao; //pontuacao a ser definida pela função avaliaIndividuo
    SDL_Point inicio; //ponto inicial do individuo, é definido pelo usuário e igual para todos
    float menorDistancia; //a distancia da junta mais próxima a algum obstaculo(!= pontuação)
} Individuo;//o individuo a ser analisado

void arrumaTela(Tela* tela); //limpa a tela
void inicializaSDL(Tela* tela); //inicializa o renderizador e a janela
Individuo* individuoAleatorio(SDL_Point inicio); //gera um individuo aleatorio
void lerPonto(Tela* tela, SDL_Point* objetivo, SDL_Rect** obstaculos, int numero_obstaculos); //le um clique do usuario até que um ponto que não colida com os obstaculos seja clicado
void lerObstaculos(Tela* tela, SDL_Rect*** obstaculos, int* quantidade_obstaculos); //le os obstaculos, para de ler no pressionar da tecla enter
void copiaIndividuo(Individuo* destino, Individuo* origem); // copia todos os atributos de um individuo
int lerInput(int* mutacao, SDL_Point* objetivo); //le possiveis inputs relacionados a fechar o progama, alterar a mutacao ou alterar o destino
void desenhaCirculo(Tela* tela, SDL_Point ponto, int raio);
void mostraIndividuo(Tela* tela, Individuo* individuo, int r, int g, int b); //desenha um braço na tela
void mutaIndividuo(Individuo* individuo, int mutacao);//muta um numero definido na chamada da função de angulos do individuo
void avaliaIndividuo(Individuo* individuo, SDL_Rect** obstaculos, int num_obs, SDL_Point objetivo);//funcao explicada mais detalhadamente na sua definição
float distancia(SDL_Point a, SDL_Point b);//calcula a distancia entre dois pontos
void mostraTela(Tela* tela); //simplesmente apresenta o que foi desenhado até agora para o usuário
float distanciaPontoRect(SDL_Point ponto, SDL_Rect rect); //pega o ponto mais proximo do retangulo e calcula a distancia a um ponto
int min(int a, int b);
int max(int a, int b);

int min(int a, int b) {return (a < b) ? a : b;}
int max(int a, int b) {return (a > b) ? a : b;}

float distanciaPontoRect(SDL_Point ponto, SDL_Rect rect) {
    //acha o x e y do ponto pertencente ao retângulo mais próximo do ponto que quero analisar.
    //vale lembrar que o retangulo em SDL tem 4 parametros (x do ponto superior esquerdo, y do ponto superior esquerdo, altura e comprimento)
    //para achar a coordenada mais proxima de um ponto no retangulo, basta comparar com o seu ponto superior esquerdo;
    //se o x do ponto for menor do que o do ponto superior esquerdo, então o x mais próximo é o do ponto superior esquerdo;
    //se o x do ponto for maior do que o do ponto superior esquerdo + comprimento, o x mais perto é o do ponto superior esquerdo + comprimento;
    // caso contrário, o x mais perto é o do próprio ponto analisado.
    //a mesma lógica segue para y.
    int xPerto = (ponto.x < rect.x) ? rect.x : (ponto.x > rect.x + rect.w) ? rect.x + rect.w : ponto.x;
    //para achar o x dele, é simples. caso o x do ponto seja menor do que o
    int yPerto = (ponto.y < rect.y) ? rect.y : (ponto.y > rect.y + rect.h) ? rect.y + rect.h : ponto.y;
    SDL_Point aux = {xPerto, yPerto};
    //depois é só calcular a distancia entre eles
    return distancia(aux, ponto);
}

void mostraTela(Tela* tela) {
    SDL_RenderPresent(tela->renderer);
}

void arrumaTela(Tela* tela) {
    SDL_SetRenderDrawColor(tela->renderer, 255, 255, 255, 255);
    SDL_RenderClear(tela->renderer);
}

float distancia(SDL_Point a, SDL_Point b) {
    //para evitar divisões por 0, a função retorna 1 se a distancia for menor que isso;
    return max(sqrt(pow((a.x - b.x), 2) + pow((a.y - b.y), 2)), 1);
}

void avaliaIndividuo(Individuo* individuo, SDL_Rect** obstaculos, int num_obs, SDL_Point objetivo) {
    //A pontuação do individuo leva primeiro em conta a distancia do seu ponto final ao objetivo.
    //Se essa distancia for menor do que um valor predeterminado, passa a analisar também sua distancia aos obstaculos.
    float pontuacao = 0;
    float menor_distancia = 1000000;
    SDL_Point aux;
    int x_atual = individuo->inicio.x, y_atual = individuo->inicio.y;
    for(int i = 0; i < NUM_JUNTAS; i++) {
        //a cada iteração o x e y atuais do braço se atualizam
        x_atual += (cos(individuo->angulos[i]) * (COMPRIMENTO_TOTAL/NUM_JUNTAS));
        y_atual += (sin(individuo->angulos[i]) * (COMPRIMENTO_TOTAL/NUM_JUNTAS));
        aux.x = x_atual;
        aux.y = y_atual;
        //em toda iteração checo se o braço entrou em algum obstaculo, usando uma função que detecta essa colisao do proprio SDL
        //caso tenha entrado, é dada uma pontuação negativa e a função retorna. Quero evitar essa situação ao máximo.
        //aproveito também para já calcular a menor distância que acontece entre uma junta e um obstaculo.
        for(int j = 0; j < num_obs; j++) {
            if(SDL_EnclosePoints(&aux, 1, obstaculos[j], NULL)) {individuo->pontuacao = -99999; return;}
            menor_distancia = min(menor_distancia, distanciaPontoRect(aux, *(obstaculos[j])));
        }
    }
    individuo->menorDistancia = menor_distancia;
    SDL_Point final = {x_atual, y_atual};
    pontuacao = (1.0) / (distancia(objetivo, final));
    if(distancia(objetivo, final) < DISTANCIA_MIN) 
        pontuacao += menor_distancia;
    //Por fim, o individuo recebe a pontuação calculada.
    individuo->pontuacao = pontuacao;
}

void mutaIndividuo(Individuo* individuo, int mutacao) {
    for(int i = 0; i < mutacao; i++) {
        individuo->angulos[(rand()%NUM_JUNTAS)] = ((float)(rand() % 6283184))/(1000000.0);
    }
}

void mostraIndividuo(Tela* tela, Individuo* individuo, int r, int g, int b) {
    int x_atual = individuo->inicio.x, y_atual = individuo->inicio.y;
    for(int i = 0; i < NUM_JUNTAS; i++) {
        SDL_SetRenderDrawColor(tela->renderer, r, g, b, 255);
        SDL_RenderDrawLine(tela->renderer, x_atual, y_atual, x_atual + (cos(individuo->angulos[i]) * (COMPRIMENTO_TOTAL/NUM_JUNTAS)), y_atual + (sin(individuo->angulos[i]) * (COMPRIMENTO_TOTAL/NUM_JUNTAS)));
        x_atual += (cos(individuo->angulos[i]) * (COMPRIMENTO_TOTAL/NUM_JUNTAS));
        y_atual += (sin(individuo->angulos[i]) * (COMPRIMENTO_TOTAL/NUM_JUNTAS));
        SDL_Point aux = {x_atual, y_atual};
        SDL_SetRenderDrawColor(tela->renderer, 0, 0, 0, 255);
        if(i != NUM_JUNTAS-1) desenhaCirculo(tela, aux, 3);
    }
}

int lerInput(int* mutacao, SDL_Point* objetivo)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				exit(0);
				break;
            
            case SDL_KEYDOWN:
                SDL_Keycode keyPressed = event.key.keysym.sym;
                if(keyPressed == SDLK_z) *mutacao += 1;
                else if ((keyPressed == SDLK_x) && (*mutacao > 1)) *mutacao -= 1;
                else if (keyPressed == SDLK_q) return 2;
                break;
            case SDL_MOUSEBUTTONDOWN:
                objetivo->x = event.button.x;
                objetivo->y = event.button.y;
                return 1;
			default:
				break;
		}
	}
    return 0;
}

void inicializaSDL(Tela* tela)
{
    srand((unsigned)time(NULL));
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		exit(1);
	}

	tela->window = SDL_CreateWindow("Braço Mecânico", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

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

void lerObstaculos(Tela* tela, SDL_Rect*** obstaculos, int* quantidade_obstaculos)
{
    SDL_Event event;
    int pronto = 0;
    SDL_Point primeiro, segundo;
    int qual = 0;

    while(!pronto)
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				exit(0);
				break;
            case SDL_KEYDOWN:
                SDL_Keycode keyPressed = event.key.keysym.sym;
                if(keyPressed == SDLK_RETURN) 
                {
                    pronto = 1;
                    SDL_SetRenderDrawColor(tela->renderer, 0, 255, 0, 255);
                    SDL_RenderClear(tela->renderer);
                    mostraTela(tela);
                    SDL_Delay(200);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                switch (qual)
                {
                    case 0: 
                        primeiro.x = event.button.x;
                        primeiro.y = event.button.y;
                        qual = 1;
                        break;    
                    case 1:
                        segundo.x = event.button.x;
                        segundo.y = event.button.y;
                        (*quantidade_obstaculos)++;
                        *obstaculos = (SDL_Rect**) realloc(*obstaculos, (*quantidade_obstaculos) * sizeof(SDL_Rect*));
                        if(*obstaculos == NULL) {printf("Erro na alocação dos obstáculos.\n"); exit(1);}
                        (*obstaculos)[*quantidade_obstaculos-1] = malloc(sizeof(SDL_Rect));
                        (*obstaculos)[*quantidade_obstaculos-1]->x = min(primeiro.x, segundo.x);
                        (*obstaculos)[*quantidade_obstaculos-1]->y = min(primeiro.y, segundo.y);
                        (*obstaculos)[*quantidade_obstaculos-1]->h = abs(primeiro.y - segundo.y);
                        (*obstaculos)[*quantidade_obstaculos-1]->w = abs(primeiro.x - segundo.x);
                        qual = 0;
                        break;
                    default:
                        break;
                }
                break;
			default:
                arrumaTela(tela);
                SDL_SetRenderDrawColor(tela->renderer, 0, 0, 0, 255);
                for(int i = 0; i < *quantidade_obstaculos; i++) {
                    SDL_RenderFillRect(tela->renderer, (*obstaculos)[i]);
                }
                mostraTela(tela);
				break;
		}
	}
}

void lerPonto(Tela* tela, SDL_Point* ponto, SDL_Rect** obstaculos, int quantidade_obstaculos)
{
    SDL_Event event;
    int pronto = 0;
    while(!pronto)
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				exit(0);
				break;
            case SDL_MOUSEBUTTONDOWN:
                int colisao = 0;
                SDL_Point aux = {event.button.x, event.button.y};
                for(int i = 0; i < quantidade_obstaculos; i++) {
                    colisao = SDL_EnclosePoints(&aux, 1, obstaculos[i], NULL); //checa se a luz que está sendo inserida colide com algum obstaculo.
                    if(colisao == SDL_TRUE) break;
                }
                if(colisao != SDL_TRUE) 
                {
                    *ponto = aux;
                    pronto = 1;
                }
                break;
			default:
                arrumaTela(tela);
                SDL_SetRenderDrawColor(tela->renderer, 0, 0, 0, 255);
                for(int i = 0; i < quantidade_obstaculos; i++) {
                    SDL_RenderFillRect(tela->renderer, obstaculos[i]);
                }
                mostraTela(tela);
				break;
		}
	}
}

Individuo* individuoAleatorio(SDL_Point inicio) {
    Individuo* novo = (Individuo*) malloc(sizeof(Individuo));
    for(int i = 0; i < NUM_JUNTAS; i++) {
        novo->angulos[i] = ((float)(rand() % 6283184))/(1000000.0);
    }
    novo->pontuacao = 0;
    novo->inicio.x = inicio.x;
    novo->inicio.y = inicio.y;
    return novo;
}

void copiaIndividuo(Individuo* destino, Individuo* origem) {
    destino->pontuacao = origem->pontuacao;
    destino->inicio = origem->inicio;
    destino->menorDistancia = origem->menorDistancia;
    for(int i = 0; i < NUM_JUNTAS; i++) {
        destino->angulos[i] = origem->angulos[i];
    }
}

void desenhaCirculo(Tela* tela, SDL_Point ponto, int raio) {
    for (int y = -raio; y <= raio; ++y) {
        for (int x = -raio; x <= raio; ++x) {
            if (x * x + y * y <= raio * raio) {
                SDL_RenderDrawPoint(tela->renderer, ponto.x + x, ponto.y + y);
            }
        }
    }
}

int main() {

    int taxa_mut = TAXA_MUTACAO;
    //inicializa a tela e abre a janela do programa.

    Tela tela;
    inicializaSDL(&tela);
    //inicializa a quantidade de obstáculos e o vetor de obstáculos(ponteiros de retângulos)

    int quantidade_obstaculos = 0;
    SDL_Rect** obstaculos = NULL;
    lerObstaculos(&tela, &obstaculos, &quantidade_obstaculos);

    //le o ponto inicial (onde os braços começam)
    SDL_Point inicio;
    lerPonto(&tela, &inicio, obstaculos, quantidade_obstaculos);

    //le o objetivo inicial, ele poderá ser alterado depois
    SDL_Point objetivo;
    lerPonto(&tela, &objetivo, obstaculos, quantidade_obstaculos);

    //inicializa a população com individuos aleatorios partindo do inicio definido.
    Individuo* populacao[NUM_INDIVIDUOS];
    for(int i = 0; i < NUM_INDIVIDUOS; i++) {
        populacao[i] = individuoAleatorio(inicio);
    }

    //inicializa o melhor de todos e já avalia ele, para que nao fique com uma pontuação aleatória.
    Individuo* melhorHistorico = (Individuo*) malloc(sizeof(Individuo));
    avaliaIndividuo(populacao[0], obstaculos, quantidade_obstaculos, objetivo);
    copiaIndividuo(melhorHistorico, populacao[0]);
    int geracoesRepetidas = 0;
    
    while(1) {
        //checa todos os inputs do usuario, inclusive o de fechar o programa
        int input = lerInput(&taxa_mut, &objetivo);
        if(input == 1) {avaliaIndividuo(melhorHistorico, obstaculos, quantidade_obstaculos, objetivo);}
        else if(input == 2) {
            for(int i = 0; i < NUM_INDIVIDUOS; i++) {
                populacao[i] = individuoAleatorio(inicio);
            }
            copiaIndividuo(melhorHistorico, populacao[0]);
        }

        //limpa a tela
        arrumaTela(&tela);

        //avalia todos os individuos
        for(int i = 0; i < NUM_INDIVIDUOS; i++) {
            avaliaIndividuo(populacao[i], obstaculos, quantidade_obstaculos, objetivo);
        }

        //seleciona o melhor de todos
        geracoesRepetidas++;
        for(int i = 0; i < NUM_INDIVIDUOS; i++) {
            if(populacao[i]->pontuacao > melhorHistorico->pontuacao) {
                copiaIndividuo(melhorHistorico, populacao[i]);
                geracoesRepetidas = 0;
                taxa_mut = TAXA_MUTACAO;
            }
        }

        //mutacao variavel automatica
        if(geracoesRepetidas > GERACOES_REPETIDAS) {
            if(taxa_mut < (NUM_JUNTAS * 0.5))
                taxa_mut++;
            else taxa_mut = TAXA_MUTACAO;
            geracoesRepetidas = 0;
        }

        //desenha todos os individuos com uma cor definida(azul perto do ciano)
        if(MOSTRA_POPULACAO)    
            for(int i = 0; i < NUM_INDIVIDUOS; i++) 
                mostraIndividuo(&tela, populacao[i], 19, 122, 127);
        
        //desenha o melhor de todos com uma cor vermelha
        mostraIndividuo(&tela, melhorHistorico, 255, 36, 0);

        //printf("A menor distância do melhor de todos: %f\n", melhorHistorico->menorDistancia);
        //desenha os obstáculos
        SDL_SetRenderDrawColor(tela.renderer, 0, 0, 0, 255);
        for(int i = 0; i < quantidade_obstaculos; i++) {
            SDL_RenderFillRect(tela.renderer, obstaculos[i]);
        }

        //desenha o objetivo de amarelo
        SDL_SetRenderDrawColor(tela.renderer, 255, 255, 0, 255);
        desenhaCirculo(&tela, objetivo, 5);

        //apresenta a tela desenhada ao usuário
        SDL_RenderPresent(tela.renderer);

        //muta toda a população de acordo com a taxa de mutacao atual
        for(int i = 0; i < NUM_INDIVIDUOS; i++) {
            copiaIndividuo(populacao[i], melhorHistorico);
            mutaIndividuo(populacao[i], taxa_mut);
        }
        //printf("Taxa de mutação atual: %d gerações repetidas: %d\n", taxa_mut, geracoesRepetidas);
        SDL_Delay(DELAY);
    }
}
