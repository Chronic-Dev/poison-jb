#ifndef UTILS_H
#define UTILS_H

#include "syscalls.h"

#define puts _puts
#define strlen _strlen

void _puts(const char* text);
int _strlen(const char *s);

#endif
