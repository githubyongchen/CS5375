#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

char *trace_file_name;

struct L1Cache
{
    unsigned valid_field[1024];
    unsigned dirty_field[1024];
    uint64_t tag_field[1024];
    char data_field[1024][64];
    int hits;
    int misses;
};

struct L2Cache
{
    unsigned valid_field[16384];
    unsigned dirty_field[16384];
    uint64_t tag_field[16384];
    char data_field[16384][64];
    int hits;
    int misses;
};

uint64_t convert_address(char memory_addr[])
/* Converts the physical 32-bit address in the trace file to the "binary" \\
 * (a uint64 that can have bitwise operations on it) */
{
    uint64_t binary = 0;
    int i = 0;

    while (memory_addr[i] != '\n')
    {
        if (memory_addr[i] <= '9' && memory_addr[i] >= '0')
        {
            binary = (binary * 16) + (memory_addr[i] - '0');
        }
        else
        {
            if (memory_addr[i] == 'a' || memory_addr[i] == 'A')
            {
                binary = (binary * 16) + 10;
            }
            if (memory_addr[i] == 'b' || memory_addr[i] == 'B')
            {
                binary = (binary * 16) + 11;
            }
            if (memory_addr[i] == 'c' || memory_addr[i] == 'C')
            {
                binary = (binary * 16) + 12;
            }
            if (memory_addr[i] == 'd' || memory_addr[i] == 'D')
            {
                binary = (binary * 16) + 13;
            }
            if (memory_addr[i] == 'e' || memory_addr[i] == 'E')
            {
                binary = (binary * 16) + 14;
            }
            if (memory_addr[i] == 'f' || memory_addr[i] == 'F')
            {
                binary = (binary * 16) + 15;
            }
        }
        i++;
    }

#ifdef DBG
    printf("%s converted to %llu\n", memory_addr, binary);
#endif
    return binary;
}


int isDataExistsInCacheL1(uint64_t address, int nway, struct L1Cache *l1)
{
    uint64_t block_addr = address >> (unsigned)log2(64);
    int setNumber = block_addr % 512;
    uint64_t tag = block_addr >> (unsigned)log2(512);
    int startIndex = ((int)setNumber) * nway;
    int nwayTemp = nway;
    int loopIndex = startIndex;
    while (nwayTemp > 0)
    {
        if (l1->valid_field[loopIndex] && l1->tag_field[loopIndex] == tag)
        {
            return 1;
        }
        loopIndex += 1;
        nwayTemp--;
    }
    return 0;
}
int isDataExistsInCacheL2(uint64_t address, int nway, struct L1Cache *l2)
{
    uint64_t block_addr = address >> (unsigned)log2(64);
    int setNumber = block_addr % 2048;
    uint64_t tag = block_addr >> (unsigned)log2(2048);
    int startIndex = ((int)setNumber) * nway;
    int nwayTemp = nway;
    int loopIndex = startIndex;
    while (nwayTemp > 0)
    {
        if (l2->valid_field[loopIndex] && l2->tag_field[loopIndex] == tag)
        {
            return 1;
        }
        loopIndex += 1;
        nwayTemp--;
    }
    return 0;
}
void insertDataInL1Cache(uint64_t address, int nway, struct L1Cache *l1)
{
    uint64_t block_addr = address >> (unsigned)log2(64);
    int setNumber = block_addr % 512;
    uint64_t tag = block_addr >> (unsigned)log2(512);
    int startIndex = ((int)setNumber) * nway;
    int nwayTemp = nway;
    int loopIndex = startIndex;
    int isAnySpaceEmpty = 0;
    int endIndex = startIndex + nway - 1;
    while (nwayTemp > 0)
    {
        if (l1->valid_field[loopIndex] == 0)
        {
            isAnySpaceEmpty = 1;
        }
        loopIndex++;
        nwayTemp--;
    }
    if (isAnySpaceEmpty > 0)
    {
        nwayTemp = nway;
        loopIndex = startIndex;
        while (nwayTemp > 0)
        {
            if (l1->valid_field[loopIndex] == 0)
            {
                l1->valid_field[loopIndex] = 1;
                l1->tag_field[loopIndex] = tag;
                break;
            }

            loopIndex += 1;
            nwayTemp--;
        }
    }
    else
    {
        int randomIndex = (rand() % (endIndex - startIndex + 1)) + startIndex;
        //   printf("Picking a rand variable %d",randomIndex);
        l1->valid_field[randomIndex] = 1;
        l1->tag_field[randomIndex] = tag;
    }
}
void insertDataInL2Cache(uint64_t address, int nway, struct L1Cache *l2)
{

    uint64_t block_addr = address >> (unsigned)log2(64);
    int setNumber = block_addr % 2048;
    uint64_t tag = block_addr >> (unsigned)log2(2048);
    int startIndex = ((int)setNumber) * nway;
    int nwayTemp = nway;
    int loopIndex = startIndex;
    int isAnySpaceEmpty = 0;
    int endIndex = startIndex + nway - 1;
    while (nwayTemp > 0)
    {
        if (l2->valid_field[loopIndex] == 0)
        {
            isAnySpaceEmpty = 1;
        }
        loopIndex++;
        nwayTemp--;
    }
    if (isAnySpaceEmpty > 0)
    {
        nwayTemp = nway;
        loopIndex = startIndex;
        while (nwayTemp > 0)
        {
            if (l2->valid_field[loopIndex] == 0)
            {
                l2->valid_field[loopIndex] = 1;
                l2->tag_field[loopIndex] = tag;
                break;
            }

            loopIndex += 1;
            nwayTemp--;
        }
    }
    else
    {
        int randomIndex = (rand() % (endIndex - startIndex + 1)) + startIndex;
        //   printf("Picking a rand variable %d",randomIndex);
        l2->valid_field[randomIndex] = 1;
        l2->tag_field[randomIndex] = tag;
    }
}


