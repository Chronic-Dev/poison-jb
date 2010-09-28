#ifndef INJECTPOIS0N_H
#define INJECTPOIS0N_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#define info(...) printf(__VA_ARGS__)
#define error(...) fprintf(stderr, __VA_ARGS__)
#define debug(...) if(injectpois0n_debug) fprintf(stderr, __VA_ARGS__)

extern int injectpois0n_debug;

void print_progress_bar(double progress);
int read_file(const char* filename, void** data, int* size);
int write_file(const char* filename, const void* data, int size);

#endif
