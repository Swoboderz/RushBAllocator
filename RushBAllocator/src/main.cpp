
//
//  main.cpp
//  RushBAllocator
//
//  Created by Dan Swoboda on 5/11/18.
//  Copyright © 2018 Dan Swoboda. All rights reserved.
//

#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
//#include "EXORIUM_INTERNAL.hpp"
#include "RushBAllocator.h"
//#include "../include/RushBAllocator.h"
struct myStuff {
    int64_t k = 64;
    int64_t l = 90;
    int64_t n = 129;
};

struct OtherThing {
    long long k = 20;
};

template<size_t T>
struct BigBoi {
    int64_t elements[T];
};

int main(int argc, const char * argv[]) {
    //Allocates 1GB of memory, in most cases this is fine.
    RushBAllocator allocator(1e9);
    
    RushBAllocator::RushPointer<myStuff> meme(allocator.Allocate(sizeof(myStuff)), allocator.getStartOfMem());
    allocator.Free(meme.operator->(), sizeof(myStuff));
    
    //srand(time(NULL));

    for (int i = 1; i < 8097; i++) {
        size_t size = i;
        RushBAllocator::RushPointer<char> memory(allocator.Allocate(i), allocator.getStartOfMem());
        allocator.Free(memory.operator->(), size);
    }
    std::cout << std::endl;
    
    //if this line does not return "0xe8f3785546379dcb" followed by "1", that means the memory is not the same as my client, if so please report an issue
    std::cout << allocator.Verify(0xe8f3785546379dcb) << std::endl;
    
    
    
    
    return 0;
}
