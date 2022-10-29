/*
 * CS3375 Computer Architecture
 * Course Project
 * Cache Simulator Design and Development
 * FALL 2017
 * By Yong Chen
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "cachesim.h"

int main(int argc, char *argv[])
{
   
    if (argc != 3) {
        printf("Usage: %s <direct> <trace file name>\n", argv[0]);
        return 1;
    }

#ifdef DBG
    printf("BLOCK SIZE = %d Bytes\n", BLOCK_SIZE);
    printf("%d-WAY\n", WAY_SIZE);
    printf("CACHE SIZE = %d Bytes\n", CACHE_SIZE);
    printf("NUMBER OF BLOCKS = %d\n", NUM_BLOCKS);
    printf("NUMBER OF SETS = %d\n", NUM_SETS);
    printf("\n");
#endif

    char* trace_file_name = argv[2];
    struct direct_mapped_cache dir_cache;
    char mem_request[20];
    uint64_t address;
    FILE *fp;


    /* variable_set_up */
    for (int i=0; i<NUM_BLOCKS; i++) {
        dir_cache.valid_field[i] = 0;
        dir_cache.dirty_field[i] = 0;
        dir_cache.tag_field[i] = 0;
    }
    dir_cache.hits = 0;
    dir_cache.misses = 0;


    /*Memory_trace_file_opening_part*/
    fp = fopen(trace_file_name, "r");

    if (strncmp(argv[1], "direct", 6)==0) { 
        /* Direct-mapped cache simulation */
        /*Access the cache by reading the memory request */
        while (fgets(mem_request, 20, fp)!= NULL) {
            address = convert_address(mem_request);
            direct_mapped_cache_access(&dir_cache, address);
         //   printf("Looping");
        }
        /*Printing out the results*/
        printf("\n==================================\n");
        printf("The Cache type:    Direct-Mapped Cache\n");
        printf("*************************************\n");
        printf("Total_Cache_Hits:    %d\n", dir_cache.hits);
        printf("Total_Cache_Misses:  %d\n", dir_cache.misses);
        printf("Total_Cache_Hit-Rate:    %f%%\n", ((float)dir_cache.hits / ((float)dir_cache.hits + (float)dir_cache.misses)) *100);
        printf("Total_Miss-Rate:    %f%%\n", ((float)dir_cache.misses / ((float)dir_cache.hits + (float)dir_cache.misses))*100);
        
    }

    fclose(fp);

    return 0;
}

uint64_t convert_address(char memory_addr[])
/* Conversion of the physical 32-bit address in the trace file to the  form of "binary" \\
 * (a uint64 that can have bitwise operations on it) */
{
    uint64_t binary1 = 0;
    int i = 0;

    while (memory_addr[i] != '\n') {
        if (memory_addr[i] <= '9' && memory_addr[i] >= '0') {
            binary1 = (binary1*16) + (memory_addr[i] - '0');
        } else {
            if(memory_addr[i] == 'a' || memory_addr[i] == 'A') {
                binary1 = (binary1*16) + 10;
            }
            if(memory_addr[i] == 'b' || memory_addr[i] == 'B') {
                binary1 = (binary1*16) + 11;
            }
            if(memory_addr[i] == 'c' || memory_addr[i] == 'C') {
                binary1 = (binary1*16) + 12;
            }
            if(memory_addr[i] == 'd' || memory_addr[i] == 'D') {
                binary1 = (binary1*16) + 13;
            }
            if(memory_addr[i] == 'e' || memory_addr[i] == 'E') {
                binary1 = (binary1*16) + 14;
            }
            if(memory_addr[i] == 'f' || memory_addr[i] == 'F') {
                binary1 = (binary*16) + 15;
            }
        }
        i++;
    }

#ifdef DBG
    printf("%s converted to %llu\n", memory_addr, binary1);
#endif
    return binary1;
}

void direct_mapped_cache_access(struct direct_mapped_cache *cache, uint64_t address)
{
    uint64_t block_addr = address >> (unsigned)log2(BLOCK_SIZE);
    uint64_t index = block_addr % NUM_BLOCKS;
    uint64_t tag = block_addr >> (unsigned)log2(NUM_BLOCKS);

#ifdef DBG
    printf("The Memory address: %llu, The Block address: %llu, Index is: %llu, Tag is: %llu ", address, block_addr, index, tag);
#endif

    if (cache->valid_field[index] && cache->tag_field[index] == tag) { /* Cache hit */
        cache->hits += 1;
#ifdef DBG
        printf("Hit!\n");
#endif
    } else {
        /* Cache miss */
        cache->misses += 1;
#ifdef DBG
        printf("Miss!\n");
#endif
        if (cache->valid_field[index] && cache->dirty_field[index]) {
            /* Write the cache block back to memory */
        }
        cache->tag_field[index] = tag;
        cache->valid_field[index] = 1;
        cache->dirty_field[index] = 0;
    }
}
