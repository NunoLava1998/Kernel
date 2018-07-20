#include "../paging.h"

int liballoc_lock(void) {
	__asm__ __volatile__ ("cli");
	return 0;
}
int liballoc_unlock(void) {
	__asm__ __volatile__ ("sti");
	return 0;
}
void* liballoc_alloc(int amount) {
	return (void*)paging_newpage(amount);
}
int liballoc_free(void* where, int amount) {
	return paging_freepage(where, amount);
}