int main(int argc, char *argv[])
{
    FILE *fp;
    trace_file_name = argv[2];
    char mem_request[20];
    uint64_t address;
    struct L1Cache l1;
    struct L2Cache l2;
    int numberOfBlocksinl1 = 1024;
    int numberOfBocksinl2 = 16384;
    int l1nway = 2;
    int l2nway = 8;
    int numberOfSetsl1 = 512;
    int numberOfSetsl2 = 2048;
    for (int i = 0; i < numberOfBlocksinl1; i++)
    {
        l1.valid_field[i] = 0;
        l1.dirty_field[i] = 0;
        l1.tag_field[i] = 0;
    }
    for (int i = 0; i < numberOfBocksinl2; i++)
    {
        l2.valid_field[i] = 0;
        l2.dirty_field[i] = 0;
        l2.tag_field[i] = 0;
    }

    l1.hits = 0;
    l1.misses = 0;
    l2.hits = 0;
    l2.misses = 0;

    fp = fopen(trace_file_name, "r");

    if (strncmp(argv[1], "direct", 6) == 0)
    {
        while (fgets(mem_request, 20, fp) != NULL)
        {
            address = convert_address(mem_request);
            int dataInL1 = isDataExistsInCacheL1(address,l1nway,&l1);
            if(dataInL1==1)
            {
                l1.hits++;
                l2.hits++;
            }
            else
            {
                l1.misses++;
                int dataInL2 = isDataExistsInCacheL2(address,l2nway,&l2);
                if(dataInL2)
                {
                    l2.hits+=1;

                }
                else
                {
                    l2.misses++;
                    insertDataInL2Cache(address,l2nway,&l2);
                }
                insertDataInL1Cache(address,l1nway,&l1);
            }
        }
        printf("\n==================================\n");
        printf("Cache type:    Direct-Mapped Cache for l1\n");
        printf("==================================\n");
        printf("Cache Hits:    %d\n", l1.hits);
        printf("Cache Misses:  %d\n", l1.misses);
        printf("\n");
          printf("\n==================================\n");
        printf("Cache type:    Direct-Mapped Cache for l2\n");
        printf("==================================\n");
        printf("Cache Hits:    %d\n", l2.hits);
        printf("Cache Misses:  %d\n", l2.misses);
        printf("\n");
    }

    fclose(fp);

    return 0;
}