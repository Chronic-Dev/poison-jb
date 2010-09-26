#ifndef UTILS_H
#define UTILS_H

#include "syscalls.h"

extern int stdout;
extern int stderr;

#define puts _puts

int strlen(const char* s);
void _puts(const char* msg);
int cp(const char* src, const char* dest);
int hfs_mount(char* device, char* mountdir, int options);

#endif
