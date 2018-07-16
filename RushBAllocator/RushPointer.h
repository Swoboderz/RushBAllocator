#ifndef RUSHPOINTER_H_
#define RUSHPOINTER_H_

extern void* g_MemoryStart;

#include "CRC64.h"
#include <stdint.h>
#include <cstddef>


template <typename T>
class FastRushPointer {

    public:
        T* address;
        size_t offset;

        FastRushPointer(size_t value) : address((T*)((size_t)g_MemoryStart + value)) {
            offset = value;
        }
        ~FastRushPointer(){

        }

        inline T* Fix(){
            address = (T*)((size_t)g_MemoryStart + offset);
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

template<typename T>
class VerifiableRushPointer {

    public:
        size_t offset;
        VerifiableRushPointer(size_t value){
            offset = value;
        }
        ~VerifiableRushPointer(){

        }

        /// NOTE: if object type or variable contains a pointer type that is not of the VerifiableRushPointer class, the checksum will most likely return false
        inline bool Verify(uint64_t checksum){
            return checksum = crc64(0, (const unsigned char*)((size_t)g_MemoryStart + offset), sizeof(T));
        }

        inline T& operator* (){
            return *(T*)((size_t)g_MemoryStart + offset);
        }
        inline T* operator-> (){
            return (T*)((size_t)g_MemoryStart + offset);
        }
};

template<typename T>
class UnsafeRushPointer {

    public:
        T* address;
        UnsafeRushPointer(size_t value) : address((T*)((size_t)g_MemoryStart + value)){
        }
        ~UnsafeRushPointer(){}

        inline T& operator* ()
        {
            return *address;
        }

        inline T* operator-> ()
        {
            return address;
        }
};



#endif /* RUSHPOINTER_H_ */
