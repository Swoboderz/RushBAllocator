//
//  RushBAllocator.c
//  RushBAllocator
//
//  Created by Dan Swoboda on 5/11/18.
//  Copyright © 2018 Dan Swoboda. All rights reserved.
//

#include "RushBAllocator.h"
#include <assert.h>
#include <memory>
#include <iostream>

void* CMAllocate(uint32_t size) {
#if SAFE == 1
    return calloc(1, size);
#else
    return malloc(size);
#endif
}

struct Header {
    size_t next;
    int32_t size;
};

struct Memory {
    size_t HeaderOffset;
    size_t TailOffset;
    int32_t memSize;
    char* mem;
    char* current;
    char* end;
};

const int32_t chunkSize = 16 * 1024;
const int32_t maxBlockSize = 8096;
const int32_t arrblockSize = 254;
const int32_t increment = 128;

//Aligned Sizes
int32_t blockSizes[254] = {
    16, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992, 1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016, 2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040, 3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064, 4096, 4128, 4160, 4192, 4224, 4256, 4288, 4320, 4352, 4384, 4416, 4448, 4480, 4512, 4544, 4576, 4608, 4640, 4672, 4704, 4736, 4768, 4800, 4832, 4864, 4896, 4928, 4960, 4992, 5024, 5056, 5088, 5120, 5152, 5184, 5216, 5248, 5280, 5312, 5344, 5376, 5408, 5440, 5472, 5504, 5536, 5568, 5600, 5632, 5664, 5696, 5728, 5760, 5792, 5824, 5856, 5888, 5920, 5952, 5984, 6016, 6048, 6080, 6112, 6144, 6176, 6208, 6240, 6272, 6304, 6336, 6368, 6400, 6432, 6464, 6496, 6528, 6560, 6592, 6624, 6656, 6688, 6720, 6752, 6784, 6816, 6848, 6880, 6912, 6944, 6976, 7008, 7040, 7072, 7104, 7136, 7168, 7200, 7232, 7264, 7296, 7328, 7360, 7392, 7424, 7456, 7488, 7520, 7552, 7584, 7616, 7648, 7680, 7712, 7744, 7776, 7808, 7840, 7872, 7904, 7936, 7968, 8000, 8032, 8064, 8096
};
uint8_t blockSizeLookup[maxBlockSize + 1];
bool initialized;


RushBAllocator::RushBAllocator()
{
    assert(arrblockSize < UCHAR_MAX);
    mem = new Memory;
    mem->mem = (char*)CMAllocate(1024);
    mem->current = mem->mem;
    mem->end = mem->mem + 1024;
    mem->memSize = 1024;
    mem->HeaderOffset = 0;
    mem->TailOffset = 0;
    
    if (initialized == false)
    {
        int32_t j = 0;
        for (int32_t i = 1; i <= maxBlockSize; ++i)
        {
            assert(j < arrblockSize);
            if (i <= blockSizes[j])
            {
                blockSizeLookup[i] = (uint8_t)j;
            }
            else
            {
                ++j;
                blockSizeLookup[i] = (uint8_t)j;
            }
        }
        
        initialized = true;
    }
}
RushBAllocator::~RushBAllocator(){
    free(mem->mem);
    free(mem);
}


void* RushBAllocator::Allocate(int32_t size){
#if _DEBUG == 1
    assert(size != 0);
    assert(0 < size);
    assert(size + sizeof(Header) < 8096 + 1);
#endif
    int32_t index = blockSizeLookup[size];
    
#if _DEBUG == 1
    assert(0 <= index && index < 255);
#endif
    
    if(freeLists[index]){
        
#if _DEBUG == 1
        std::cout<< "Using Free List!" << std::endl;
#endif
        Header* header = freeLists[index];
        freeLists[index] = (Header*)header->next;
        
        //iterate to the byte AFTER the header
        return (header + 1);
    } else {
        
#if _DEBUG == 1
        std::cout<< "Allocating New!" << std::endl;
#endif
        if(mem->current == mem->end){
            
#if _DEBUG == 1
            std::cout<< "Increasing Memory Size!" << std::endl;
#endif
            uint32_t oldLength = (uint32_t)(mem->current - mem->mem);
            char* newMem = (char*)CMAllocate(oldLength*2);
            memcpy(newMem, mem->mem, oldLength);
            delete [] mem->mem;
            mem->mem = newMem;
            mem->memSize = oldLength*2;
            mem->end = mem->mem + (oldLength*2);
        }
        Header* header = new (mem->current) Header;
        header->next = (size_t)nullptr;
        
        //add to singly linked list
        if((mem->mem + mem->HeaderOffset) == nullptr){
            mem->HeaderOffset = (size_t)((char*)header - mem->mem);
            mem->TailOffset = mem->HeaderOffset;
            header = nullptr;
        } else {
            ((Header*)(mem->mem + mem->TailOffset))->next = (size_t)header;
            mem->TailOffset = (size_t)((char*)header - mem->mem);
        }
        
        header->size = size;
        
        //move current to next open space
        mem->current += sizeof(Header) + size;
        
        //iterate to the byte AFTER the header
        return header + 1;
    }
}

void RushBAllocator::Free(void* p, int32_t size){
#if _DEBUG == 1
    assert(size != 0);
    assert(0 < size);
    assert(size + sizeof(Header) < 8096);
#endif
    int32_t index = blockSizeLookup[size];
    
#if _DEBUG == 1
    assert(0 <= index && index < 255);
#endif
    Header* header = (Header*)p;
    header->next = (size_t)freeLists[index];
    freeLists[index] = (Header*)header;
}


//uint8_t blockSizeLookup[
