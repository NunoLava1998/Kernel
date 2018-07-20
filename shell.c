#include "shell.h"
#include "com.h"
#include "print.h"

const char returnstring_tcom[(256 - 1)] = {0};
const char returnstring_acom[(256 - 1)] = {0};
unsigned char lastchar = 0;
unsigned char combuf[256] = {0};

static unsigned char _inb_(unsigned short port) {
	unsigned char rv;
	__asm__ __volatile__ ("inb %1, %0" : "=a"(rv) : "Nd"(port)); return rv;
}
static void _outb_(unsigned short port, unsigned char val) {
	__asm__ __volatile__ ("outb %0, %1" : : "a"(val), "Nd"(port));
}

#define ESC 0xE2 // Escape
#define BSP 0xE3 // Backspace
#define TAB 0xE4 // Tab
#define ENT 0xE5 // Enter
#define LCT 0xE6 // Left Ctrl
#define LSH 0xE7 // Left Shift
#define RSH 0xE8 // Right Shift
#define CPS 0xE9 // Caps Lock
#define F1 0xEA // F1
#define F2 0xEB // F2
#define F3 0xEC // F3
#define F4 0xED // F4
#define F5 0xEE // F5
#define F6 0xEF // F6
#define F7 0xF0 // F7
#define F8 0xF1 // F8
#define F9 0xF2 // F9
#define F10 0xF3 // F10
#define F11 0xF4 // F11
#define F12 0xF5 // F12
#define NML 0xF6 // Number Lock
#define SCL 0xF7 // Scroll Lock
#define ALT 0xF8 // Left Alt
#define RESET_OK 0x02 // For reset()
#define RESET_INITR 0x64 // For reset()

static unsigned char allowedtype = 0;

// Below is a scancode for pressed (not released) keys on the default US QWERTY keyboard.

unsigned char scode[127] = {
	0, ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', BSP, TAB, 'q', 'w', 'e', 'r', 't', 'y',
	'u', 'i', 'o', 'p', '[', ']', ENT, LCT, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', LSH,
	'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', RSH, '*', ALT, ' ', CPS, F1, F2, F3, F4, F5, F6,
	F7, F8, F9, F10, NML, SCL, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', 0, 0, 0, F11,
	F12
};

static void halt(void) {
	for (;;) {
		__asm__ __volatile__ ("cli; hlt");
	}
}

void writecb(unsigned char num) {
	if (combuf[(256 - 1)] != 0) {
		// Shift all items back by one
		for (int i = 0; i < (256 - 1); i++) {
			combuf[i] = combuf[i - 1];
		}
		combuf[(256 - 1)] = num; // Assign new item
	} else {
		for (int len = 0; len < (256 - 1); len++) {
			if (combuf[len] == 0) {
				combuf[len] = num;
				break;
			}
		}
	}
}

unsigned char lastcharcb(void) {
	for (int i = (256 - 1); i >= 0; i--) {
		if (combuf[i] != 0) {
			return (unsigned char)combuf[i];
		}
	}
	return 0xFF;
}

static void shreset(void) {
	unsigned char ok = RESET_OK;
	while (ok & RESET_OK) {
		ok = _inb_(RESET_INITR);
	}
	_outb_(0x64, 0xFE);
	halt();
}

static unsigned char kbhn_getchar(void) {
	unsigned char rv = 0;
	do {
		if (_inb_(0x60) != rv) {
			rv = _inb_(0x60);
			if (rv > 0) {
				return scode[rv];
			}
		}
	} while(1);
}

void kbhn(void) {
	unsigned char c = kbhn_getchar();
	if (allowedtype == 1) {
		if (c < 0xE2) {
			const char* s = (const char*)(&c);
			prints(s, 0x0F);
			writecb(c);
		}
	}
	return;
}

static const char* getcom(void) {
	unsigned char isenter = 0;
	__asm__("sti");
	while (isenter == 0) {
		allowedtype = 1;
		if (lastchar == ENT) {
			allowedtype = 0; isenter = 1;
		}
	}
	return (const char*)combuf;
}

static char* strcpy(const char* _s1, const char* _s2) { // Unlike most strcpy(), the arguments are (source, destination), not (destination, source).
	char* s1 = (char*)_s1; char* s2 = (char*)_s2;
	if (s2 == 0) {
		return 0;
	}
	char *ptr = (char*)s2;
	while (*s1 != 0) {
		*s2 = *s1;
		s2++; s1++;
	}
	*s2 = 0;
	return ptr;
}

static const char* getcomname(const char com[(256 - 1)]) {
	unsigned char comnamebuf[(256 - 1)] = {0};
	unsigned char returnstring[(256 - 1)] = {0};
	unsigned char where = 0;
	for (int len = 0; len < (256 - 1); len++) {
		comnamebuf[len] = com[len];
	}
	for (int len = 0; len < (256 - 1); len++) {
		if (comnamebuf[len] == ' ') {
			where = len;
			break;
		}
		if (comnamebuf[len] == 0) {
			where = len;
			break;
		}
	}
	for (int len = 0; len < where; len++) {
		comnamebuf[len] = returnstring[len];
	}
	strcpy((const char*)returnstring, returnstring_tcom);
	return returnstring_tcom;
}

static unsigned int getcom_startargs(const char com[(256 - 1)]) {
	unsigned char comnamebuf[(256 - 1)] = {0};
	unsigned char where = 0;
	for (int len = 0; len < (256 - 1); len++) {
		comnamebuf[len] = com[len];
	}
	for (int len = 0; len < (256 - 1); len++) {
		if (comnamebuf[len] == ' ') {
			where = len;
			break;
		}
		if (comnamebuf[len] == 0) {
			where = len;
			break;
		}
	}
	return where;
}

static const char* getcomargs(const char com[(256 - 1)]) {
	unsigned int start = (getcom_startargs(com) + 1);
	unsigned char comargsbuf[(256 - 1)] = {0};
	(void)comargsbuf;
	const char returnstring[(256 - 1)] = {0};
	for (int len = start; len < (256 - 1); len++) {
		// ToDo: Parse arguments
	}
	strcpy(returnstring, returnstring_acom);
	return returnstring_acom;
}

static void sherror(char* fullcom) {
	prints("Unknown command: \n\"", 0x0F);
	prints(fullcom, 0x0F);
	prints("\".", 0x0F);
}

static int strcmp(const char* s1, const char* s2) {
	for (int i = 0;; i++) {
		if (s1[i] != s2[i]) {
			return s1[i] < s2[i] ? -1 : 1;
			// "return" = Return the following value:
			// "s1[i] < s2[i] ? -1 : 1" = If s1[i] is smaller than s2[i], return -1. If not, return 1.
		}
		if (s1[i] == 0) {
			return 0;
		}
	}
}

static void shfunction(char* com, char* arg, char* full) {
	(void)arg; // We have not implemented arguments yet, so this function simply removes the compiler warning.
	if (strcmp(com, "version") == 0) {
		shversion();
	} else if (strcmp(com, "reset") == 0) {
		shreset();
	} else {
		sherror(full);
	}
}

static void shloop(void) {
	char* command;
	char* progname;
	char* args;
	while (1) {
		__asm__("sti");
		prints("\n> ", 0x0F);
		command = (char*)getcom();
		prints("\n", 0);
		progname = (char*)getcomname(command);
		args = (char*)getcomargs(command);
		shfunction(progname, args, command);
	}
}

void shell(void) {
	shloop();
	for(;;) {
		shreset();
	}
}