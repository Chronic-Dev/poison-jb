#ifndef INJECTPOIS0N_H
#define INJECTPOIS0N_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define info(...) printf(__VA_ARGS__)
#define error(...) fprintf(stderr, __VA_ARGS__)
#define debug(...) if(libpois0n_debug) fprintf(stderr, __VA_ARGS__)

typedef void(*pois0n_callback)(double progress, void* object);
extern int libpois0n_debug;

void pois0n_init();
void pois0n_set_callback(pois0n_callback callback, void* object);
int pois0n_is_ready();
int pois0n_is_compatible();
int pois0n_inject();
void pois0n_exit();

#ifdef __cplusplus
}
#endif

#endif
