#include "mmu.h"
#include "instruction.h"
#include "generator.h"
#include "file.h"
#include "cpu.h"
#include <time.h>
#include <stdio.h>

bool canOnlyReplaceBlock(Line line)
{
    // Or the block is empty or
    // the block is equal to the one in memory RAM and can be replaced
    if (line.tag == INVALID_ADD || (line.tag != INVALID_ADD && !line.updated))
        return true;
    return false;
}

bool canOnlyReplaceBlockRAM(MemoryBlock block)
{
    // Or the block is empty or
    // the block is equal to the one in memory RAM and can be replaced
    if (block.tag == INVALID_ADD || (block.tag != INVALID_ADD && !block.updated))
        return true;
    return false;
}

int ramMapping(int address, RAM *RAM)
{
    int pos = 0;
    for (int i = 0; i < RAM->size; i++)
    {
        if (RAM->blocks[i].tag == (address % RAM->size))
        {
            pos = i;
        }
    }

    return pos;
}

int memoryCacheMapping(int address, Cache *cache)
{
    int pos = 0;

    switch (MAP_METHOD)
    {
    case 1:
        // Least recently used: increment the times used for all lines, and return the line with the largest times used
        for (int i = 0; i < cache->size; i++)
        {
            cache->lines[i].cacheTime++;
            if (cache->lines[i].cacheTime > cache->lines[pos].cacheTime)
            {
                pos = i;
            }

            if (cache->lines[i].tag == address)
                return i;
        }
        cache->lines[pos].timesUsed++;
        return pos;
    case 2:
        // Least frequently used: return the line with the largest times used
        for (int i = 0; i < cache->size; i++)
        {
            if (cache->lines[i].timesUsed < cache->lines[pos].timesUsed)
            {
                pos = i;
            }

            if (cache->lines[i].tag == address)
                return i;
        }

        return pos;
    default:
        return address % cache->size;
    }
}

void updateMachineInfos(Machine *machine, Line *line)
{
    line->timesUsed++;
    line->cacheTime = 0;

    switch (line->cacheHit)
    {

    case 1:
        machine->hitL1 += 1;
        break;

    case 2:
        machine->hitL2 += 1;
        machine->missL1 += 1;
        break;

    case 3:
        machine->hitL3 += 1;
        machine->missL1 += 1;
        machine->missL2 += 1;
        break;

    case 4:
        machine->hitRAM += 1;
        machine->missL1 += 1;
        machine->missL2 += 1;
        machine->missL3 += 1;
        break;

    case 5:
        machine->missL1 += 1;
        machine->missL2 += 1;
        machine->missL3 += 1;
        machine->missRAM += 1;
        machine->hitStorage += 1;
        break;
    }
    machine->totalCost += line->cost;
}

void storageToRAM(RAM *RAM, Storage *storage, int tag)
{
    int ramPos = 0, storagePos = 0;
    for (int i = 0; i < storage->size; i++)
    {
        if (storage->blocks[i].tag == tag)
        {
            storagePos = i;

            ramPos = tag % RAM->size;
            MemoryBlock aux;
            aux = storage->blocks[storagePos];
            storage->blocks[storagePos] = RAM->blocks[ramPos];
            RAM->blocks[ramPos] = aux;

            saveStorage(*storage);

            return;
        }
    }
}

