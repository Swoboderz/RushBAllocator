INC_DIR = /include


output: CRC64.o RushBAllocator.o
	g++ CRC64.o RushBAllocator.o -o output


main.o: main.cpp
	g++ -c main.cpp

RushBAllocator.o: RushBAllocator.cpp RushBAllocator.h CRC64.h
	g++ -c RushBAllocator.cpp

CRC64.o: CRC64.h
	g++ -c CRC64.h

clean:
	rm *.o