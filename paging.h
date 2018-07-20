#ifndef _PAGING_H_
#define _PAGING_H_

void paging_enable(void);
void paging_initialize(void);
char* paging_newpage(int);
int paging_freepage(char*, int);

#endif