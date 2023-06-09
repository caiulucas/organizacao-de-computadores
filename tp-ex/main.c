#include "cpu.h"
#include "generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char**argv) {

    srand(1507);   // Inicializacao da semente para os numeros aleatorios.

    if (argc != 7) {
        printf("Numero de argumentos invalidos! Sao 5.\n");
        printf("Linha de execucao: ./exe TIPO_INSTRUCAO [TAMANHO_RAM|ARQUIVO_DE_INSTRUCOES] TAMANHO_L1 TAMANHO_L2\n");
        printf("\tExemplo 1 de execucao: ./exe random 20 10 2 4 8\n");
        printf("\tExemplo 2 de execucao: ./exe file arquivo_de_instrucoes.txt\n");
        return 0;
    }

    int memoriesSize[5];
    Machine machine;
    Instruction *instructions;

    memoriesSize[0] = atoi(argv[2]);        //Externa
    memoriesSize[1] = atoi(argv[3]);        //RAM
    memoriesSize[2] = atoi(argv[4]);        //Cache 1
    memoriesSize[3] = atoi(argv[5]);        //Cache 2
    memoriesSize[4] = atoi(argv[6]);    
        //Cache 3
    if (strcmp(argv[1], "random") == 0) {
        instructions = generateRandomInstructions(memoriesSize[0]);     
    } else if (strcmp(argv[1], "file") == 0) {
        instructions = readInstructions(argv[2], memoriesSize);
    } 
    else {
        printf("Invalid option.\n");
        return 0;
    }
    
    printf("Starting machine...\n");
    start(&machine, instructions, memoriesSize);
    if (memoriesSize[1] < 20)
        printMemories(&machine, memoriesSize[0]);
    run(&machine);
    if (memoriesSize[1] < 20)
        printMemories(&machine, memoriesSize[0]);
    stop(&machine, memoriesSize);
    printf("Stopping machine...\n");
    return 0;
}