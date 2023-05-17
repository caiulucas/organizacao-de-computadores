#include "cpu.h"
#include "generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char **argv)
{
  srand(time(NULL)); // Inicialização da semente para os números aleatórios.

  if (argc != 3)
  {
    printf("Numero de argumentos inválidos! Sao 3.\n");
    printf("Linha de execução: ./exe TIPO_INSTRUCAO [TAMANHO_RAM|ARQUIVO_DE_INSTRUCOES]\n");
    printf("\tExemplo 1 de execução: ./exe random 10\n");
    printf("\tExemplo 3 de execução: ./exe file arquivo_de_instrucoes.txt\n");
    return 0;
  }
  
  int ramSize;
  Machine machine;
  Instruction *instructions;

  if (strcmp(argv[1], "random") == 0)
  {
    ramSize = atoi(argv[2]);
    instructions = generateRandomInstructions(ramSize);
  }
  else if (strcmp(argv[1], "file") == 0)
  {
    instructions = readInstructions(argv[2], &ramSize);
  }
  else
  {
    printf("Opção inválida.\n");
    return 0;
  }

  printf("Iniciando a máquina...\n");
  start(&machine, instructions, ramSize);
  printRAM(&machine);
  run(&machine);
  printRAM(&machine);
  stop(&machine);
  printf("Finalizando a máquina...\n");
  
  return 0;
}