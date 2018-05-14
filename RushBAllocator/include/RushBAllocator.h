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

//Outside allocator for allowed reference

class RushBAllocator {
public:
    
    struct Data {
        char * mem;
        size_t size;
    };
    
    RushBAllocator(const size_t totalSize = 1e9);
    
    ~RushBAllocator();
    
    
    
    void Free(void* ptr, size_t size);
    
    void* OffsetToPointer(size_t offset);
    
    uint64_t Verify(uint64_t checksum);
    
    Data GetData();
    
    void SetData(Data data);
    
    void Init();
    
    void Reset();
    
    template <typename T>
    class RushPointer {
        //this MUST be the first element
        T* address;
        size_t offset;
    public:
        RushPointer(size_t value, char* startOfMem) {
            address = (T*)(startOfMem + value);
            offset = value;
        }
        ~RushPointer(){
            
        }
        
        inline T* Fix(char* startOfMem){
            address = (T*)(startOfMem + offset);
            return address;
        }
        
        inline T& operator* ()
        {
            return *address;
        }
        
        inline T* operator-> ()
        {
            return address;
        }
    };

    size_t Allocate(const size_t size);
    
    inline char* getStartOfMem(){
        return startOfMem;
    }
    
private:
    char* startOfMem;
    size_t totalSize = 0;
    size_t offset = 0;
    
    
    struct AllocationHeader {
        size_t next;
        size_t blockSize;
        char padding;
    };
    
    size_t m_freeList[arrblockSize] = {0};
    
    
    const int32_t AllocationHeaderSize = sizeof(AllocationHeader);
};



#endif /* RushBAllocator_h */
