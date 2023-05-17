#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "cpu.h"
#include "mmu.h"
#include "file.h"
#include "constants.h"

void start(Machine *machine, Instruction *instructions, int *memoriesSize)
{
  if (memoriesSize[0] == 0)
  {
    readBinaryFile(&machine->storage);
  }
  else
  {
    startStorage(&machine->storage, memoriesSize[0]);
  }

  startRAM(&machine->ram, memoriesSize[1]);
  startCache(&machine->l1, memoriesSize[2]);
  startCache(&machine->l2, memoriesSize[3]);
  startCache(&machine->l3, memoriesSize[4]);
  machine->instructions = instructions;
  machine->hitL1 = 0;
  machine->hitL2 = 0;
  machine->hitL3 = 0;
  machine->hitRAM = 0;
  machine->missL1 = 0;
  machine->missL2 = 0;
  machine->missL3 = 0;
  machine->missRAM = 0;
  machine->totalCost = 0;
  machine->interruptions = 0;
}

void stop(Machine *machine)
{
  free(machine->instructions);
  stopStorage(&machine->storage);
  stopRAM(&machine->ram);
  stopCache(&machine->l1);
  stopCache(&machine->l2);
  stopCache(&machine->l3);
}

void executeInstruction(Machine *machine, int PC, bool interrupted)
{
  Instruction instruction = machine->instructions[PC];
  // Registers
  int word1, word2;

  // Addresses to be consulted
  Address add1 = instruction.add1;
  Address add2 = instruction.add2;
  Address add3 = instruction.add3;

  // Line find in memory
  Line *line;

  switch (instruction.opcode)
  {
  case -1:
    printf("  > Ending execution.\n");
    break;
  case 0:
    if (interrupted)
    {
      // Taking information to RAM
      line = MMUSearchOnMemories(add1, machine); /* Searching block on memories */
      word1 = line->block.words[add1.word];
#ifdef PRINT_LOG
      printf("  > MOV BLOCK[%d.%d.%d](%4d) > \n", line->cacheHit, add1.block, add1.word, line->block.words[add1.word]);
#endif

      line = MMUSearchOnMemories(add2, machine); /* Searching block on memories */
#ifdef PRINT_LOG
      printf("BLOCK[%d.%d.%d](%4d| \n", line->cacheHit, add2.block, add2.word, line->block.words[add2.word]);
#endif

      line->block.words[add2.word] = word1;
      line->updated = true;
#ifdef PRINT_LOG
      printf("%4d).\n", line->block.words[add2.word]);
#endif
    }

    int nInterruptions = rand() % 10;
    machine->interruptions = nInterruptions;
    break;

#ifdef PRINT_INTERRUPTIONS
    printf("  > %d interruptions!\n", nInterruptions);
#endif

    for (int i = 0; i < nInterruptions; i++)
    {
      instruction.opcode = rand() % 3;
    }

  case 1:                                      // Sum
    line = MMUSearchOnMemories(add1, machine); /* Searching block on memories */
    word1 = line->block.words[add1.word];
#ifdef PRINT_LOG
    printf("  > SUM BLOCK[%d.%d.%d](%4d)\n", line->cacheHit, add1.block, add1.word, line->block.words[add1.word]);
#endif

    line = MMUSearchOnMemories(add2, machine); /* Searching block on memories */
    word2 = line->block.words[add2.word];
#ifdef PRINT_LOG
    printf(" + BLOCK[%d.%d.%d](%4d)\n", line->cacheHit, add2.block, add2.word, line->block.words[add2.word]);
#endif

    line = MMUSearchOnMemories(add3, machine); /* Searching block on memories */
#ifdef PRINT_LOG
    printf(" > BLOCK[%d.%d.%d](%4d|\n", line->cacheHit, add3.block, add3.word, line->block.words[add3.word]);
#endif

    line->updated = true;
    line->block.words[add3.word] = word2 + word1;
#ifdef PRINT_LOG
    printf("%4d).\n", line->block.words[add3.word]);
#endif
    break;
  case 2:                                      // Subtract
    line = MMUSearchOnMemories(add1, machine); /* Searching block on memories */
    word1 = line->block.words[add1.word];
#ifdef PRINT_LOG
    printf("  > SUB BLOCK[%d.%d.%d](%4d)\n", line->cacheHit, add1.block, add1.word, line->block.words[add1.word]);
#endif

    line = MMUSearchOnMemories(add2, machine); /* Searching block on memories */
    word2 = line->block.words[add2.word];
#ifdef PRINT_LOG
    printf(" - BLOCK[%d.%d.%d](%4d)\n", line->cacheHit, add2.block, add2.word, line->block.words[add2.word]);
#endif

    line = MMUSearchOnMemories(add3, machine); /* Searching block on memories */
#ifdef PRINT_LOG
    printf(" > BLOCK[%d.%d.%d](%4d|\n", line->cacheHit, add3.block, add3.word, line->block.words[add3.word]);
#endif

    line->updated = true;
    line->block.words[add3.word] = word2 - word1;

#ifdef PRINT_LOG
    printf("%4d).\n", line->block.words[add3.word]);
#endif

    break;

  default:
    printf("Invalid instruction.\n");
  }
#ifdef PRINT_INTERMEDIATE_RAM
  printMemories(machine);
#endif

  if (interrupted == true)
    machine->interruptions -= 1;

  while (machine->interruptions > 0)
  {
#ifdef PRINT_INTERRUPTIONS
    printf("> Fix interruption %d\n", machine->interruptions);
#endif
    executeInstruction(machine, machine->interruptions - 1, true); // avoid generate infinite interruptions
  }
}

