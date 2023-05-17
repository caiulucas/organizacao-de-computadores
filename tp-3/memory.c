#include <stdio.h>

#include "memory.h"
#include "file.h"

void startStorage(Storage *storage, int size)
{
  storage->blocks = (MemoryBlock *)malloc(sizeof(MemoryBlock) * size);
  storage->size = size;

  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < WORDS_SIZE; j++)
    {
      storage->blocks[i].words[j] = rand() % 100;
      storage->blocks[i].hit = 0;
      storage->blocks[i].tag = INVALID_ADD;
      storage->blocks[i].updated = false;
    }
  }

  saveStorage(*storage);
}

void stopStorage(Storage *storage)
{
  free(storage->blocks);
}

void startRAM(RAM *ram, int size)
{
  ram->blocks = (MemoryBlock *)malloc(sizeof(MemoryBlock) * size);
  ram->size = size;

  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < WORDS_SIZE; j++)
    {
      ram->blocks[i].words[j] = rand() % 100;
      ram->blocks[i].hit = 0;
      ram->blocks[i].tag = INVALID_ADD;
      ram->blocks[i].updated = false;
      ram->blocks[i].cost = 0;
    }
  }
}

void stopRAM(RAM *ram)
{
  free(ram->blocks);
}

void startCache(Cache *cache, int size)
{
  cache->lines = (Line *)malloc(sizeof(Line) * size);
  cache->size = size;

  for (int i = 0; i < size; i++)
  {
    cache->lines[i].tag = INVALID_ADD;
    cache->lines[i].cacheTime = 0;
    cache->lines[i].timesUsed = 0;
    cache->lines[i].cost= 0;
    cache->lines[i].cacheHit = 0;
    cache->lines[i].updated = false;
  }
}

void stopCache(Cache *cache)
{
  free(cache->lines);
}
