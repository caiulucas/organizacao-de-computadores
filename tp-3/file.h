#pragma once
#include "memory.h"

void saveStorage(Storage);

void readBinaryFile(Storage *);

MemoryBlock *getBlockFromFile(int);

int appendBlockToBinary(Storage *);
