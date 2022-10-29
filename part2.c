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
#include <stdlib.h>
#include <math.h>

char *trace_file_name;

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

void readFileAndDoCacheHitOrMiss(int totalNumberOfBlocks, int nway, int blockSize)
{
    // Every thing is ready , just call the file and do the hit
    int numberOfSets = totalNumberOfBlocks / nway;
    struct direct_mapped_cache
    {
        unsigned valid_field[totalNumberOfBlocks]; /* Valid field */
        unsigned dirty_field[totalNumberOfBlocks]; /* Dirty field; since we don't distinguish writes and \\
                                             reads in this project yet, this field doesn't really matter */
        uint64_t tag_field[totalNumberOfBlocks];   /* Tag field */
        int hits;                                  /* Hit count */
        int misses;                                /* Miss count */
    };
    struct direct_mapped_cache d_cache;
    /* Initialization */
    for (int i = 0; i < totalNumberOfBlocks; i++)
    {
        d_cache.valid_field[i] = 0;
        d_cache.dirty_field[i] = 0;
        d_cache.tag_field[i] = 0;
    }
    d_cache.hits = 0;
    d_cache.misses = 0;

    char mem_request[20];
    FILE *fp;
    fp = fopen(trace_file_name, "r");
    uint64_t address;

   // printf("%s IS THE FILENAME", trace_file_name);
    while (fgets(mem_request, 20, fp) != NULL)
    {
        // printf("%s", mem_request);
        address = convert_address(mem_request);
        uint64_t block_addr = address >> (unsigned)log2(blockSize);
        int setNumber = block_addr % numberOfSets;
        uint64_t tag = block_addr >> (unsigned)log2(numberOfSets);
        int startIndex = ((int)setNumber) * nway;

        int endIndex = startIndex + nway - 1;

        int hitMade = 0;
        int isAnySpaceEmpty = 0;
        int nwayTemp = nway;
        int loopIndex = startIndex;
        int i = 0;
        //  printf("Coming the tag %d \n",tag);
        for (int j = startIndex; j <= endIndex; j++)
        {
            //   printf("\n\n\n **** At Index: %d Valid Field %d TagField %d",j,d_cache.valid_field[j],d_cache.tag_field[j]);
        }
        // printf("*********************");
        while (nwayTemp > 0)
        {
            i++;
            //  printf("%d ***********", loopIndex);
            if (d_cache.valid_field[loopIndex] && d_cache.tag_field[loopIndex] == tag)
            { /* Cache hit */
                d_cache.hits += 1;
                hitMade = 1;
                break;
            }
            if (d_cache.valid_field[loopIndex] == 0)
            {
                isAnySpaceEmpty = 1;
            }

            loopIndex += 1;
            nwayTemp--;
        }

        if (hitMade == 0)
        {
            //   printf("Hit was zero for the index %d", hitMade);
            d_cache.misses += 1;
            if (isAnySpaceEmpty > 0)
            {
                //  printf("Going on empty");
                nwayTemp = nway;
                loopIndex = startIndex;
                while (nwayTemp > 0)
                {
                    if (d_cache.valid_field[loopIndex] == 0)
                    {
                        //  printf("\nInserting at the index : %d ", loopIndex);
                        d_cache.valid_field[loopIndex] = 1;
                        d_cache.tag_field[loopIndex] = tag;
                        break;
                    }

                    loopIndex += 1;
                    nwayTemp--;
                }
            }
            else
            {
                // pick a random index and replace

                int randomIndex = (rand() % (endIndex - startIndex + 1)) + startIndex;
                //   printf("Picking a rand variable %d",randomIndex);
                d_cache.valid_field[randomIndex] = 1;
                d_cache.tag_field[randomIndex] = tag;
            }
        }
    }
    // printf("\n\n\n==================================\n");
    // printf("Cache type:n-Mapped Cache\n with no of rows: %d, number of sets:  %d", totalNumberOfBlocks, numberOfSets);
    printf("==================================\n");
    printf("Cache Hits:    %d\n", d_cache.hits);
    printf("Cache Misses:  %d\n", d_cache.misses);
    printf("Cache Hit Rate : %0.1f%%\n", ((float)d_cache.hits / (float)(d_cache.hits + d_cache.misses))*100);
    printf("Cache Miss Rate : %0.1f%%\n", ((float)d_cache.misses / (float)(d_cache.hits + d_cache.misses))*100);
    printf("\n");
    fclose(fp);
}

void startProcess(int cacheSize, int totalNumberOfBlocks, int nway, int blockSize)
{
    // printf("The below output is for cache with %dKB and %d bytes and %d way associative ", cacheSize, blockSize, nway);
    readFileAndDoCacheHitOrMiss(totalNumberOfBlocks, nway, blockSize);
}

