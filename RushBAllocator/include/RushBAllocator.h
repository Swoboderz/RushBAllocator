//
//  RushBAllocator.h
//  RushBAllocator
//
//  Created by Dan Swoboda on 5/11/18.
//  Copyright © 2018 Dan Swoboda. All rights reserved.
//

#ifndef RushBAllocator_h
#define RushBAllocator_h

#define SAFE 1
#define _DEBUG 1

#include <stdint.h>
#include <stdlib.h>

const int32_t maxBlockSize = 8096;
const int32_t arrblockSize = 254;

class RushBAllocator {
public:
    
    struct Data {
        char * mem;
        size_t size;
    };
    
    RushBAllocator(const size_t totalSize);
    
    ~RushBAllocator();
    
    size_t Allocate(const size_t size);
    
    void Free(void* ptr, size_t size);
    
    void* OffsetToPointer(size_t offset);
    
    uint64_t Verify(uint64_t checksum);
    
    Data GetData();
    
    void SetData(Data data);
    
    void Init();
    
    void Reset();
private:
    size_t totalSize;
    
    char* startOfMem;
    size_t offset;
    
    struct FreeHeader {
        size_t blockSize;
    };
    struct AllocationHeader {
        size_t next;
        size_t blockSize;
        char padding;
    };
    
    size_t m_freeList[arrblockSize] = {0};
    
    const int32_t FreeHeaderSize = sizeof(FreeHeader);
    const int32_t AllocationHeaderSize = sizeof(AllocationHeader);
};




#endif /* RushBAllocator_h */
