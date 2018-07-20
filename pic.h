#ifndef _PIC_H_
#define _PIC_H_

unsigned char inb(unsigned short);
void outb(unsigned short, unsigned char);
void hwait(void);
void cmask(unsigned char);
char read_serial(void);
void write_serial(char);
void lpic(void);

#endif