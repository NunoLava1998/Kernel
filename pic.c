#include "pic.h"

unsigned char inb(unsigned short port) {
	unsigned char rv;
	__asm__ __volatile__ ("inb %1, %0" : "=a"(rv) : "Nd"(port)); return rv;
}
void outb(unsigned short port, unsigned char val) {
	__asm__ __volatile__ ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void hwait(void) {
	__asm__ __volatile__ ("jmp 1f\n\t"
						  "1: jmp 2f\n\t"
						  "2: " );
}

static void houtb(unsigned short port, unsigned char val) {
	outb(port, val);
	hwait();
}

void cmask(unsigned char irq) {
	unsigned short port;
	unsigned char value;
	if (irq < 8) {
		port = 0x21;
	} else {
		port = 0xA1;
		irq -= 8;
	}
	value = inb(port) & ~(1 << irq); outb(port, value);
}

// Serial port functions:
// 1: Helper functions
// 2: Main functions

// **1: Helper functions**
// _s_received: Returns 0 if a serial has not been received, otherwise returns a 1. Helper function of read_serial()
// _s_isempty: Returns 0 if the serial port is empty, and otherwise returns a 1. Helper function of write_serial().

static int _s_received(void) {
	return inb(0x3FD) & 1;
}

static int _s_isempty(void) {
	return inb(0x3FD) & 0x20;
}

// **2: Main functions**
// read_serial: Reads from the serial port and returns a one byte value.
// write_serial: Writes a one byte value (the only argument) to the serial port.

char read_serial(void) {
	while (!_s_received());
	return inb(0x3F8);
}

void write_serial(char c) {
	while (!_s_isempty());
	outb(0x3F8, c);
}


void lpic(void) {
	// Reconfigure and enable PIC.
	unsigned char m1 = inb(0x21);
	unsigned char m2 = inb(0xA1);
	houtb(0x20, 0x11);
	houtb(0xA0, 0x11);
	houtb(0x21, 0x20);
	houtb(0xA1, 0x28);
	houtb(0x21, 4);
	houtb(0xA1, 2);
	houtb(0x21, 0x01);
	houtb(0xA1, 0x01);
	outb(0x21, m1);
	outb(0xA1, m2);
	unsigned short port = 0x21;
	unsigned char vl = inb(0x21) & ~(1 << (unsigned char)0x00);
	outb(port, vl);
	vl = inb(0x21) & ~(1 << (unsigned char)0x01);
	outb(port, vl);
	// Initialize serial ports (COM1, COM2, etc..)
	outb(0x3F9, 0x00);
	outb(0x3FB, 0x80);
	outb(0x3F8, 0x03);
	outb(0x3F9, 0x00);
	outb(0x3FB, 0x03);
	outb(0x3FA, 0xC7);
	outb(0x3FC, 0x0B);
}

