#include "var.h"

void setbit(char c, unsigned char b) {
	c |= 0x01 << b;
}

void nulbit(char c, unsigned char b) {
	c &= ~(0x01 << b);
}

void togbit(char c, unsigned char b) {
	c ^= 0x01 << b;
}

char getbit(char c, unsigned char b) {
	return (char)(c >> b) & 0x01;
}