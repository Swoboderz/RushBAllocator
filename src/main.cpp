//
//  main.cpp
//  RushBAllocator
//
//  Created by Dan Swoboda on 5/11/18.
//  Copyright © 2018 Dan Swoboda. All rights reserved.
//

#include <iostream>

#include "RushBAllocator.h"

struct myStuff {
    int64_t i = 64;
    int64_t k = 64;
    int64_t l = 64;
    int64_t n = 64;
};

struct OtherThing {
    long long k = 20;
};

int main(int argc, const char * argv[]) {
    RushBAllocator allocator;
    
    void* mem = allocator.Allocate(sizeof(myStuff));
    myStuff* stuff = new (mem) myStuff;
    
    mem = allocator.Allocate(sizeof(OtherThing));
    OtherThing* thing = new (mem) OtherThing;
    
    
    
    //Check if allocation works
    std::cout << thing->k << std::endl;
    std::cout << stuff->i << std::endl;
    
    allocator.Free(thing, sizeof(OtherThing));
    
    mem = allocator.Allocate(sizeof(OtherThing));
    thing = new (mem) OtherThing;
    
    //check if re-allocation works (same size)
    std::cout << thing->k << std::endl;
    
    allocator.Free(thing, sizeof(OtherThing));
    
    //check if re-allocation works (different size) NOTE: may use same block due to aligned sizes
    mem = allocator.Allocate(sizeof(myStuff));
    myStuff* newStuff = new (mem) myStuff;
    
    std::cout << newStuff->i << std::endl;
    
    allocator.Free(newStuff, sizeof(myStuff));
    allocator.Free(stuff, sizeof(myStuff));
    return 0;
}
