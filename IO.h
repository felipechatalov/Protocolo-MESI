#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "constants.h"


// imprime um slot da memoria cache
void printCacheSlot(CacheSlot* slot){
    printf("[ ");
    for (int j = 0; j < BLOCK_SIZE; j++){
        printf("%d ", slot->values[j].value);
    }
    printf("]");
}
// imprime a memoria cache
int printCache(Cache cache){
    printf("\nCache");

    for (int i = 0; i < CACHE_SIZE; i++){
        
        printf("\n[ ");
        
        for (int j = 0; j < BLOCK_SIZE; j++){
            printf("%d ", cache.slots[i].values[j].value);
        }
        
        char s = ' ';
        switch (cache.slots[i].state){
            case MODFIED:
                s = 'M';
                break;
            case EXCLUSIVE:
                s = 'E';
                break;
            case SHARED:
                s = 'S';
                break;
            case INVALID:
                s = 'I';
                break;
            case NUL:
                s = '*';
                break;
        }
        printf("]  %c", s);
    }
    return 1;
}

//  cache 1
// 0 x x x x  y y y y y y y y
// 1 x x x x  y y y y y y y y
// 2 x x x x  y y y y y y y y
// 3 x x x x  y y y y y y y y
// 4 cache 2  y y y y y y y y
// 5 x x x x  y y y y y y y y
// 6 x x x x  y y y y y y y y
// 7 x x x x  y y y y y y y y 
// 8 x x x x  y y y y y y y y
// 9          y y y y y y y y


void printRAMandAllCaches(int mram[], Cache caches[], int numberOfCaches){
    int rows = 8;
    int count = 0;
    int r = (MRAM_SIZE/rows)+1;
    int c = CACHE_SIZE*numberOfCaches + numberOfCaches;
    int higher = r;
    if (c > higher){
        higher = c;
    }

    printf("\nCaches\t\t\t\t\tRAM %d", higher);
    for (int i = 0; i < higher; i++){
        if (i < c){
            // printf("\n%d, %d, %d", (i+i/CACHE_SIZE), i, count);
            if (i == 0 || i == count*CACHE_SIZE+count){
                printf("\nCache %d \t\t", i/CACHE_SIZE);
                count++;
            }
            else{
                printf("\n[ ");
                for (int j = 0; j < BLOCK_SIZE; j++){
                    // printf("%d ", cache[i].values[j].value);
                    printf("%d ", caches[(i-count)/CACHE_SIZE].slots[(i-count)%CACHE_SIZE].values[j].value);
                }
                char s = ' ';
                switch (caches[(i-count)/CACHE_SIZE].slots[(i-count)%CACHE_SIZE].state){
                    case MODFIED:
                        s = 'M';
                        break;
                    case EXCLUSIVE:
                        s = 'E';
                        break;
                    case SHARED:
                        s = 'S';
                        break;
                    case INVALID:
                        s = 'I';
                        break;
                    case NUL:
                        s = '*';
                        break;
                }
                // printf("]%c %d %d %d\t", s, (i-count)/CACHE_SIZE, (i-count)%CACHE_SIZE, i-count);
                printf("]  %c\t", s);

            }
        }
        else{
            printf("\n\t\t\t\t");
        }

        if (i < MRAM_SIZE/rows){
            for (int k = 0; k < rows; k++){
                int t = (i+(k*MRAM_SIZE/rows));
                if (t < MRAM_SIZE){
                    printf("[%4d] = %d  ", t, mram[t]);
                }
            }
        }

    }
}
int printInputObjArray(InputObj testRAM[], int n){
    printf("\n");
    for (int i = 0; i < n; i++){
        printf("%d", testRAM[i].value);
        if (testRAM[i].update == 1){
            printf("* ");
        }
        else printf(" ");
    }
    printf("\n");
    return 1;
}
// imprime a sequencia de acesso do LRU
void printLRU(int lru[]){
    printf("\nLRU:\n");
    printf("Proximo Valor -->  [ ");
    for (int i = 0; i < CACHE_SIZE; i++){
        printf("%d ", lru[i]);
    }
    printf("]");
}
// imprime os valores da memoria RAM
void printMRAM(int mram[]){
    for (int i = 0; i < MRAM_SIZE; i++){
        printf("\n[%2d] = %d", i, mram[i]);
    }
}
int checkInput(char* buffer, int i){
    // 0 = 48
    // 9 = 57
    for (int j = 0; j < i-1; j++){
        if (buffer[j] < 48 || buffer[j] > 57){
            return 0;
        }
    
    }
    if ((buffer[i-1] < 48 || buffer[i-1] > 57) && buffer[i-1] != 42){
        return 0;
    }
    
    return 1;
}
// cuida do input do usuario para a posicao a ser acessada da memoria RAM
int handleUserInput(int* update, int ENTRY_LIMIT){

    // aloca um buffer de 16 char's
    // e um ponteiro que ira apontar para a primeira posicao de 'buffer'    
    char buffer[16]; 
    char *p;
    // 'c' eh usado para capturar o input do usuario
    int c;
    // define todas as posicoes do vetor 'buffer' como '\0'
    memset(buffer, 0, sizeof(buffer));
    printf("\n$.. ");
    // 'i' conta quantos numeros o usuario digitou e 'number' eh o valor que sera retornado
    int i, number, retry = 1;
    // enquanto o usuario nao digitar um numero valido
    while (retry){
        i = 0;
        p = buffer;
        while((c = getchar()) != '\n' && c != EOF){
            // copia o input do usuario para o buffer
            *p++ = (char)c;
            
            i++;
        }
        // coloca o '\0' no final do buffer e transfoma o numero em inteiro        
        *p++ = 0;
        // printf("\n%s", buffer);
        number = atoi(buffer);

        if (strcmp(buffer, "quit") == 0){
            printf("\nSaindo do programa");
            return -1;
        }
        // verifica se a primeira posicao do buffer se encontra entre 0 e 9 em ascii
        // e se o numero é posistivo e menor que o tamanho da memoria RAM
        // 0 -> 48    9 -> 57
        // TODO: verificar todos os indices de buffer
        else if (i == 0 || !checkInput(buffer, i) || (number < 0 || number > ENTRY_LIMIT-1)){

            // caso encontre algum erro ele deixa 'retry' como 0 para que o usuario tente novamente
            retry = 1;
            *update = 0;
            printf("\nIndex invalido, digite um index novamente ou 'quit' para sair");
            printf("\n$.. ");
        }
        else{
            retry = 0;
        }
        

    }
    *update = 0;
    // caso a ultima posicao do 'buffer' seja um '*' o endereco acessado deve ser atualizado
    if (buffer[i-1] == 42){
        *update = 1;
        buffer[i-1] = 0;
    }
    return number;
}