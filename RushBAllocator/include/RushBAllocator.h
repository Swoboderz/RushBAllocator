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
#include <vector>

struct Header;
struct Memory;

class RushBAllocator {
public:
    RushBAllocator();
    ~RushBAllocator();
    
    
    void* Allocate(int32_t size);
    void Free(void* p, int32_t size);
    
    void Empty();
    
private:
    Memory* mem;
    
    //std::vector<std::vector<Header*>> freeList;
    Header* freeLists[254];
};



#endif /* RushBAllocator_h */
