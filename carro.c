#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "astar.c"


#define true 1
#define false 0

#define DIM 24
#define OBSTACULO '*'
#define NADA ' '
#define NAO_EXPLORADO '?'
#define AVATAR 'C'

#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'

void clrscr()
{
    system("@cls||clear");
}

int estaNoMapa(int x, int y){

    //printf("estano mapa? x: %d   y: %d\n",x,y);
    if(x < 0 || x >= DIM)
        return false;

    if(y < 0 || y >= DIM)
        return false;

    return true;

}


char** mapa_real;
char** mapa_scaneado;



typedef struct {
    int x,y;

    int range_visao;
    //int direcao;
} CARRO;


//submatriz (menor trexo do mapa)
typedef struct {

    int id;

    int tam_setor;

    int min_i;
    int max_i;

    int min_j;
    int max_j;

    int pontosNaoExplorados;
    int pontosObstaculos;
    int pontosVagos;

} SETOR;

SETOR* criar_setor(int id, int tam, int min_i, int max_i, int min_j, int max_j);


void iniciar_mapas( );
void exibir_mapa(char** mapa);
void exibir_mapa_scaneado(char** mapa, CARRO* carro);
void scanear(char** mapa_real, char** mapa_scaneado, CARRO* carro);

CARRO* criar_carro(int x, int y, int range);
void exibir_carro(CARRO* carro);
char obter_direcao(CARRO* carro, char** mapa_real, char** mapa_scaneado);

void exibir_setor(SETOR* setor);
SETOR** buscar_areas(char** mapa, int dim);
SETOR* qual_setor(CARRO* carro, SETOR** setores, int num_setores);
SETOR* menos_explorado(SETOR** setores, int num_setores);
//necesario pq da malloc de um array de setores a cada iteracao
void free_setores(SETOR** setores, int tam);

void andar_no_caminho(lista* path, CARRO* carro, SETOR** setores, SETOR* min_setor);

int main()
{
    srand(time(NULL));

    iniciar_mapas();


    CARRO* carro = criar_carro(DIM/2, DIM/2, 3);

    long iteracoes = 0;

    while(1){

        // clrscr();

        // printf("\n\nMapa Real\n\n");
        // exibir_mapa(mapa_real);

        scanear(mapa_real, mapa_scaneado, carro );


        //da um passo aleatorio
        for(int i = 0; i < 5; i++){
            clrscr();
            puts("DANDO PASSO ALEATORIO");
            obter_direcao(carro, mapa_real, mapa_scaneado);
            scanear(mapa_real, mapa_scaneado, carro );
            exibir_mapa_scaneado(mapa_scaneado,carro);
            getchar();

        }

        clrscr();
        printf("\n\nMapa Scaneado\n\n");
        exibir_mapa_scaneado(mapa_scaneado,carro);

        iteracoes ++;
        printf("Iteracao: %ld\n", iteracoes);

        //printa areas menos exploradas
        int dim_setores = 6;
        int qtd_setores = (DIM/dim_setores)*(DIM/dim_setores);
        SETOR** setores = buscar_areas(mapa_scaneado, dim_setores);

        //printa informacoes do carro
        exibir_carro(carro);

        puts("Carro esta no setor\n");
        SETOR* setor_do_carro = qual_setor(carro,setores,qtd_setores); 
        exibir_setor( setor_do_carro );

        //setor menos explorado
        SETOR* min_setor = menos_explorado( setores, qtd_setores );

        // centro do setor menos explorado
        int target_x = (min_setor->min_j + min_setor->max_j) / 2;
        int target_y = (min_setor->min_i + min_setor->max_i) / 2;

        puts("\nSETOR MENOS EXPLORADO\n\n");
        exibir_setor(min_setor);

        puts("Pressione Enter para Continuar...");
        getchar();

        node* n = astar(mapa_scaneado, DIM, OBSTACULO, carro->x, carro->y, target_x, target_y);

        lista* path = build_path(n);
        // print_list(path);

        //andar pelo caminho encontrado ( se possivel ** ) 

        andar_no_caminho(path, carro, setores, min_setor);

        puts("Pressione Enter para ver a proxima iteracao");
        getchar();

        // scanear(mapa_real, mapa_scaneado, carro );
        free_setores(setores,qtd_setores);



    }


    return 0;
}


