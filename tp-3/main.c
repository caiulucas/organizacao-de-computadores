#include "cpu.h"
#include "generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char **argv)
{

    srand(1507); // Inicializacao da semente para os numeros aleatorios.

    int memoriesSize[5];
    Machine machine;
    Instruction *instructions;

    // mudar para fazer funcionar a leitura de arquivos
    if (strcmp(argv[1], "random") == 0)
    {
        if (argc != 7)
        {
            printf("Numero de argumentos invalidos! Sao 6.\n");
            printf("Linha de execucao: ./exe TIPO_INSTRUCAO [TAMANHO_MEMORIA|ARQUIVO_DE_INSTRUCOES] TAMANHO_RAM TAMANHO_L1 TAMANHO_L2 TAMANHO_L3\n");
            printf("\tExemplo 1 de execucao: ./exe random 20 10 2 4 6\n");
            printf("\tExemplo 2 de execucao: ./exe file arquivo_de_instrucoes.txt\n");
            return 0;
        }

        memoriesSize[0] = atoi(argv[2]);
        memoriesSize[1] = atoi(argv[3]);
        instructions = generateRandomInstructions(memoriesSize[1]);
        memoriesSize[2] = atoi(argv[4]);
        memoriesSize[3] = atoi(argv[5]);
        memoriesSize[4] = atoi(argv[6]);
    }
    else if (strcmp(argv[1], "file") == 0)
    {
        instructions = readInstructions(argv[2], memoriesSize);
    }
    else
    {
        printf("Invalid option.\n");
        return 0;
    }

    printf("Starting machine...\n");
    start(&machine, instructions, memoriesSize);
    // if (memoriesSize[0] < 20)
    //     printMemories(&machine);
    run(&machine);
    // if (memoriesSize[0] < 20)
        // printMemories(&machine);
    stop(&machine);
    // printf("Stopping machine...\n");
    return 0;
}