#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


#define RAM_TEST_ARRAY_SIZE 16
#define MRAM_SIZE 64
#define CACHE_SIZE 4 
#define BLOCK_SIZE 4
#define NULL_VALUE_FOR_CACHE -11
#define NUMBER_OF_CACHES 2

// usado para a criacao de array para o teste automatico
typedef struct InputObj{
    int value;
    int update;
}InputObj;
// usado para a memoria cache, onde a cache se trata de 
// um array de N CacheSlot's, cada CacheSlot eh um array de BLOCK_SIZE CacheValue's
// cada CacheSlot se iguala eh um bloco da cache, que guarda em si um certo numero
// de valores definidos pelo BLOCK_SIZE
typedef struct CacheValue{
    int value;
    int RAMaddress;
    int update;
}CacheValue;
typedef struct CacheSlot{
    CacheValue values[BLOCK_SIZE];
    enum {MODFIED, EXCLUSIVE, SHARED, INVALID, NUL} state;
}CacheSlot;
typedef struct Cache{
    int id;
    CacheSlot slots[CACHE_SIZE];
}Cache;

// standard values for constants: 
// RAM_TEST_ARRAY_SIZE = 100
// MRAM_SIZE = 1024
// CACHE_SIZE = 32
// BLOCK_SIZE = 4

