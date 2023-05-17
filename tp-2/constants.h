#include <stdbool.h>

#define WORDS_SIZE 4
#define INVALID_ADD -1
#define COST_ACCESS_L1 1
#define COST_ACCESS_L2 10
#define COST_ACCESS_L3 100
#define COST_ACCESS_RAM 1000
#define MAP_METHOD 1 // 1 = LRU, 2 = LFU

#define PRINT_INTERMEDIATE_RAM
#define PRINT_LOG