void performNwayAssociativeMappingforPart1()
{
    int continueSelect = 0;
    do
    {
         long cacheSize = 32 * 1024;
    int totalNumberOfBlocks = 0;
    int ca = 0;
    printf("For execution over 32KB Cache size, Click 1 for 16 bytes cache line size, 2 for 32 bytes cache line size, 3 for 128 bytes cache line size : \n");
    scanf("%d", &ca);
    int selection = 0;
    printf("Select 1 for fully associative execution, 2 for 2 way execution, 4 for 4 way execution, 8 for 8 way execution: ");
    scanf("%d", &selection);
    switch (ca)
    {
    case 1:
        printf("\n\n*********** STARTING EXECUTION FOR 16 BYTES cache line size **************** \n");
        totalNumberOfBlocks = (int)cacheSize / 16;
        switch (selection)
        {
        case 8:
            startProcess(32, totalNumberOfBlocks, 8, 16);
            break;
        case 4:
            startProcess(32, totalNumberOfBlocks, 4, 16);
            break;
        case 2:
            startProcess(32, totalNumberOfBlocks, 2, 16);
            break;
        case 1:
            startProcess(32, totalNumberOfBlocks, totalNumberOfBlocks, 16);
            break;
        default:
            break;
        }

        break;
    case 2:
        printf("\n\n*********** STARTING EXECUTION FOR 32 BYTES cache line size **************** \n");
        totalNumberOfBlocks = (int)cacheSize / 32;
        switch (selection)
        {
        case 8:
            startProcess(32, totalNumberOfBlocks, 8, 32);
            break;
        case 4:
            startProcess(32, totalNumberOfBlocks, 4, 32);
            break;
        case 2:
            startProcess(32, totalNumberOfBlocks, 2, 32);
            break;
        case 1:
            startProcess(32, totalNumberOfBlocks, totalNumberOfBlocks, 32);
            break;
        default:
            break;
        }

        break;
    case 3:
        printf("\n\n*********** STARTING EXECUTION FOR 128 BYTES cache line size **************** \n");
        totalNumberOfBlocks = (int)cacheSize / 128;
        switch (selection)
        {
        case 8:
            startProcess(32, totalNumberOfBlocks, 8, 128);
            break;
        case 4:
            startProcess(32, totalNumberOfBlocks, 4, 128);
            break;
        case 2:
            startProcess(32, totalNumberOfBlocks, 2, 128);
            break;
        case 1:
            startProcess(32, totalNumberOfBlocks, totalNumberOfBlocks, 1628);
            break;
        default:
            break;
        }

        break;
    }
    printf("=============== EXECUTION ENDED ===============\n");



        printf("Select 1 to continue , any other to exit: ");
        scanf("%d",&continueSelect);
    } while (continueSelect==1);
    

   
}
void performNwayAssociativeMappingforPart2()
{

     int continueSelect = 0;
    do
    {
 long cacheSize = 16 * 1024;
    int totalNumberOfBlocks = 0;
    int ca = 0;
    printf("For execution over 64Bytes Cache line size ,Click 1 for 16KB Cachesize, 2 for 32KB Cachesize, 3 for 64KB Cachesize \n  \n");
    scanf("%d", &ca);
    int selection;
    printf("Select 1 for fully associative execution, 2 for 2 way execution, 4 for 4 way execution, 8 for 8 way execution: ");
    scanf("%d", &selection);
 
        switch (ca)
        {
        case 1:
            totalNumberOfBlocks = (int)cacheSize / 64;
            printf("\n\n*********** STARTING EXECUTION OF  16kb cache size and 64 bytes cache line size/block size=======\n");
            switch (selection)
            {
            case 8:
                startProcess(16, totalNumberOfBlocks, 8, 64);
                break;
            case 4:
                startProcess(16, totalNumberOfBlocks, 4, 64);
                break;
            case 2:
                startProcess(16, totalNumberOfBlocks, 2, 64);
                break;
            case 1:
                startProcess(16, totalNumberOfBlocks, totalNumberOfBlocks, 64);
                break;

            default:
                break;
            }

            break;
        case 2:
            printf("\n\n*********** STARTING EXECUTION OF  32kb cache size and 64 bytes cache line size/block size=======\n");
            cacheSize = 32 * 1024;
            totalNumberOfBlocks = (int)cacheSize / 64;
            switch (selection)
            {
            case 8:
                startProcess(32, totalNumberOfBlocks, 8, 64);
                break;
            case 4:
                startProcess(32, totalNumberOfBlocks, 4, 64);
                break;
            case 2:
                startProcess(32, totalNumberOfBlocks, 2, 64);
                break;
            case 1:
                startProcess(32, totalNumberOfBlocks, totalNumberOfBlocks, 64);
                break;

            default:
                break;
            }
            break;
        case 3:
            printf("\n\n*********** STARTING EXECUTION OF  64kb cache size and 64 bytes cache line size/block size=======\n");
            cacheSize = 64 * 1024;
            totalNumberOfBlocks = (int)cacheSize / 64;
            switch (selection)
            {
            case 8:
                startProcess(64, totalNumberOfBlocks, 8, 64);
                break;
            case 4:
                startProcess(64, totalNumberOfBlocks, 4, 64);
                break;
            case 2:
                startProcess(64, totalNumberOfBlocks, 2, 64);
                break;
            case 1:
                startProcess(64, totalNumberOfBlocks, totalNumberOfBlocks, 64);
                break;

            default:
                break;
            }
            break;

        default:
            break;
        }


        printf("Select 1 to continue , any other to exit: ");
        scanf("%d",&continueSelect);
    } while (continueSelect==1);


   
}

int main(int argc, char *argv[])
{
    trace_file_name = argv[2];
    int switchCase = 0;
    printf("Select 1 to  execute Question 1 or 2 to Question 2 :");
    scanf("%d", &switchCase);

    switch (switchCase)
    {
    case 1:
        performNwayAssociativeMappingforPart1();
        break;

    case 2:
        performNwayAssociativeMappingforPart2();
        break;
    }
}