#include "mmu.h"

#include <stdio.h>

int time = 0;

bool canOnlyReplaceBlock(Line line)
{
    // Or the block is empty or
    // the block is equal to the one in memory RAM and can be replaced
    if (line.tag == INVALID_ADD || (line.tag != INVALID_ADD && !line.updated))
        return true;
    return false;
}

int memoryCacheMapping(int *pos, Line *lines, int size, int block)
{

    for (int i = 0; i < size; i++)
    {
        if (lines[i].tag == block)
        {
            *pos = i;
            return 1;
        }
    }

    return 0;
}

int ramMapping(int *pos, MemoryBlock *RAM, int size, int block, int word)
{
    for (int i = 0; i < size; i++)
    {
        //  printf("\n*********\n%d %d  e %d %d\n***************\n", RAM[i].tag, RAM[i].words[i], block, word);
        if (RAM[i].tag == (block % size))
        {
            for (int j = 0; j < WORDS_SIZE; j++)
            {
                //     printf("\n*********\n%d %d  e %d %d\n***************\n", RAM[i].tag, RAM[i].words[j], block%size, word);

                if (RAM[i].words[j] == word)
                {
                    *pos = i;
                    //    printf("\nFOIIII\n");
                    return 1;
                }
            }
        }
    }

    // printf("\nNAOOOOOOOOOOO\n");
    return 0;
}

void memoryToRam(MemoryBlock *RAM, MemoryBlock *memoryEx, int ramSize, int memorySize, int block)
{

    int ramPos, memoryPos;

    //  updateMemoryReceive(memoryEx, memorySize);

    for (int i = 0; i < memorySize; i++)
    {
        if (memoryEx[i].tag == block)
        {
            memoryPos = i;
            break;
        }
    }

    ramPos = block % ramSize;

    MemoryBlock aux;

    aux = memoryEx[memoryPos];

    memoryEx[memoryPos] = RAM[ramPos];

    RAM[ramPos] = aux;

    //  updateMemoryApply(memoryEx, memorySize);
}

void updateMachineInfos(Machine *machine, Line *line)
{

    switch (line->cacheHit)
    {
    case 1:
        machine->hitL1 += 1;
        time++;
        break;

    case 2:
        machine->hitL2 += 1;
        machine->missL1 += 1;
        time++;
        break;

    case 3:
        machine->hitL3 += 1;
        machine->missL1 += 1;
        machine->missL2 += 1;
        time++;
        break;

    case 4:
        machine->hitRAM += 1;
        machine->missL1 += 1;
        machine->missL2 += 1;
        machine->missL3 += 1;
        time++;
        break;

    case 5:
        machine->hitMemoryEx += 1;
        machine->missL1 += 1;
        machine->missL2 += 1;
        machine->missL3 += 1;
        machine->missRAM += 1;
        time++;
        break;
    }

    machine->totalCost += line->cost;
}

int LRU(Cache *cache)
{

    int size = cache->size, menorTempo = cache->lines[0].time;

    for (int i = 0; i < size; i++)
    {
        if (menorTempo > cache->lines[i].time)
        {
            menorTempo = cache->lines[i].time;
            return i;
        }
    }
    return 0;
}

