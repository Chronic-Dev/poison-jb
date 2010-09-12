/*
 *
 *  greenpois0n - include/lzss.h
 *  (c) 2009 Chronic-Dev Team
 *
 */

#ifndef LZSS_H
#define LZSS_H

unsigned char* lzss_compress(unsigned char* dst, unsigned int dstlen, unsigned char* src, unsigned int srclen);
int lzss_decompress(unsigned char* dst, unsigned int dstlen, unsigned char* src, unsigned int srclen);
unsigned int adler32(unsigned char* src, int length);

#endif /* LZSS_H */

