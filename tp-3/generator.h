#ifndef GENERATOR_H
#define GENERATOR_H

#include "cpu.h"

Instruction* generateRandomInstructions(int);
Instruction* readInstructions(char*, int*);
Instruction *generateRandomInstruction(int ramSize);


#endif // !GENERATOR_H