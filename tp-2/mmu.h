#ifndef MMU_H
#define MMU_H

#include "memory.h"
#include "instruction.h"
#include "cpu.h"

Line* MMUSearchOnMemories(Address, Machine*);

#endif // !MMU_H