//
//  RushBAllocator.c
//  RushBAllocator
//
//  Created by Dan Swoboda on 5/11/18.
//  Copyright © 2018 Dan Swoboda. All rights reserved.
//

#include "RushBAllocator.h"
#include "CRC64.h"
#include <assert.h>
#include <memory>

#ifdef _DEBUG
#include <iostream>
#endif

void* CMAllocate(size_t size) {
#if SAFE == 1
    return calloc(1, size);
#else
    return malloc(size);
#endif
}

static const size_t CalculatePadding(const size_t baseAddress, const size_t alignment) {
    const size_t multiplier = (baseAddress / alignment) + 1;
    const size_t alignedAddress = multiplier * alignment;
    const size_t padding = alignedAddress - baseAddress;
    return padding;
}

static const size_t CalculatePaddingWithHeader(const size_t baseAddress, const size_t alignment, const size_t headerSize) {
    size_t padding = CalculatePadding(baseAddress, alignment);
    size_t neededSpace = headerSize;
    
    if (padding < neededSpace){
        // Header does not fit - Calculate next aligned address that header fits
        neededSpace -= padding;
        
        // How many alignments I need to fit the header
        if(neededSpace % alignment > 0){
            padding += alignment * (1+(neededSpace / alignment));
        }else {
            padding += alignment * (neededSpace / alignment);
        }
    }
    
    return padding;
}



//Aligned Sizes
int32_t blockSizes[254] = {
    16, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992, 1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016, 2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040, 3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064, 4096, 4128, 4160, 4192, 4224, 4256, 4288, 4320, 4352, 4384, 4416, 4448, 4480, 4512, 4544, 4576, 4608, 4640, 4672, 4704, 4736, 4768, 4800, 4832, 4864, 4896, 4928, 4960, 4992, 5024, 5056, 5088, 5120, 5152, 5184, 5216, 5248, 5280, 5312, 5344, 5376, 5408, 5440, 5472, 5504, 5536, 5568, 5600, 5632, 5664, 5696, 5728, 5760, 5792, 5824, 5856, 5888, 5920, 5952, 5984, 6016, 6048, 6080, 6112, 6144, 6176, 6208, 6240, 6272, 6304, 6336, 6368, 6400, 6432, 6464, 6496, 6528, 6560, 6592, 6624, 6656, 6688, 6720, 6752, 6784, 6816, 6848, 6880, 6912, 6944, 6976, 7008, 7040, 7072, 7104, 7136, 7168, 7200, 7232, 7264, 7296, 7328, 7360, 7392, 7424, 7456, 7488, 7520, 7552, 7584, 7616, 7648, 7680, 7712, 7744, 7776, 7808, 7840, 7872, 7904, 7936, 7968, 8000, 8032, 8064, 8096
};
uint32_t blockSizeLookup[maxBlockSize + 1];
bool initialized;

RushBAllocator::RushBAllocator(const size_t totalSize_){
    totalSize = totalSize_;
    startOfMem = (char*)CMAllocate(totalSize_);
    memset(startOfMem, 0, totalSize_);
    offset = 0;
    
    if (initialized == false)
    {
        int32_t j = 0;
        for (int32_t i = 1; i <= maxBlockSize; ++i)
        {
            assert(j < maxBlockSize);
            if (i <= blockSizes[j])
            {
                blockSizeLookup[i] = (uint32_t)j;
            }
            else
            {
                ++j;
                blockSizeLookup[i] = (uint32_t)j;
            }
        }
        
        initialized = true;
    }
}
RushBAllocator::~RushBAllocator(){
    free(startOfMem);
}

size_t RushBAllocator::Allocate(const size_t size){
    size_t padding = CalculatePaddingWithHeader((size_t)(startOfMem + offset), 8, AllocationHeaderSize);
    const std::size_t requiredSize = size + padding;
    
    int32_t index = blockSizeLookup[requiredSize];
    
    
    if (m_freeList[index]) {
        AllocationHeader* ah = (AllocationHeader*)((size_t)startOfMem + m_freeList[index] - 1);
        m_freeList[index] = ah->next;
        return ((size_t)ah + AllocationHeaderSize) - (size_t)startOfMem;
    } else {
        
        
        
        int32_t blockSize = blockSizes[index];
        const std::size_t alignmentPadding =  padding - AllocationHeaderSize;
        
        // Setup data block
        const std::size_t headerAddress =  ((size_t)startOfMem + offset) + alignmentPadding;
        const std::size_t dataAddress = headerAddress + AllocationHeaderSize;
        
        //setup headers
        ((RushBAllocator::AllocationHeader *) headerAddress)->blockSize = requiredSize;
        ((RushBAllocator::AllocationHeader *) headerAddress)->padding = alignmentPadding;
        
        //move offset to end of block
        offset += blockSize;
#ifdef _DEBUG
        std::cout << "A" << "\t@H " << (void*) headerAddress << "\tD@ " <<(void*) dataAddress << "\tS " << ((RushBAllocator::AllocationHeader *) headerAddress)->blockSize <<  "\tAP " << alignmentPadding << "\tP " << padding << "\tM " << offset  << " blockSize " << blockSize << std::endl;
#endif
        
        return dataAddress - (size_t)startOfMem;
    }
    
    
    
    return 0;
}

void RushBAllocator::Free(void* ptr, size_t size){
    
    
    const size_t headerAddress = (size_t) ptr - sizeof (RushBAllocator::AllocationHeader);
    
    RushBAllocator::AllocationHeader * allocationHeader{ (RushBAllocator::AllocationHeader *) headerAddress};
    int32_t index = blockSizeLookup[allocationHeader->blockSize];
    allocationHeader->next = m_freeList[index];
    m_freeList[index] = (headerAddress - (size_t)startOfMem) + 1;
    memset(ptr, 0, size);
    
}

void* RushBAllocator::OffsetToPointer(size_t offset_){
    return (void*)(startOfMem + offset_);
}

uint64_t RushBAllocator::Verify(uint64_t checksum){
#ifdef _DEBUG
    std::cout << "0x" << std::hex << crc64(0, (unsigned char*)startOfMem, offset) << std::endl;
#endif
    return checksum == crc64(0, (unsigned char*)startOfMem, offset);
}

RushBAllocator::Data RushBAllocator::GetData(){
    return {startOfMem, (size_t)startOfMem + offset};
}

void RushBAllocator::SetData(RushBAllocator::Data data){
    memset(startOfMem, 0, (size_t)startOfMem + offset);
    memcpy(startOfMem, data.mem, data.size);
}
