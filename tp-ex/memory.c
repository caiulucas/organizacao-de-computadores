#include "memory.h"

void startRAM(RAM *ram, int size)
{
    ram->blocks = (MemoryBlock *)malloc(sizeof(MemoryBlock) * size);
    ram->size = size;

    for (int i = 0; i < size; i++)
    {
        ram->blocks[i].tag = i;
        for (int j = 0; j < WORDS_SIZE; j++)
            ram->blocks[i].words[j] = rand() % 100;
    }
}

void startMemory(MEMORY *memoryEx, int size)
{
    memoryEx->blocks = (MemoryBlock *)malloc(sizeof(MemoryBlock) * size);
    memoryEx->size = size;

    FILE *arquivo = fopen("MemoriaExterna.bin", "wb");

    for (int i = 0; i < size; i++)
    {
        memoryEx->blocks[i].tag = i;
        fwrite(&memoryEx->blocks[i].tag, sizeof(int), 1, arquivo);

        for (int j = 0; j < WORDS_SIZE; j++)
        {
            memoryEx->blocks[i].words[j] = rand() % 100;
            fwrite(&memoryEx->blocks[i].words[j], sizeof(int), 1, arquivo);
        }
    }

    fclose(arquivo);
}

void stopRAM(RAM *ram)
{
    free(ram->blocks);
}

void stopMemory(MEMORY *memoryEx, int size)
{
    // updateMemoryReceive(memoryEx, size);
    free(memoryEx->blocks);
}

void startCache(Cache *cache, int size)
{
    cache->lines = (Line *)malloc(sizeof(Line) * size);
    cache->size = size;

    for (int i = 0; i < size; i++)
    {
        cache->lines[i].tag = INVALID_ADD;
        cache->lines[i].time = 0;
        cache->lines[i].block.tag = INVALID_ADD;
        cache->lines[i].updated = false;
        for (int j = 0; j < WORDS_SIZE; j++)
            cache->lines[i].block.words[j] = 0;
    }
}

void stopCache(Cache *cache)
{
    free(cache->lines);
}

void updateMemoryReceive(MEMORY *memoryEx, int size)
{
    FILE *arquivo = fopen("MemoriaExterna.bin", "rb");

    for (int i = 0; i < size; i++)
    {
        fread(&memoryEx->blocks[i].tag, sizeof(int), 1, arquivo);

        for (int j = 0; j < WORDS_SIZE; j++)
        {
            fread(&memoryEx->blocks[i].words[j], sizeof(int), 1, arquivo);
        }
    }

    fclose(arquivo);
}

void updateMemoryApply(MEMORY *memoryEx, int size)
{
    FILE *arquivo = fopen("MemoriaExterna.bin", "wb");

    for (int i = 0; i < size; i++)
    {
        fwrite(&memoryEx->blocks[i].tag, sizeof(int), 1, arquivo);

        for (int j = 0; j < WORDS_SIZE; j++)
        {
            fwrite(&memoryEx->blocks[i].words[j], sizeof(int), 1, arquivo);
        }
    }

    fclose(arquivo);
}