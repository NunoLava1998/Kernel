#ifndef _PRINT_H_
#define _PRINT_H_

volatile unsigned short* vbuf; // Video buffer
unsigned short x; // X position
unsigned short y; // Y position
void print_loc(unsigned char, unsigned char, unsigned char, unsigned char); // x, y, character, color
void printc(unsigned char, unsigned char); // character, color
void prints(const char*, unsigned char); // string, color

#endif