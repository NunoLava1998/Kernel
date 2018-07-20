#include "rw.h"

void mwrite8(void* a, unsigned char b) {
	unsigned char* c = (unsigned char*)a;
	*c = b;
}

void mwrite16(void* a, unsigned short b) {
	unsigned short* c = (unsigned short*)a;
	*c = b;
}

void mwrite32(void* a, unsigned int b) {
	unsigned int* c = (unsigned int*)a;
	*c = b;
}

void mwritesz(void* a, unsigned int sz, char c) {
	unsigned char* b = (unsigned char*)a;
	for (unsigned int i = 0; i < sz; i++) {
		unsigned char* _c = (unsigned char*)(b + i);
		*_c = c;
	}
}

void mcpy(void* a, void* b, unsigned int c) {
	while (a < (b + c)) 
		*((unsigned char*)a++) = *((unsigned char*)b++);
}

void mnull8(void* a) {
	unsigned char* b = (unsigned char*)a;
	*b = 0;
}

void mnull16(void* a) {
	unsigned short* b = (unsigned short*)a;
	*b = 0;
}

void mnull32(void* a) {
	unsigned int* b = (unsigned int*)a;
	*b = 0;
}

void mnullsz(void* a, unsigned int sz) {
	unsigned char* b = (unsigned char*)a;
	for (unsigned int i = 0; i < sz; i++) {
		unsigned char* _c = (unsigned char*)(b + i);
		*_c = 0;
	}
}

unsigned char mget8(void* a) {
	unsigned char* b = (unsigned char*)a;
	return *b;
}
unsigned short mget16(void* a) {
	unsigned short* b = (unsigned short*)a;
	return *b;
}
unsigned int mget32(void* a) {
	unsigned int* b = (unsigned int*)a;
	return *b;
}
/*
static unsigned char* findblock(unsigned int size, unsigned int hedsize_low, unsigned int hedsize_hi) {
	// This function finds a block for malloc() and returns it's address.
	// First, we need to outline the free areas that we can guarantee to be conventional memory.
	struct __goodmemory__ {
		// There is around 30 KB of code between 0x500 and 0x7BFF. However, this is reserved for real mode stuff.
		unsigned char* start1; // 0x07E00
		unsigned char* end1; // 0x7FFFF (total: 480.5 KiB)
		unsigned char* start2; // 0x1000000 + kernel size
		unsigned char* end2; // 0xC0000000 or end of memory
	} goodmemory;
	// Second, the necessary variables we will need.
	unsigned int hedsize = hedsize_low + hedsize_hi;
	unsigned int sz = (size + hedsize); // The last parameter is to guarantee the correct size.
}

unsigned char* malloc(unsigned int size) {
	// Structs for our memory allocation.
	struct stblock {
		unsigned int magicnumber; // "USD" + 0x8A. Stands for "used".
		unsigned int blocksize; // Size of the block. This is 32 bits, which means it reaches up to ~4GB only.
		unsigned int padding_low; // Padding (0x00000000)
		unsigned int padding_hi; // Padding (0x000000FF)
	} startblock;
	struct enblock {
		unsigned int padding_low; // Padding (0xFAFAFAFA)
		unsigned int padding_hi; // Padding (0xFAFAFAFA)
		unsigned int blocksize; // Size of the block, yet again.
	} endblock;
	
	startblock.magicnumber = (unsigned int){'U', 'S', 'D', 0x8A};
	startblock.blocksize = (unsigned int)(size + sizeof(startblock) + sizeof(endblock));
	startblock.padding_low = 0x00000000;
	startblock.padding_hi = 0x000000FF;
	// Between startblock{} and endblock{} is the data. We give a pointer to the beginning of this.
	endblock.padding_low = 0xFAFAFAFA;
	endblock.padding_hi = 0xFAFAFAFA;
	endblock.blocksize = (unsigned int)(size + sizeof(startblock) + sizeof(endblock));
	// For the actual allocating....
	unsigned char* address = findblock(size, sizeof(startblock), sizeof(endblock));
	
	
}
*/