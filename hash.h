#ifndef __HASH_H__
#define __HASH_H__

#include <windows.h>

unsigned int crc32(unsigned int, const void *, size_t);

/* 0x47495254 means TRIG. Just key for crc32 algorithm. */
#define hash(buf, size) crc32(0x47495254,buf,size)


#endif
