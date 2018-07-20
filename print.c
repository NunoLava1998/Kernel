#include "print.h"

static unsigned int __strlen__(const char* s) {
	unsigned int i = 0;
	while (s[i]) {
		i++;
	}
	return i;
}


volatile unsigned short* vbuf = (volatile unsigned short*)0xC00B8000;
unsigned short x = 0;
unsigned short y = 0;

void print_loc(unsigned char _x, unsigned char _y, unsigned char c, unsigned char cl) {
	vbuf[(_y * 80 + _x)] = ((unsigned short) c | (unsigned short)cl << 8);
}

static void newline(void) {
	x = 0; y++;
}

static void tab(void) {
	// Tab stops are at x positions:
	// 0, 7, 15, 23, 31, 39, 47, 55, 63, 71 and 79.
	// If you start from 1:
	// 1, 8, 16, 24, 32, 40, 48, 56, 64, 72 and 80.
	for (unsigned char i = 0; i < 80; i += 7) {
		if (x < i) {
			x = i;
			break;
		}
	}
}

static void backspace(void) {
	unsigned short _x = (x - 1);
	unsigned short _y = (y - 1);
	if (x == 0) {
		if (y == 0 && x == 0) {
			return;
		}
		print_loc(_x, _y, 0, 0);
		y--; x = 0;
	} else {
		x--;
		print_loc(_x, _y, 0, 0);
	}
}

void printc(unsigned char c, unsigned char cl) {
	if (c == '\n') {
		newline(); return;
	}
	if (c == '\b') {
		backspace(); return;
	}
	if (c == '\t') {
		tab(); return;
	}
	if (c != '\b' || c != '\n' || c != '\t') {
		print_loc(x, y, c, cl);
		if (++x == 80) {
			x = 0;
			if (++y == 25) {
				y = 0;
			}
		}
	}
}

void prints(const char* s, unsigned char cl) {
	for (unsigned int i = 0; i < __strlen__(s); i++) {
		printc(s[i], cl);
	}
}