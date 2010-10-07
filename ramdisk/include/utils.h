#ifndef UTILS_H
#define UTILS_H

#include "syscalls.h"

extern int stdout;
extern int stderr;

#define puts _puts
#define putc _putc
#define memset _memset

static inline void flipEndian(unsigned char* x, int length) {
	int i;
	unsigned char tmp;

	for(i = 0; i < (length / 2); i++) {
		tmp = x[i];
		x[i] = x[length - i - 1];
		x[length - i - 1] = tmp;
	}
}

int strlen(const char* s);
void puti(unsigned int integer);
void putc(const char c);
void puts(const char* msg);
void sleep(unsigned int seconds);
int cp(const char* src, const char* dest);
int hfs_mount(const char* device, const char* mountdir, int options);
int install(const char* src, const char* dst, int uid, int gid, int mode);
int force_unmount();
int force_mount();

#endif