Line *MMUSearchOnMemories(Address add, Machine *machine)
{
    int l1pos, l2pos, l3pos, ramPos;
    Line *cache1, *cache2, *cache3;
    MemoryBlock *RAM;
    // Strategy => write back
    l1pos = memoryCacheMapping(add.block, &machine->l1);
    l2pos = memoryCacheMapping(add.block, &machine->l2);
    l3pos = memoryCacheMapping(add.block, &machine->l3);
    ramPos = ramMapping(add.block, &machine->ram);

    cache1 = machine->l1.lines;
    cache2 = machine->l2.lines;
    cache3 = machine->l3.lines;
    RAM = machine->ram.blocks;

    // Direct memory map

    if (cache1[l1pos].tag == add.block)
    {
        /* Block is in memory cache L1 */
        cache1[l1pos].cost = COST_ACCESS_L1;
        cache1[l1pos].cacheHit = 1;
    }
    else if (cache2[l2pos].tag == add.block)
    {
        /* Block is in memory cache L2 */
        cache2[l2pos].tag = add.block;
        cache2[l2pos].updated = false;
        cache2[l2pos].cost = COST_ACCESS_L1 + COST_ACCESS_L2;
        cache2[l2pos].cacheHit = 2;
        // !Can be improved?
        updateMachineInfos(machine, &(cache2[l2pos]));
        return &(cache2[l2pos]);
    }
    else if (cache3[l3pos].tag == add.block)
    {
        /* Block is in memory cache L3 */
        cache3[l3pos].tag = add.block;
        cache3[l3pos].updated = false;
        cache3[l3pos].cost = COST_ACCESS_L1 + COST_ACCESS_L2 + COST_ACCESS_L3;
        cache3[l3pos].cacheHit = 3;
        // !Can be improved?
        updateMachineInfos(machine, &(cache3[l3pos]));
        return &(cache3[l3pos]);
    }
    else if (RAM[ramPos].tag == add.block)
    {
        RAM[ramPos].tag = add.block;
        RAM[ramPos].updated = false;
        RAM[ramPos].cost = COST_ACCESS_L1 + COST_ACCESS_L2 + COST_ACCESS_L3 + COST_ACCESS_RAM;
        RAM[ramPos].hit = 4;

        storageToRAM(&machine->ram, &machine->storage, add.block);
    }
    else
    {
        l2pos = memoryCacheMapping(cache1[l1pos].tag, &machine->l2); /* Need to check the position of the block that will leave the L1 */
        l3pos = memoryCacheMapping(cache2[l2pos].tag, &machine->l3); /* Need to check the position of the block that will leave the L2 */
        ramPos = ramMapping(cache3[l3pos].tag, &machine->ram);       /* Need to check the position of the block that will leave the L3 */
        // add.block = add.block % machine->ram.size;

        storageToRAM(&machine->ram, &machine->storage, add.block);
        // Caso não esteja, está na Memoria Externa e precisa ser levada para a RAM.
        /* Block only in memory RAM, need to bring it to cache and manipulate the blocks */

        if (!canOnlyReplaceBlock(cache1[l1pos]))
        {

            /* The block on cache L1 cannot only be replaced, the memories must be updated */
            if (!canOnlyReplaceBlock(cache2[l2pos]))
            {
                /* The block on cache L2 cannot only be replaced, the memories must be updated */
                machine->ram.blocks[cache2[l2pos].tag] = cache2[l2pos].block;

                if (!canOnlyReplaceBlock(cache3[l3pos]))
                {

                    if (!canOnlyReplaceBlockRAM(RAM[ramPos]))
                    {
                        /* The block on memory RAM cannot only be replaced, the storage must be updated */
                        storageToRAM(&machine->ram, &machine->storage, RAM[ramPos].tag);
                    }
                    machine->ram.blocks[cache3[l3pos].tag] = cache3[l3pos].block;
                }
                cache3[l3pos] = cache2[l2pos];
            }
            cache2[l2pos] = cache1[l1pos];
        }
        cache1[l1pos].block = machine->ram.blocks[add.block];
        cache1[l1pos].tag = add.block;
        cache1[l1pos].updated = false;
        cache1[l1pos].cost = COST_ACCESS_L1 + COST_ACCESS_L2 + COST_ACCESS_L3 + COST_ACCESS_RAM + COST_ACCESS_STORAGE;
        cache1[l1pos].cacheHit = 5;
    }

    updateMachineInfos(machine, &(cache1[l1pos]));
    return &(cache1[l1pos]);
}