void andar_no_caminho(lista* path, CARRO* carro, SETOR** setores, SETOR* min_setor){
    
    
    lista * current = path;
    while (current != NULL) {

        scanear(mapa_real, mapa_scaneado, carro );
        node* n = current->nod;

        //se puder andar continue atualizando a posicao do carro
        if(mapa_scaneado[ n->y ][ n->x ] != OBSTACULO){
            carro->x = n->x;
            carro->y = n ->y;
            scanear(mapa_real, mapa_scaneado, carro );

        // se nao saia do loop()
        }else{
            scanear(mapa_real, mapa_scaneado, carro );
            // break;
            //andar_no_caminho(path, carro, setores, min_setor);
            break;
        }

        clrscr();
        printf("\n\nMapa Scaneado\n\n");
        exibir_mapa_scaneado(mapa_scaneado,carro);

        puts("\nRUMO AO SETOR\n\n");
        exibir_setor(min_setor);

        puts("Pressione Enter para prox passo no caminho encontrado");
        getchar();
        
        current = current->next;
    }
}

void iniciar_mapas( ){

    mapa_real = (char**) malloc(sizeof(char*) * DIM);
    mapa_scaneado = (char**) malloc(sizeof(char*) * DIM);
    //zera todos os mapas
    for(int i= 0; i < DIM; i++){
        mapa_real[i] = (char*) malloc(sizeof(char) * DIM);
        mapa_scaneado[i] = (char*) malloc(sizeof(char) * DIM);
        for(int j= 0; j < DIM; j++){
            mapa_real[i][j] = NADA;
            mapa_scaneado[i][j] = NAO_EXPLORADO;
        }
    }


    //gerar terreno no mapa real


    int min_obstaculos = DIM*2;
    int num_obstaculos = min_obstaculos + (rand() % (DIM*2));

    for(int i = 0; i< num_obstaculos; i++){
        int i = rand() % DIM;
        int j = rand() % DIM;

        mapa_real[i][j] = OBSTACULO;
    }
}

void exibir_mapa(char** mapa){

    printf("\n");
    for(int i= 0; i < DIM+1; i++){
        printf("--");
    }
    printf("\n");

    for(int i= 0; i < DIM; i++){
        for(int j= 0; j < DIM; j++){
            if(j == 0 ){
               printf("| ");
            }
            printf("%c ",mapa[i][j]);
            if( j == DIM - 1){
               printf("| ");
            }
        }
        printf("\n");
    }
    for(int i= 0; i < DIM+1; i++){
        printf("--");
    }
    printf("\n");

}


CARRO* criar_carro(int x, int y, int range){
    CARRO* carro = (CARRO*) malloc(sizeof(CARRO));

    carro->x = x;
    carro->y = y;
    carro->range_visao = range;

    return carro;
}


