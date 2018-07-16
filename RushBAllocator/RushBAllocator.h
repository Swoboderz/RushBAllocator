#ifndef RUSHBALLOCATOR_H_
#define RUSHBALLOCATOR_H_


#include <stdint.h>
#include <cstddef>


#define MAX_BLOCKSIZE 8096
#define ARR_SIZE 254

extern void* g_MemoryStart;

class RushBAllocator{
public:

	RushBAllocator(const size_t startingSize = 2048, char* address = nullptr);
	void* Allocate(int32_t size);
	void Free(void* ptr, int32_t size);

private:
	size_t totalSize = 0;
	size_t offset = 0;


	struct AllocationHeader {
	    size_t next = 0;
	    size_t blockSize = 0;
	    char padding = 0;
	};

	size_t m_freeList[ARR_SIZE] = {0};


	const int32_t AllocationHeaderSize = sizeof(AllocationHeader);
};


#endif /* RUSHBALLOCATOR_H_ */
