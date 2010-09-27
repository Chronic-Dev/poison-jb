#ifndef UTILS_H
#define UTILS_H

#include "syscalls.h"

extern int stdout;
extern int stderr;

#define puts _puts

int strlen(const char* s);
void _puts(const char* msg);
void sleep(unsigned int seconds);
int cp(const char* src, const char* dest);
int hfs_mount(const char* device, const char* mountdir, int options);

#endif
