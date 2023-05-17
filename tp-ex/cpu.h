#ifndef CPU_H
#define CPU_H

#include "instruction.h"
#include "memory.h"


typedef struct {
    Instruction* instructions;
    MEMORY memoryEx;
    RAM ram;
    Cache l1; // cache L1
    Cache l2; // cache L2
    Cache l3; // cache L3
    int hitL1, hitL2, hitL3, hitRAM, hitMemoryEx;
    int missL1, missL2, missL3, missRAM;
    int totalCost;
} Machine;

void start(Machine*, Instruction*, int*);
void stop(Machine*, int*);
void run(Machine*);
void printMemories(Machine*, int);
void tratadorDeInterrupcao (Machine* machine, int word1);

#endif // !CPU_H