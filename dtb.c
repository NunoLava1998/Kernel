#include "dtb.h"
#include "shell.h"

#define MAX32 0xFFFFFFFF
#define GRANL 0xCF

static void __mset__(void* a, unsigned int sz, char c) {
	unsigned char* b = (unsigned char*)a;
	for (unsigned int i = 0; i < sz; i++) {
		unsigned char* _c = (unsigned char*)(b + i);
		*_c = c;
	}
}

struct gdt_table {
	unsigned short limit_low;
	unsigned short base_low;
	unsigned char base_med;
	unsigned char access;
	unsigned char granularity;
	unsigned char base_hi;
} __attribute__ ((packed));

struct idt_table {
	unsigned short base_low;
	unsigned short selector;
	unsigned char unused;
	unsigned char flags;
	unsigned short base_high;
} __attribute__ ((packed));

struct gdt_pointer {
	unsigned short limit;
	unsigned int base;
} __attribute__ ((packed));

struct idt_pointer {
	unsigned short limit;
	unsigned int base;
} __attribute__ ((packed));

struct gdt_table gdt_entry[5];
struct gdt_pointer gdtp;
struct idt_table idt_entry[256];
struct idt_pointer idtp;

static void gdt_mkentry(unsigned short num, unsigned int base, unsigned int limit, unsigned char access, unsigned char granularity) {
	gdt_entry[num].limit_low = limit;
	gdt_entry[num].base_low = base;
	gdt_entry[num].base_med = (base >> 16);
	gdt_entry[num].base_hi = (base >> 24);
	gdt_entry[num].granularity = ((limit >> 16) & 0x0F) | (granularity & 0xF0);
	gdt_entry[num].access = access;
}

static void idt_mkentry(unsigned short num, unsigned int base, unsigned selector, unsigned char flags) {
	idt_entry[num].base_low = base;
	idt_entry[num].base_high = (base >> 16);
	idt_entry[num].selector = selector;
	idt_entry[num].unused = 0;
	idt_entry[num].flags = flags;
}

extern void lgdt(void);
extern void isr(void);
extern void irq(void);
extern void lidt(void);

void cli(void) {
	__asm__ __volatile__ ("cli");
}

void sti(void) {
	__asm__ __volatile__ ("sti");
}

void ldtb(void) {
	gdtp.limit = (unsigned short)(sizeof(gdt_entry) * 5) - 1;
	gdtp.base = (unsigned int)&gdt_entry;
	idtp.limit = (unsigned short)(sizeof(idt_entry) * 256) - 1;
	idtp.base = (unsigned int)&idt_entry;
	__mset__(&idt_entry, 256, 0);
	
	gdt_mkentry(0, 0, 0, 0, 0);
	gdt_mkentry(1, 0, MAX32, 0x9A, GRANL);
	gdt_mkentry(2, 0, MAX32, 0x92, GRANL);
	gdt_mkentry(3, 0, MAX32, 0xFA, GRANL);
	gdt_mkentry(4, 0, MAX32, 0xF2, GRANL);
	
	for (unsigned char i = 0; i < 32; i++) {
		idt_mkentry(i, (unsigned)isr, 0x08, 0x8E);
	}
	for (unsigned char i = 32; i < 48; i++) {
		idt_mkentry(i, (unsigned)irq, 0x08, 0x8E);
	}
	
	lgdt();
	lidt();
}