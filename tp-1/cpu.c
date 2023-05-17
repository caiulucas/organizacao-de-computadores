#include "cpu.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int sum(int value1, int value2)
{
  return value1 + value2;
}

int sub(int value1, int value2)
{
  return value1 - value2;
}

int mult(int value1, int value2)
{
  int result = 0;

  if (value1 == 0 || value2 == 0)
    return 0;

  int absValue1 = (value1 < 0) ? -value1 : value1;
  int absValue2 = (value2 < 0) ? -value2 : value2;

  for (int i = 0; i < absValue2; i++)
  {
    result = sum(result, absValue1);
  }

  result = ((value1 < 0 && value2 > 0) || (value1 > 0 && value2 < 0)) ? -result : result;

  return result;
}

int division(int value1, int value2)
{
  int result = 0;

  int absValue1 = (value1 < 0) ? -value1 : value1;
  int absValue2 = (value2 < 0) ? -value2 : value2;

  while (absValue1 >= absValue2)
  {
    absValue1 = sub(absValue1, absValue2);
    result = sum(result, 1);
  }

  result = ((value1 < 0 && value2 > 0) || (value1 > 0 && value2 < 0)) ? -result : result;

  return result;
}

int expo(int value1, int value2)
{
  int result = 1;

  for (int i = 0; i < value2; i++)
  {
    result = mult(result, value1);
  }

  return result;
}

int fibo(int stop)
{
  int primary = 0;
  int secondary = 1;
  int result = 0;

  for (int i = 0; i < stop - 2; i++)
  {
    result = sum(primary, secondary);
    primary = secondary;
    secondary = result;
  }

  return result;
}

void start(Machine *machine, Instruction *instructions, int RAMSize)
{
  machine->instructions = instructions;
  machine->RAM.items = (int *)malloc(sizeof(int) * RAMSize);
  machine->RAM.size = RAMSize;
  for (int i = 0; i < RAMSize; i++)
  {
    int itemValue = (int)(rand() % 51);
    machine->RAM.items[i] = itemValue;
  }
}

void stop(Machine *machine)
{
  free(machine->instructions);
  free(machine->RAM.items);
}

void run(Machine *machine)
{
  // Registradores
  int PC = 0; // Program Counter - Contador de programa
  int opcode = 0;
  int address1;
  int address2;
  int RAMContent1;
  int RAMContent2;
  int value;
  int result;

  int address3;

  char opMessage[20];

  while (opcode != -1)
  {
    Instruction instruction = machine->instructions[PC];
    opcode = instruction.opcode;

    if (opcode == 0)
    {
      value = (float)instruction.info1;
      address1 = instruction.info2;
      machine->RAM.items[address1] = value;
      printf("  > Levando informação (%d) para a RAM[%d].\n", value, address1);
      return;
    }

    address1 = instruction.info1;
    address2 = instruction.info2;
    RAMContent1 = machine->RAM.items[address1];
    RAMContent2 = machine->RAM.items[address2];

    switch (opcode)
    {
    case -1:
      printf("  > Finalizando a execução.\n");
      break;

    case 1: // Somando
      result = sum(RAMContent1, RAMContent2);
      strcpy(opMessage, "Somando");
      break;

    case 2: // Subtraindo
      result = sub(RAMContent1, RAMContent2);
      strcpy(opMessage, "Subtraindo");
      break;

    case 3: // Multiplicando
      result = mult(RAMContent1, RAMContent2);
      strcpy(opMessage, "Multiplicando");
      break;

    case 4: // Dividindo
      result = division(RAMContent1, RAMContent2);
      strcpy(opMessage, "Dividindo");
      break;

    case 5: // Exponenciando
      result = expo(RAMContent1, RAMContent2);
      strcpy(opMessage, "Exponenciando");
      break;

    case 6: // Fibonacci
      result = (RAMContent1);
      strcpy(opMessage, "Fibonacci");
      break;

    default:
      printf("  > Finalizando a execução.\n");
      break;
    }


    address3 = instruction.info3;

    if (opcode == 6)
    {
      printf("  > %s até RAM[%d] (%d) e salvando na RAM[%d] (%d).\n",
             opMessage, address1, RAMContent1, address3, result);
    }
    else
    {
      printf("  > %s RAM[%d] (%d) com RAM[%d] (%d) e salvando na RAM[%d] (%d).\n",
             opMessage, address1, RAMContent1, address2, RAMContent2, address3, result);
    }
    machine->RAM.items[address3] = result;

    PC++;
  }
}

void printRAM(Machine *machine)
{
  printf("  > RAM");
  for (int i = 0; i < machine->RAM.size; i++)
    printf("\t\t[ %d ] : %d\n", i, machine->RAM.items[i]);
}