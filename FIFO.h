#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>



// 2 structs usados para a criacao da FIFO, 
typedef struct Item {
    int value;
    struct Item* Next;
}Item;
typedef struct Fila {
    int lenght;
    Item* First;
    Item* Last;
}Fila;

int getRandomInt(int low, int high){
        return rand() % (high-low) + low;
}

// inicializa um item com valor 0 e next com NULL
Item* initializeItem(Item* i){
    i -> value = 0;
    i -> Next = NULL;
    return i;
}
// inicializa uma fila com tamanho 0, first e last com NULL
Fila* initializeFila(Fila* f){
    f->lenght = 0;
    f->First = NULL;
    f->Last = NULL;
    return f;
}
// inseree um item na fila 
int insertFila(Item* new, Fila* f){
    if (f->lenght == 0){
        f->First = new;
        f->Last = new;
        f->lenght++;
        return 1;
    }
    new -> Next = f -> First;
    f -> First = new;
    f -> lenght++;
    return 1;
}
// remove o ultimo item da fila
Item* RemoveLast(Fila* f){
    if (f -> lenght == 0){
        return NULL;
    }

    Item* r = f -> Last;

    if (f -> lenght == 1){
        f -> First = NULL;
        f -> Last = NULL;
        f->lenght--;
        return r;
    }

    Item* t = f -> First;
    while(t -> Next -> Next != NULL){
        t = t -> Next;
    }
    f -> Last = t;
    t -> Next = NULL;
    f->lenght--;

    return r;
}
// remove o ultimo elemento da fila, adiciona-o de volta e retorna um ponteiro para o item realocado
Item* NextInLine(Fila* fila){
    if (fila->lenght == 0){
        return NULL;
    }
    Item* t = RemoveLast(fila);
    insertFila(t, fila);
    return t;
}
// imprime todos os elementos da fila
int printFila(Fila* f){
    if (f-> First == NULL){
        printf("\nNo items in list");
        return 0;
    }
    Item* t = f -> First;
    printf("\n");
    while (t -> Next != NULL){
        printf(" %d |", t -> value);
        t = t -> Next;
    }
    printf(" %d", t -> value);
    printf("  <-- Proximo Valor");
    
    return 1;
}
// retorna uma fila de tamanho CACHE_SIZE com valores aleatorios nao repetidos, para mapear o acesso a memoria cache
Fila* randomizeFila(Fila* fila){

    int iter = 0;
    // enquanto a fila for menor que CACHE_SIZE
    // adiciona valores aleatorios nao repetidos
    while (iter < CACHE_SIZE){
        // aloca uma celula na memoria com valor aleatorio
        Item* cell = malloc( sizeof(Item) );
        initializeItem(cell);
        int n = getRandomInt(0, CACHE_SIZE);
        
        // percore pela fila verificando se o valor ja existe
        // caso exista ele nao eh adicionado e geramos outro valor aleatorio
        int curr = 0;
        while (curr < iter){
            if (NextInLine(fila)->value == n){
                n = getRandomInt(0, CACHE_SIZE);
                curr = -1;
            }            
            curr++;
        }

        // por fim se o valor nao existir, ele eh adicionado
        cell->value = n;
        insertFila(cell, fila);

        iter++;
    }
    return fila;
}
// retorna um valor aleatorio entre low e high
Fila* OrderFIFO(Fila* fila){
    int iter = 0;
    while (iter < CACHE_SIZE){
        Item* cell = malloc( sizeof(Item) );
        initializeItem(cell);
        cell->value = iter;
        insertFila(cell, fila);
        iter++;
    }
    return fila;
}
void initializeListOfFIFO(Fila* list, int n){
    for (int i = 0; i < n; i++){
        initializeFila(&list[i]);
    }
}
void OrderListOfFIFO(Fila* list, int n){
    for (int i = 0; i < n; i++){
        OrderFIFO(&list[i]);
    }
}
void printAllFIFO(Fila* list, int n){
    for (int i = 0; i < n; i++){
        printf("\nFila %d: ", i);
        printFila(&list[i]);
    }
}