void run(Machine *machine)
{
  int PC = 0; // Program Counter
  while (machine->instructions[PC].opcode != -1)
  {
    executeInstruction(machine, PC++, false);
    printf("\tL1:(%6d, %6d) | L2:(%6d, %6d) | L3:(%6d, %6d) | RAM:(%6d, %6d) | COST: %d\n",
           machine->hitL1, machine->missL1,
           machine->hitL2, machine->missL2,
           machine->hitL3, machine->missL3,
           machine->hitRAM, machine->missRAM,
           machine->totalCost);
  }
}

void printc(char *text, int fieldWidth)
{
  int padlenLeft = floor((fieldWidth - strlen(text)) * 0.5);
  int padlenRight = ceil((fieldWidth - strlen(text)) * 0.5);
  printf("|%*s%s%*s|", padlenLeft, "", text, padlenRight, "");
}

void printcolored(int n, bool updated)
{
  if (updated)
    printf("\x1b[1;37;42m[%5d]\x1b[0m", n); // Green
  else
    printf("\x1b[1;37;41m[%5d]\x1b[0m", n); // Red
}

void printMemories(Machine *machine)
{
  printf("\x1b[0;30;47m     ");
  printc("Storage", WORDS_SIZE * 8 - 1);
  printc("RAM", WORDS_SIZE * 8 + 6);
  printc("Cache L3", WORDS_SIZE * 8 + 6);
  printc("Cache L2", WORDS_SIZE * 8 + 6);
  printc("Cache L1", WORDS_SIZE * 8 + 6);
  printf("\x1b[0m\n");

  for (int i = 0; i < machine->storage.size; i++)
  {
    printf("\x1b[0;30;47m%5d|\x1b[0m", i);

    for (int j = 0; j < WORDS_SIZE; j++)
    {
      printf(" %5d |", machine->storage.blocks[i].words[j]);
    }

    if (i < machine->ram.size)
    {
      printf("|");
      printcolored(machine->ram.blocks[i].tag, machine->ram.blocks[i].updated);
      for (int j = 0; j < WORDS_SIZE; j++)
      {
        printf(" %5d |", machine->ram.blocks[i].words[j]);
      }

      if (i < machine->l3.size)
      {
        printf("|");
        printcolored(machine->l3.lines[i].tag, machine->l3.lines[i].updated);

        for (int j = 0; j < WORDS_SIZE; j++)
        {
          printf(" %5d |", machine->l3.lines[i].block.words[j]);
        }

        if (i < machine->l2.size)
        {
          printf("|");

          printcolored(machine->l2.lines[i].tag, machine->l2.lines[i].updated);

          for (int j = 0; j < WORDS_SIZE; j++)
          {
            printf(" %5d |", machine->l2.lines[i].block.words[j]);
          }

          if (i < machine->l1.size)
          {
            printf("|");

            printcolored(machine->l1.lines[i].tag, machine->l1.lines[i].updated);

            for (int j = 0; j < WORDS_SIZE; j++)
              printf(" %5d |", machine->l1.lines[i].block.words[j]);
          }
        }
      }
    }
    printf("\n");
  }
}
