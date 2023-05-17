#ifndef CPU_H
#define CPU_H

typedef struct
{
  int opcode;
  int info1;
  int info2;
  int info3;
} Instruction;

typedef struct
{
  int *items;
  int size;
} Memory;

typedef struct
{
  Instruction *instructions;
  Memory RAM;
} Machine;

int sum(int value1, int value2);
int sub(int value1, int value2);
int mult(int value1, int value2);
int division(int value1, int value2);
int expo(int value1, int value2);
int fibo(int stop);

void start(Machine *, Instruction *, int);
void stop(Machine *);
void run(Machine *);
void printRAM(Machine *);

#endif // !CPU_H