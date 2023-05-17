#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "constants.h"

typedef struct {
    int words[WORDS_SIZE];
    int tag;    
} MemoryBlock;

typedef struct {
    MemoryBlock* blocks;
    int size;
} RAM;

typedef struct {
    MemoryBlock* blocks;
    int size;
} MEMORY;

typedef struct {
    MemoryBlock block;
    int tag; /* Address of the block in memory RAM */
    bool updated;
    int cost;
    int cacheHit;
    int time;
} Line;

typedef struct {
    Line* lines;
    int size;
} Cache;

void startMemory(MEMORY*, int);
void stopMemory(MEMORY*, int);

void updateMemoryReceive(MEMORY*, int);
void updateMemoryApply(MEMORY*, int);

void startCache(Cache*, int);
void stopCache(Cache*);

void startRAM(RAM*, int);
void stopRAM(RAM*);

#endif // !MEMORY_H