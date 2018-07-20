#include "paging.h"

static void* paging_memset(void* s, int c, int n) {
	for (int i = 0; i < n; i++)
		((char*)s)[i] = c;
	
	return s;
}

#define HIGHERHALF 0x0C000000

static unsigned int* pagedir = 0;
static unsigned int pagedir_loc = 0;
static unsigned int* lastpage = 0;

unsigned int pagetable[4096] __attribute__((aligned(4096)));
unsigned char pagetable_available[4096];

void paging_virtual_to_physical(unsigned int virtual, unsigned int physical) {
	unsigned short id = (virtual >> 22);
	for (unsigned int i = 0; i < 4096; i++) {
		lastpage[i] = physical | 3;
		physical += 4096;
	}
	pagedir[id] = ((unsigned int)lastpage) | 3;
	lastpage = (unsigned int*)(((unsigned int)lastpage) + 4096);
}

void paging_enable(void) {
	__asm__ __volatile__ ("mov %%eax, %%cr3" : : "a"(pagedir_loc));
	__asm__ __volatile__ ("mov %cr0, %eax");
	__asm__ __volatile__ ("orl $0x80000000, %eax");
	__asm__ __volatile__ ("mov %eax, %cr0");
}

void paging_initialize(void) {
	pagedir = (unsigned int*)HIGHERHALF + (int)&pagetable;
	pagedir_loc = (unsigned int)&pagedir;
	lastpage = (unsigned int*)HIGHERHALF + (int)&pagetable + (4096 * 4096);
	for (unsigned int i = 0; i < 4096; i++) {
		int a = (i * 0x1000);
		int b = a | 3;
		pagedir[i] = b;
	}
	paging_virtual_to_physical(0, 0);
	paging_virtual_to_physical((unsigned int)&pagetable, 0x0D000000);
	for (unsigned int i = 0; i < 4096; i++) {
		pagetable_available[i] = 0;
	}
	paging_enable();
}

static int _paging_createpage(int page) {
	if (page > 4096) {
		return 1;
	}
	pagetable_available[page] = 1;
	return 0;
}

static int _paging_freepage(int page) {
	if (page > 4096) {
		return 1;
	}
	paging_memset((void*)(HIGHERHALF + (int)&(pagetable[page])), 0, 4096); 
	pagetable_available[page] = 0;
	return 0;
}

char* paging_newpage(int page_amount) {
	char* returnaddress = 0x00;
	if (page_amount > 4096) {
		return (char*)0x00;
	}
	if (page_amount == 0) {
		return (char*)0x00;
	}
	// Look for pages.
	for (int i = 0; i < 4096; i++) {
		int gotit = 0;
		int failedgettingpage = 0;
		if (pagetable_available[i] == 0) { // We've reached a free page.
			int b = 0;
			for (int a = 0; a < 4096; a++) {
				// How many free pages? (b is amount of free pages)
				if (pagetable[i + a] == 0) {
					b++;
				}
				if ((pagetable[i + a] == 1) || (b < (int)sizeof(page_amount))) {
					// These free blocks are too large!
					failedgettingpage = 1;
					break;
				}
				if (b >= (int)sizeof(page_amount)) {
					// The amount of free pages is bigger than or equal to the size we need. Good!
					for (int c = 0; c < page_amount; c++) {
						// Allocate said pages
						returnaddress = (char*)&pagetable[i];
						_paging_createpage(i + c);
						gotit = 1;
					}
				}
				if (failedgettingpage == 1) {
					break;
				}
				if (gotit == 1) {
					break;
				}
			}
		}
		if (gotit == 1) {
			break;
		}
	}
	return returnaddress; // Done!
}

int paging_freepage(char* pagestart, int page_amount) {
	int retval = 0;
	if (page_amount > 4096) {
		return 0;
	}
	if (page_amount == 0) {
		return 0;
	}
	for (int i = (int)&pagestart; i < (int)(&pagestart + page_amount); i++) {
		retval = _paging_freepage(i);
	}
	return retval;
}