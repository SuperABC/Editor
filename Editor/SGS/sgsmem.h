#ifndef MEMORY_H
#define MEMORY_H
#include <iostream>

struct MemBlock {
	void *cont[256];
	unsigned char offset;
	MemBlock *next = NULL;
};

class SgsMemory {
private:
	MemBlock *localMem;
	MemBlock *globeMem;
public:
	SgsMemory();
	~SgsMemory();

	void *alloc(unsigned int size, bool local = true);
	void *alloc(void *ptr, bool local);
	void free(void *ptr);
	void clear();
};


#endif