void scanear(char** mapa_real, char** mapa_scaneado, CARRO* carro){

    //olhar carro->range_visao casas para todas as direcoes no mapa real e escrever em mapa_scaneado

    int achou_esquerda =false;
    int achou_direita = false;
    int achou_cima = false;
    int achou_baixo = false;

    //falta verificar se indice < 0 E > DIM
    for(int i = 1; i <= carro->range_visao; i++){

        //olhar para cima
        if(estaNoMapa(carro->x ,carro->y - i) && !achou_cima){
            if(mapa_real[carro->y - i][carro->x] == OBSTACULO){
                mapa_scaneado[carro->y - i][carro->x] = OBSTACULO;
                achou_cima = true;
            }else{
                mapa_scaneado[carro->y - i][carro->x] = NADA;
            }
        }

        //baixo
        if(estaNoMapa(carro->x ,carro->y + i) && !achou_baixo){
            if(mapa_real[carro->y + i][carro->x] == OBSTACULO){
                mapa_scaneado[carro->y + i][carro->x] = OBSTACULO;
                achou_baixo = true;
            }
            else{
                mapa_scaneado[carro->y + i][carro->x] = NADA;
            }
        }

        //esquerda
        if(estaNoMapa(carro->x - i ,carro->y) && !achou_esquerda){
            if(mapa_real[carro->y][carro->x - i ] == OBSTACULO){
                mapa_scaneado[carro->y][carro->x-i] = OBSTACULO;
                achou_esquerda= true;
            }else{
                mapa_scaneado[carro->y][carro->x-i] = NADA;
            }
        }

        if(estaNoMapa(carro->x + i ,carro->y) && !achou_direita){
            if(mapa_real[carro->y][carro->x+i] == OBSTACULO){
                mapa_scaneado[carro->y][carro->x+i ] = OBSTACULO;
                achou_direita = true;
            }else{
                mapa_scaneado[carro->y][carro->x+i ] = NADA;
            }
        }


    }

}

void exibir_carro(CARRO* carro){

    printf("\nCarro -- x: %d\ty: %d\trange: %d\n",carro->x, carro->y,carro->range_visao);
}


void exibir_mapa_scaneado(char** mapa, CARRO* carro){

    printf("\n");
    for(int i= 0; i < DIM+1; i++){
        printf("--");
    }
    printf("\n");

    for(int i= 0; i < DIM; i++){
        for(int j= 0; j < DIM; j++){
            if(j == 0 ){
               printf("| ");
            }

            if(carro->x == j && carro->y == i)
                printf("%c ",AVATAR);
            else
                printf("%c ",mapa[i][j]);

            if( j == DIM - 1){
               printf("| ");
            }
        }
        printf("\n");
    }
    for(int i= 0; i < DIM+1; i++){
        printf("--");
    }
    printf("\n");
}


//escolhe algum movimento para andar no mapa q ele encherga (nao eh o mapa real)
char obter_direcao(CARRO* carro, char** mapa_real, char** mapa_scaneado){

    //direcao
    int d = rand() % 4;

    char direcao;
    if(d == 0){
        direcao = UP;
    }
    else if(d == 1){
        direcao = DOWN;
    }
    else if(d == 2){
        direcao = LEFT;
    }
    else {
        direcao = RIGHT;
    }

    //printf("\n\nW (cima) A (Esquerda) S (Baixo) D (Direita) \n");
    //direcao = getc(stdin);

    //verificar se a prox posicao vai estar dentro da matriz
    switch (direcao){
        case UP:

            if(estaNoMapa(carro->x,carro->y - 1) ){
                if(  mapa_real[carro->y - 1][carro->x] != OBSTACULO ){
                    carro->y--;
                }else{
                    return obter_direcao(carro, mapa_real, mapa_scaneado);
                }
            }


            break;
        case DOWN:
            if(estaNoMapa(carro->x,carro->y + 1) ){
                if(  mapa_real[carro->y + 1][carro->x] != OBSTACULO  ){

                    carro->y++;
                }else{
                    return obter_direcao(carro, mapa_real, mapa_scaneado);
                }
            }
            break;
        case LEFT:

            if(estaNoMapa(carro->x - 1,carro->y) ){
                if(  mapa_real[carro->y][carro->x - 1] != OBSTACULO ){

                        carro->x--;
                }
                else{
                    return obter_direcao(carro, mapa_real, mapa_scaneado);
                }
            }

            break;
        case RIGHT:
            if(estaNoMapa(carro->x + 1,carro->y)){
                if( mapa_real[carro->y][carro->x + 1] != OBSTACULO  ){

                        carro->x++;
                }else{
                    return obter_direcao(carro, mapa_real, mapa_scaneado);
                }
            }

            break;
    }

    return direcao;

}

