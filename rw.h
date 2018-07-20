
#ifndef _READ_WRITE_H_
#define _READ_WRITE_H_

/* | These are the read and write functions. This header contains functions for reading and writing to memory (not the hard disk or any other devices).
   | Most of the functions here will include an option for an address (void*) so it can know where to write to.
*/

void mwrite8(void*, unsigned char); // where, what
void mwrite16(void*, unsigned short); // where, what
void mwrite32(void*, unsigned int); // where, what
void mwritesz(void*, unsigned int, char); // where, size, what
void mcpy(void*, void*, unsigned int); // from, to, size
void mnull8(void*); // where
void mnull16(void*); // where
void mnull32(void*); // where
void mnullsz(void*, unsigned int); // where, size
unsigned char mget8(void*); // where (returns value at location)
unsigned short mget16(void*); // where (returns value at location)
unsigned int mget32(void*); // where (returns value at location)

#endif