Line *MMUSearchOnMemorys(Address add, Machine *machine)
{
    //     Strategy => write back

    //     LRU map

    int l1pos = 0, l2pos = 0, l3pos = 0, RAMpos = 0;

    Line *cache1 = machine->l1.lines;
    Line *cache2 = machine->l2.lines;
    Line *cache3 = machine->l3.lines;
    // MemoryBlock* RAM = machine->ram.blocks;
    // MemoryBlock* MEMORY = machine->memoryEx.blocks;

    if (memoryCacheMapping(&l1pos, cache1, machine->l1.size, add.block) == 1)
    {
        /* Block is in memory cache L1 */
        cache1[l1pos].cost = COST_ACCESS_L1;
        cache1[l1pos].cacheHit = 1;
        cache1[l1pos].time = time;
    }
    else if (memoryCacheMapping(&l2pos, cache2, machine->l2.size, add.block) == 1)
    {
        /* Block is in memory cache L2 */
        cache2[l2pos].tag = add.block;
        cache2[l2pos].updated = false;
        cache2[l2pos].cost = COST_ACCESS_L1 + COST_ACCESS_L2;
        cache2[l2pos].cacheHit = 2;
        cache1[l2pos].time = time;
        updateMachineInfos(machine, &(cache2[l2pos]));
        return &(cache2[l2pos]);
    }
    else if (memoryCacheMapping(&l3pos, cache3, machine->l3.size, add.block) == 1)
    {
        /* Block is in memory cache L3*/
        cache3[l3pos].tag = add.block;
        cache3[l3pos].updated = false;
        cache3[l3pos].cost = COST_ACCESS_L1 + COST_ACCESS_L2 + COST_ACCESS_L3;
        cache3[l3pos].cacheHit = 3;
        cache1[l3pos].time = time;
        updateMachineInfos(machine, &(cache3[l3pos]));
        return &(cache3[l3pos]);
    }
    else if (ramMapping(&RAMpos, machine->ram.blocks, machine->ram.size, add.block, add.word) == 1 /*conferir se ta na RAM através de mapeamento direto, %.*/)
    {
        l1pos = LRU(&machine->l1);
        l2pos = LRU(&machine->l2);
        l3pos = LRU(&machine->l3);

        if (!canOnlyReplaceBlock(cache1[l1pos]))
        {

            /* The block on cache L1 cannot only be replaced, the memories must be updated */
            if (!canOnlyReplaceBlock(cache2[l2pos]))
            {
                /* The block on cache L2 cannot only be replaced, the memories must be updated */
                machine->ram.blocks[cache2[l2pos].tag] = cache2[l2pos].block;

                if (!canOnlyReplaceBlock(cache3[l3pos]))
                    machine->ram.blocks[cache3[l3pos].tag] = cache3[l3pos].block;
                cache3[l3pos] = cache2[l2pos];
            }
            cache2[l2pos] = cache1[l1pos];
        }

        // add.block = add.block % machine->ram.size;
        cache1[l1pos].block.tag = machine->ram.blocks->tag;
        cache1[l1pos].block.words[0] = machine->ram.blocks[add.block].words[0];
        cache1[l1pos].block.words[1] = machine->ram.blocks[add.block].words[1];
        cache1[l1pos].block.words[2] = machine->ram.blocks[add.block].words[2];
        cache1[l1pos].block.words[3] = machine->ram.blocks[add.block].words[3];

        cache1[l1pos].tag = add.block;
        cache1[l1pos].updated = false;
        cache1[l1pos].cost = COST_ACCESS_L1 + COST_ACCESS_L2 + COST_ACCESS_L3 + COST_ACCESS_RAM;
        cache1[l1pos].cacheHit = 4;
    }
    else
    {

        // add.block = add.block % machine->ram.size;
        memoryToRam(machine->ram.blocks, machine->memoryEx.blocks, machine->ram.size, machine->memoryEx.size, add.block);
        // Caso não esteja, está na Memoria Externa e precisa ser levada para a RAM.
        /* Block only in memory RAM, need to bring it to cache and manipulate the blocks */
        l1pos = LRU(&machine->l1);
        l2pos = LRU(&machine->l2);
        l3pos = LRU(&machine->l3);

        if (!canOnlyReplaceBlock(cache1[l1pos]))
        {

            /* The block on cache L1 cannot only be replaced, the memories must be updated */
            if (!canOnlyReplaceBlock(cache2[l2pos]))
            {
                /* The block on cache L2 cannot only be replaced, the memories must be updated */
                machine->ram.blocks[cache2[l2pos].tag] = cache2[l2pos].block;

                if (!canOnlyReplaceBlock(cache3[l3pos]))
                    machine->ram.blocks[cache3[l3pos].tag] = cache3[l3pos].block;
                cache3[l3pos] = cache2[l2pos];
            }
            cache2[l2pos] = cache1[l1pos];
        }
        cache1[l1pos].block = machine->ram.blocks[add.block];
        cache1[l1pos].tag = add.block;
        cache1[l1pos].updated = false;
        cache1[l1pos].cost = COST_ACCESS_L1 + COST_ACCESS_L2 + COST_ACCESS_L3 + COST_ACCESS_RAM;
        cache1[l1pos].cacheHit = 5;
    }

    updateMachineInfos(machine, &(cache1[l1pos]));
    return &(cache1[l1pos]);
}
// atribuir tag à ram

//    tag%tamanho da ram  = i

//  verfiicar se a tag do resultado acima é igual a tag procurada
//  se o item for igual, então otimo achou. Se nao for igual, então ta na memoria externa

// se tiver, verifificar
// receber um valor
// verificar se ta na posição %d
// se sim, retornar o bloco
