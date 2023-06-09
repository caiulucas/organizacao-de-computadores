#include "mmu.h"

#include <stdio.h>

bool canOnlyReplaceBlock(Line line)
{
    // Or the block is empty or
    // the block is equal to the one in memory RAM and can be replaced
    if (line.tag == INVALID_ADD || (line.tag != INVALID_ADD && !line.updated))
        return true;
    return false;
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
    }
    machine->totalCost += line->cost;
}

Line *MMUSearchOnMemories(Address add, Machine *machine)
{
    int l1pos, l2pos, l3pos;
    Line *cache1, *cache2, *cache3;
    MemoryBlock *RAM = machine->ram.blocks;
    // Strategy => write back
    l1pos = memoryCacheMapping(add.block, &machine->l1);
    l2pos = memoryCacheMapping(add.block, &machine->l2);
    l3pos = memoryCacheMapping(add.block, &machine->l3);

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
    else
    {
        /* Block only in memory RAM, need to bring it to cache and manipulate the blocks */
        l2pos = memoryCacheMapping(cache1[l1pos].tag, &machine->l2); /* Need to check the position of the block that will leave the L1 */
        l3pos = memoryCacheMapping(cache2[l2pos].tag, &machine->l3); /* Need to check the position of the block that will leave the L1 */
        if (!canOnlyReplaceBlock(cache1[l1pos]))
        {
            /* The block on cache L1 cannot only be replaced, the memories must be updated */
            if (!canOnlyReplaceBlock(cache2[l2pos]))
            {
                /* The block on cache L2 cannot only be replaced, the memories must be updated */

                if (!canOnlyReplaceBlock(cache3[l3pos]))
                {
                    /* The block on cache L3 cannot only be replaced, the memories must be updated */
                    RAM[cache3[l3pos].tag] = cache3[l3pos].block;
                }
                cache3[l3pos] = cache2[l2pos];
                cache3[l3pos].cacheTime = 0;
            }
            cache2[l2pos] = cache1[l1pos];
            cache2[l2pos].cacheTime = 0;
        }
        cache1[l1pos].block = RAM[add.block];
        cache1[l1pos].tag = add.block;
        cache1[l1pos].updated = false;
        cache1[l1pos].cost = COST_ACCESS_L1 + COST_ACCESS_L2 + COST_ACCESS_L3 + COST_ACCESS_RAM;
        cache1[l1pos].cacheHit = 4;
        cache1[l1pos].cacheTime = 0;
    }

    updateMachineInfos(machine, &(cache1[l1pos]));
    return &(cache1[l1pos]);
}
