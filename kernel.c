#include "klib.h"

unsigned char leftsym[4] = {176, 177, 178, 219};
unsigned char rightsym[4] = {219, 178, 177, 176};
const char* ldmsg = " Kernel successfully loaded. ";
const char ldmsgnul[(sizeof(leftsym))] = {0, 0, 0, 0};

unsigned int getarraylen(unsigned char array[]) {
	return (unsigned int)(sizeof(*array) / sizeof(array[0]));
}

void grld(unsigned char bmap[], unsigned short color, unsigned short where) {
	for (unsigned int i = 0; i < getarraylen(bmap); i++) {
		unsigned short* tl = (unsigned short*)0xC00B8000 + where + (i * 2);
		unsigned short* cl = (unsigned short*)0xC00B8000 + where + (i * 2) + 1;
		*tl = bmap[i]; *cl = color;
	}
}

void osld(void) {
	__asm__ __volatile__ ("cli"); // Disable interrupts, if not already disabled
	mwritesz((void*)0xC00B8000, ((sizeof(unsigned short)) * (80 * 25)), 0); // Clear video memory
	ldtb(); // Load the GDT (Global Descriptor Table) and IDT (Interrupt Descriptor Table). These are very basic things that allow for a lot of programs to run.
	__asm__ __volatile__ ("sti"); // Enable interrupts.
	lpic(); // Load the PIC, along with the serial ports. The jump crashes the system with a double fault???
}

void kmain(void) {
	osld(); // Load some of the basic features of the OS.
	paging_initialize(); // Initialize and enable paging.
	prints((const char*)ldmsgnul, 0x00);
	grld(leftsym, 0x09, 0);
	prints(ldmsg, 0x0F);
	grld(rightsym, 0x09, ((sizeof(leftsym)) + (sizeof(ldmsg))));
}