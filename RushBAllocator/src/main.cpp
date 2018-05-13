
//
//  main.cpp
//  RushBAllocator
//
//  Created by Dan Swoboda on 5/11/18.
//  Copyright © 2018 Dan Swoboda. All rights reserved.
//

#include <iostream>
#include <vector>
#include "../include/RushBAllocator.h"

struct myStuff {
    int64_t k = 64;
    int64_t l = 90;
    int64_t n = 129;
};

struct OtherThing {
    long long k = 20;
};

int main(int argc, const char * argv[]) {
    //Allocates 1GB of memory, in most cases this is fine.
    RushBAllocator allocator(1e9);
    
    size_t meme = allocator.Allocate(sizeof(myStuff));
    
    void* trueMem = allocator.OffsetToPointer(meme);
    
    myStuff* stuff = new (trueMem) myStuff;
    meme = allocator.Allocate(sizeof(myStuff));
    trueMem = allocator.OffsetToPointer(meme);
    myStuff* stuff2 = new (trueMem) myStuff;
    meme = allocator.Allocate(sizeof(myStuff));
    trueMem = allocator.OffsetToPointer(meme);
    myStuff* stuff3 = new (trueMem) myStuff;
    
    allocator.Free(stuff, sizeof(myStuff));
    allocator.Free(stuff2, sizeof(myStuff));
    allocator.Free(stuff3, sizeof(myStuff));
    
    std::vector<myStuff*> stuffs;
    for (int i = 0; i < 200000; i++) {
        size_t tempMem = allocator.Allocate(sizeof(myStuff));
        void* tempTrueMem = allocator.OffsetToPointer(tempMem);
        stuffs.push_back(new (tempTrueMem) myStuff);
    }
    for (int i = 0; i < 200000; i++) {
        allocator.Free(stuffs[i], sizeof(myStuff));
    }
    std::cout << std::endl;
    
    //if this line does not return "0x48d99418278c1656" followed by "1", that means the memory is not the same as my client, if so please report an issue
    std::cout << allocator.Verify(0x48d99418278c1656) << std::endl;
    
    return 0;
}
