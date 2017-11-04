#include <stdio.h>
#include <stdlib.h>

#define TAM_MAX 999
#define VALOR_INVALIDO -9999




typedef struct node {
    struct node *parent;
    int x, y;
    float f, g, h;
} node;

node* create_node(){
    node* a = (node*) malloc(sizeof(node));
    
    a->x = 0;
    a->y = 0;
    
    a->f = 0;
    a->g = 0;
    a->h = 0;
    
    a->parent = NULL;

    return a;
}

node* new_node(int x, int y, double g, double h){
    node* a = (node*) malloc(sizeof(node));
    
    a->x = x;
    a->y = y;
    
    a->g = g;
    a->h = h;

    a->f = a->g + a->h;

    a->parent = NULL;
    
    return a;
}



typedef struct lista{
    node* nod; // elemento da matriz usado para a*
    struct lista* next; //ponteiro p prox elemento
} lista;



void push_begin(lista ** head, node* val) {

    if(head == NULL){
        lista * new_nod;
        new_nod = (lista*) malloc(sizeof(lista));

        new_nod->nod = val;
        new_nod->next = NULL;
    }else{
        lista * new_nod;
        new_nod = (lista*) malloc(sizeof(lista));

        new_nod->nod = val;
        new_nod->next = *head;
        *head = new_nod;
    }

    
}



void print_list(lista * head) {
    lista * current = head;

    while (current != NULL) {
        printf("(x: %d  y: %d)  - f = %f,  g = %f, h = %f\n", current->nod->x, current->nod->y, current->nod->f , current->nod->g, current->nod->h);
        current = current->next;
    }
}


// retorna o no que possui a pos x,y 
//retorna null caso n exista
node* isOnTheList(lista* head, int x, int y) {
    lista* current = head;

    if(head == NULL)
        return NULL;

    long i = 0;
    while (current != NULL) {
        if(current->nod->x == x && current->nod->y == y)
            return current->nod;

        i++;
        current = current->next;
    }

    return NULL;
}


node* pop(lista ** head) {
    node* retval = NULL;
    lista * next_node = NULL;

    if (*head == NULL) {
        return NULL;
    }

    next_node = (*head)->next;
    retval = (*head)->nod;
    free(*head);
    *head = next_node;

    return retval;
}

node* remove_by_index(lista ** head, int n) {
    int i = 0;
    node* retval = NULL;
    lista * current = *head;
    lista * temp_node = NULL;

    if(*head == NULL){
        puts("remove_by_index: list null");
        return NULL;
    }

    if (n == 0) {
        return pop(head);
    }

    for (int i = 0; i < n-1; i++) {
        if (current->next == NULL) {
            return retval;
        }
        current = current->next;
    }

    temp_node = current->next;
    retval = temp_node->nod;
    current->next = temp_node->next;
    free(temp_node);

    return retval;

}

node* remove_min_f(lista** head ){
    if(head == NULL){
        puts("remove min f: head null");
        return NULL;
    }
    
    lista * current = *head;

    float min = current->nod->f;
    int min_index = 0;
    int index = 0;

    while (current != NULL) {
        
        if(current->nod->f < min){
            min = current->nod->f;
            min_index = index;
        }
        
        index++;
        current = current->next;
    }

    //printf("remove_min_f: minimo no indice %d\n", min_index);
    
    return remove_by_index(head, min_index);
}




//distancia de manhatan
double H(int x, int y, int destx, int desty){
    double dist = abs(x - destx) + abs(y-desty);
    return  dist;
}


//retorna os sucessores de todas as direcoes
//exceto diagonais
lista* gerar_sucessores(node* min, char** map, int map_dim, char obstacle){

    
    //gerar sucessores
    lista* sucessores = NULL;

    if(map[min->y][min->x] == obstacle)
        return sucessores;

    // gerar sucessor para esquerda
    if( min->x > 0){

        if(map[min->y][min->x - 1] != obstacle){
            double h = H(min->x, min->y, min->x-1, min->y);
            node* n = new_node(min->x-1, min->y, min->g + 1, h );
            n->parent = min;
            push_begin(&sucessores, n);
        }         
    }

    //gerar sucessor da direita
    if(min->x < map_dim - 1){
        if(map[min->y][min->x + 1] != obstacle){
            double h = H(min->x, min->y, min->x+1, min->y);
            node* n = new_node(min->x+1, min->y, min->g + 1, h );
            n->parent = min;
            push_begin(&sucessores, n);
        }         
    }

    // gerar sucessor para cima
    if( min->y > 0){
        if(map[min->y - 1][min->x] != obstacle){
            double h = H(min->x, min->y - 1, min->x, min->y);
            node* n = new_node(min->x, min->y - 1, min->g + 1, h );
            n->parent = min;
            push_begin(&sucessores, n);
        }
    }

    //gerar sucessor para baixo
    if(min->y < map_dim - 1){
        if(map[min->y + 1][min->x] != obstacle){
            double h = H(min->x, min->y + 1, min->x, min->y);
            node* n = new_node(min->x, min->y + 1, min->g + 1, h );
            n->parent = min;
            push_begin(&sucessores, n);
        }
    }

    return sucessores;
}




// recebe um mapa e sua dimensao, caracter considerado obstaculo dentro do mapa, posicao da origem e do destino
node* astar(char** map, int map_dim, char obstacle, int x, int y, int destx, int desty){
    
    lista* openList = NULL;
    lista* closedList = NULL;   

    node* origem =  new_node(x,y,0,0); // create_node();

    push_begin(&openList, origem);


    //enquanto lista nao estiver vazia
    while( openList != NULL){
        //mover no atual para lista fechada
        node* min = remove_min_f(&openList);
       
        push_begin(&closedList, min);

        lista* sucessores = gerar_sucessores(min, map, map_dim, obstacle);
    
        lista* atual = sucessores;
        while(atual != NULL){
            //verificar se sucessor esta na lista fechada
            node* n = isOnTheList(closedList, atual->nod->x, atual->nod->y);

           
            // se n nao estiver na lista fechada
            if(n == NULL){
                
                n = isOnTheList(openList, atual->nod->x, atual->nod->y);
                if( n == NULL){
                    atual->nod->parent = min;
                    push_begin(&openList, atual->nod);    

                }else{
                    
                    if(atual->nod->f < n->f){
                        n->f = atual->nod->f;
                        n->g = atual->nod->g;
                        n->h = atual->nod->h;

                        n->parent = atual->nod->parent;
                    }
                }              
            }

            if(atual->nod->x == destx && atual->nod->y == desty){
                //retornar o caminho
                return atual->nod;
            }

            atual = atual->next;
        }

    }

    puts("NAO ENCONTROU CAMINHO");
    return NULL;
    
}


lista* build_path( node* n){

    node* it = n;
    lista* path = NULL;

    while( it != NULL){
        push_begin(&path, it);
        it= it->parent;
    }


    return path;

}
