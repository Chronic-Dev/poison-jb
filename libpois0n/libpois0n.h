#ifndef LIBPOIS0N_H
#define LIBPOIS0N_H

#ifdef __cplusplus
extern "C" {
#endif

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

int initInjector(); //Returns 0 on success.
int isInDFU(); //Returns 0 on success, -1 if failed to connect, -2 if not in DFU
int isCompatibleDevice(); //Returns 0 on success, -1 if failed to discover type, -2 if incompatible device
int SHAtter(); //Returns 0 on success.

#ifdef __cplusplus
}
#endif

#endif
