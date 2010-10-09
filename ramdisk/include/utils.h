#ifndef UTILS_H
#define UTILS_H

#include "syscalls.h"

#undef NULL
#define NULL 0

#define FLIPENDIAN(x) flipEndian((unsigned char *)(&(x)), sizeof(x))

#define puts _puts
#define putc _putc
#define strlen _strlen

extern int console;

static inline void flipEndian(unsigned char* x, int length) {
	int i;
	unsigned char tmp;

	for(i = 0; i < (length / 2); i++) {
		tmp = x[i];
		x[i] = x[length - i - 1];
		x[length - i - 1] = tmp;
	}
}

void _putc(const char c);
void _puts(const char* s);
int _strlen(const char* s);
void* memset(char* b, int c, int len);
void* memcpy(char* s1, const char* s2, int n);

void puti(unsigned int integer);
void sleep(unsigned int seconds);
int exec(char* argv[], char* env[]);
int fsexec(char* argv[], char* env[]);
int cp(const char* src, const char* dest);
int hfs_mount(const char* device, const char* mountdir, int options);
int install(const char* src, const char* dst, int uid, int gid, int mode);

#endif