// busca area menos explorada no mapa
// para isso divide o mapa( a matriz ) n partes de dimencao dim
//dim deve ser multiplo da DIM do mapa
SETOR** buscar_areas(char** mapa, int dim){

    int tam_setor = dim;
    int nPartes = (DIM/dim)*(DIM/dim);

    SETOR** setores = (SETOR**) malloc(sizeof(SETOR*) * nPartes);

    // setando valores para o setor 0
    int min_i = 0;
    int max_i = min_i + tam_setor;

    int min_j = 0;
    int max_j = min_j+tam_setor;
    for(int setor = 0; setor < nPartes; setor++){

        setores[setor] = criar_setor(setor,tam_setor, min_i, max_i, min_j, max_j);

        for(int i = min_i; i < max_i; i++){
            for(int j = min_j; j < max_j; j++){
                if(mapa[i][j] == NAO_EXPLORADO){
                    setores[setor]->pontosNaoExplorados++;
                }else if(mapa[i][j] == OBSTACULO){
                    setores[setor]->pontosObstaculos++;
                }else{
                    setores[setor]->pontosVagos++;
                }

            }
        }

        if( min_j + tam_setor >= DIM ){

            min_j = 0;
            max_j = min_j + tam_setor;

            if(min_i + tam_setor >= DIM){
                //percorreu tudo
                break;
            }else{
                min_i += tam_setor;
                max_i = min_i + tam_setor;
            }
        }else{
            min_j += tam_setor;
            max_j = min_j + tam_setor;
        }

    }

    /* //USADO PARA DEBUGAR

    printf("\nSetores:\n\n");
    for(int i = 0; i < nPartes; i++){
       exibir_setor(setores[i]);
    }
    printf("\n");
    //*/

    return setores;

}

SETOR* menos_explorado(SETOR** setores, int num_setores){

    if(setores == NULL){
        return NULL;
    }

    SETOR* min = setores[0];
    for(int setor = 0; setor < num_setores; setor++){

        if(setores[setor]->pontosNaoExplorados > min->pontosNaoExplorados)
            min = setores[setor];
    
    }

    return min;
}

//retorna o setor q o carro se encontra
SETOR* qual_setor(CARRO* carro, SETOR** setores, int num_setores){

    for(int setor = 0; setor < num_setores; setor++){
        //exibir_setor(setores[setor]);

        if(carro->x >= setores[setor]->min_j && carro->x <= setores[setor]->max_j
           && carro->y >= setores[setor]->min_i && carro->y <= setores[setor]->max_i){

            return setores[setor];

        }

    }

    puts("NAO ACHOU SETOR");
    return NULL;

}

SETOR* criar_setor(int id, int tam, int min_i, int max_i, int min_j, int max_j){

    SETOR* s = (SETOR*) malloc(sizeof(SETOR));

    s->id = id;

    s->tam_setor = tam;
    s->min_i = min_i;
    s->max_i = max_i;
    s->min_j = min_j;
    s->max_j = max_j;

    s->pontosNaoExplorados = 0;
    s->pontosObstaculos = 0;
    s->pontosVagos = 0;

    return s;
}

void exibir_setor(SETOR* setor){
    printf("\nsetor %d :\n\tde (%d,%d) ate (%d,%d)\n\n", setor->id, setor->min_i, setor->min_j, setor->max_i, setor->max_j);
    printf("\tNao Explorados: %d\tObstaculos: %d\tVagos: %d\n\n",
               setor->pontosNaoExplorados, setor->pontosObstaculos, setor->pontosVagos);
}


void free_setores(SETOR** setores, int tam){

    for(int i = 0; i < tam; i++){
        free(setores[i]);
    }

    free(setores);
}
