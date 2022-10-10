#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "IO.h"
#include "FIFO.h"



int isInsideCache(int search, CacheSlot cache[], int* pos){
    // procura um valor(search) na memoria cache e retorna 1 caso encontre e retorna a sua pos do bloco na cache
    for (int i = 0; i < CACHE_SIZE; i++){
        for (int j = 0; j < BLOCK_SIZE; j++){
            if (cache[i].values[j].RAMaddress == search){
                printf("\nValor ja se encontra no bloco %d da cache", i);
                
                *pos = i;
                return 1;
            }
        }        
    }
    printf("\nValor nao encontrado na cache");
    return 0;
}
int isCacheFull(CacheSlot cache[]){
    // retorna 1 se a cache esta cheia
    for (int i = 0; i < CACHE_SIZE; i++){
        if (cache[i].values[0].value == NULL_VALUE_FOR_CACHE){
            // printf("\nCache nao esta cheia");
            return 0;
        }        
    }
    printf("\nCache esta cheia");
    return 1;
}
int needToUpdateBlock(CacheSlot cache[], int pos){
    // checa se o bloco precisa ser atualizado
    // cache = vetor de slots da memoria cache
    // pos = posicao do bloco na memoria cache
    for (int i = 0; i < BLOCK_SIZE; i++){
        if (cache[pos].values[i].update){
            return 1;
        }
    }
    return 0;
}
void writeBack(CacheSlot cache[], int mram[], int pos){
    // reescreve os valores atualizados para a memoria ram
    for (int i = 0; i < BLOCK_SIZE; i++){
        if (cache[pos].values[i].update){
            printf("\nAtualizando valor %d no index %d na memoria ram ", cache[pos].values[i].value, cache[pos].values[i].RAMaddress);
            
            // acessa o endereco do valor e atualiza o valor dentro da memoria RAM
            mram[cache[pos].values[i].RAMaddress] = cache[pos].values[i].value;
        }
    }
}
int* generateRandomRAMValues(int mram[], int size, int low, int high){
    // gera um vetor de valores aleatorios(repetidos ou nao) na memoria ram prontas para acesso
    int i = 0;
    while(i < size){
        mram[i] = getRandomInt(low, high);
        i++;
    }

    return mram;
}
int generateRandomRAMAccess(InputObj testRAM[], int mramsize, int n){
    // gera um vetor de acesso aleatorio na memoria ram para teste automatico
    int i = 0;
    while (i < n){
        testRAM[i].value = getRandomInt(0, mramsize);
        int r;
        r = getRandomInt(0, 10);
        // se o valor gerado for menor que 2, o acesso eh de atualizacao, assim +- 10% dos valores serao de atualizacao
        if (r < 2){
            r = 1;
        }
        else{
            r = 0;
        }
        testRAM[i].update = r;
        i++;
    }
    return 1;
}
void initializeCacheSlots(Cache* cache){
    // inicializa memoria cache com valores nulos
    for (int i = 0; i < CACHE_SIZE; i++){
        for (int j = 0; j < BLOCK_SIZE; j++){
            cache->slots[i].values[j].value = NULL_VALUE_FOR_CACHE;
            cache->slots[i].values[j].update = 0;
            cache->slots[i].values[j].RAMaddress = -1;
            cache->slots[i].state = NUL;
        }
    }
}
void initializeCacheList(Cache caches[], int n){
    for (int i = 0; i < n; i++){
        // printf("\niniciou %d", i);
        initializeCacheSlots(&caches[i]);
        caches[i].id = i;
    }
}
CacheSlot* accessRAMSlot(int addr, int update, int ram[], int*pos){
    // acessa um slot na memoria ram e retorna o bloco em q o elemento procurado se encontra
    // addr = endereco da memoria ram
    // update = 1 se for uma atualizacao, 0 se for um acesso
    // ram = vetor de memoria ram
    // pos = posicao do bloco em que o elemento esta

    // aloca um CacheSlot na memoria
    CacheSlot* ret = (malloc(sizeof(CacheSlot)));

    // verifica em que bloco o elemento se encontra
    int block = addr/BLOCK_SIZE;
    // recebe a posicao do elemento acessado dentro do bloco
    *pos = addr%BLOCK_SIZE;

    // copia os valores da ram para 'ret'
    int b = 0;
    while (b < BLOCK_SIZE){
        // valor do elemento acessado
        ret->values[b].value = ram[(block*BLOCK_SIZE)+b];
        // endereco do elemnto acessado
        ret->values[b].RAMaddress = (block*BLOCK_SIZE)+b;
        ret->values[b].update = 0;
        b++;
    }
    // caso precise ser atualizado o valor eh modificado por um nome aleatorio
    // e a variavel update muda para 1 para indicar que o elemento precisa passar
    // pelo write-back
    if (update){
        printf("\nValor atualizado, de %d para ", ret->values[addr%BLOCK_SIZE].value);

        ret->values[addr%BLOCK_SIZE].update = 1;
        ret->values[addr%BLOCK_SIZE].value = getRandomInt(100, 999);
       
        printf("%d", ret->values[addr%BLOCK_SIZE].value);
    }
    return ret;
}
void switchCacheBlock(CacheSlot cache[], int mram[], int pos, CacheSlot* block){
    // troca CacheBlock's quando a memoria cache ja se encontra cheia
    // pos = posicao do bloco na memoria cache que vai ser substituido
    // block = bloco que vai substituir o bloco na posicao pos

    // caso precise atualizar algum valor no bloco a ser substituido
    // passamos para a funcao writeBack atualizar os valores na memoria RAM
    if (needToUpdateBlock(cache, pos)){
        writeBack(cache, mram, pos);
    }

    // apos atualizar caso necessario, copia os valores do bloco que vai substituir para o bloco que vai substituir
    for (int i = 0; i < BLOCK_SIZE; i++){
        cache[pos].values[i].value = block->values[i].value; 
        cache[pos].values[i].update = block->values[i].update; 
        cache[pos].values[i].RAMaddress = block->values[i].RAMaddress; 
    }
}
// int TryToAccessAddress(int mram[], int slot, int update, CacheSlot cache[], Fila* FIFO){
//     // cuida de toda a logica de acessar uma poiscao da memoria RAM e se deve ou nao copiar o bloco para a CACHE
//     int pos, block;
//     CacheSlot* ptr;
//     // acessa a posicao da memoria RAM
//     ptr = accessRAMSlot(slot, update, mram, &pos);
//     printf("\nBloco acessado: ");
//     printCacheSlot(ptr);
//
//     // caso o endereco ja se encontre na cache retorna 1
//     if (isInsideCache(ptr->values[pos].RAMaddress, cache, &block)){
//         if (update){
//             cache[block].values[pos].value = ptr->values[pos].value;
//             cache[block].values[pos].update = 1;
//         }
//         return 1;
//     }
//
//     int new;
//
//     new = NextInLine(FIFO)->value;
//     switchCacheBlock(cache, mram, new, ptr);
//
//     return 0;
//
// }
int requestAccessToRAM(int mram[], int req, int update, Cache caches[], int src, Fila FIFO[]){
    int pos, block;
    CacheSlot* ptr;

    ptr = accessRAMSlot(req, update, mram, &pos);
    printf("\nBloco acessado: ");
    printCacheSlot(ptr);


    // caso o valor ja se encontra na cache solicitante
    if (isInsideCache(ptr->values[pos].RAMaddress, caches[src].slots, &block)){
        if (update){
            caches[src].slots[block].values[pos].value = ptr->values[pos].value;
            caches[src].slots[block].values[pos].update = 1;
            // printf("\nxcache src id: %d", caches[src].id);
            switch (caches[src].slots[block].state){
                case SHARED:
                    caches[src].slots[block].state = MODFIED;
                    for (int i = 0; i < NUMBER_OF_CACHES; i++){
                        if (caches[src].id != i){
                            if (isInsideCache(ptr->values[pos].RAMaddress, caches[i].slots, &block)){
                                caches[i].slots[block].state = INVALID;
                                // printf("\nvirou invalid na cache %d", i);
                            }
                        }
                    }
                    break;
                case EXCLUSIVE:
                    caches[src].slots[block].state = MODFIED;
                    break;
            }
            printf("\nOcorreu um WH na cache %d", caches[src].id);
            return 0;
        }
        printf("\nOcorreu um RH na cache %d", caches[src].id);
        return 0;
    }

    // caso o valor nao se encontra na cache solicitante
    int new;
    new = NextInLine(&FIFO[src])->value;
    switchCacheBlock(caches[src].slots, mram, new, ptr);
    if (update){
        caches[src].slots[new].state = MODFIED;
        // printf("\ncache src id: %d", caches[src].id);
        for (int i = 0; i < NUMBER_OF_CACHES; i++){
            if (caches[src].id != i){
                if (isInsideCache(ptr->values[pos].RAMaddress, caches[i].slots, &block)){
                    caches[i].slots[block].state = INVALID;
                    // printf("\nvirou invalid na cache %d", i);
                }
            }
        }
        printf("\nOcorreu um WM na cache %d", caches[src].id);
        return 0;
    }
    int exc = 1;
    for (int i = 0; i < NUMBER_OF_CACHES; i++){
        if (caches[src].id != i){
            if (isInsideCache(ptr->values[pos].RAMaddress, caches[i].slots, &block)){
                caches[i].slots[block].state = SHARED;
                caches[src].slots[new].state = SHARED;
                exc = 0;
            }
        }
    }
    if (exc){
        caches[src].slots[new].state = EXCLUSIVE;
    }
    printf("\nOcorreu um RM na cache %d", caches[src].id);
    return 0;

}

int StepByStepUser(){
    // funcao onde o usuario anda passo a passo pelo programa
    // inicializa o necessario para o programa funcionar
    int req, update = 0;
    srand(time(NULL));
    int mram[MRAM_SIZE];
    generateRandomRAMValues(mram, MRAM_SIZE, 100, 999);
    Cache cacheList[NUMBER_OF_CACHES];
    
    // printCache(cacheList[0]);
    // printCache(cacheList[1]);
    
    initializeCacheList(cacheList, NUMBER_OF_CACHES);

    // printCache(cacheList[0]);
    // printCache(cacheList[1]);

    Fila FIFOList[NUMBER_OF_CACHES];
    initializeListOfFIFO(FIFOList, NUMBER_OF_CACHES);
    OrderListOfFIFO(FIFOList, NUMBER_OF_CACHES);


    printRAMandAllCaches(mram, cacheList, NUMBER_OF_CACHES);

    do
    {
        printf("\nDigite o ID da memoria CACHE que deseja utilizar: ");
        char c = handleUserInput(&update, NUMBER_OF_CACHES);
        printf("\n------------------------------------------------------------");
        printf("\n\nDigite um index para acessar da memoria RAM ou 'quit' para sair");
        req = handleUserInput(&update, MRAM_SIZE);
        
        printf("\n ");
        printf("\nCACHE %d ACESSANDO INDEX '%d' DA MRAM, DE VALOR %d", cacheList[c].id, req, mram[req]);
        // TryToAccessAddress(mram, input, update, &cacheList->slots[0], &fila);
        requestAccessToRAM(mram, req, update, cacheList, c, FIFOList);
        printAllFIFO(FIFOList, NUMBER_OF_CACHES);

        
        printf("\n");
        printRAMandAllCaches(mram, cacheList, NUMBER_OF_CACHES);
    } while (req != -1);
    return 0;
}

int main(int argc, char* argv[]){
    StepByStepUser();
    return 0;
}