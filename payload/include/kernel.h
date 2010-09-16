/*
 *
 *  greenpois0n - payload/include/kernel.h
 *  (c) 2009 Chronic-Dev Team
 *
 */

#ifndef KERNEL_H
#define KERNEL_H

#include "common.h"
#include "device.h"
#include "offsets.h"
#include "commands.h"

#ifdef TARGET_KERNEL_LOAD
#	define SELF_KERNEL_LOAD ((void*)(TARGET_BASEADDR + TARGET_KERNEL_LOAD))
#endif
#ifdef TARGET_KERNEL_PHYMEM
#	define SELF_KERNEL_PHYMEM ((void*)(TARGET_BASEADDR + TARGET_KERNEL_PHYMEM))
#endif

#ifndef SELF_KERNEL_LOAD
#	define SELF_KERNEL_LOAD 0
#	warning "SELF_KERNEL_LOAD not defined"
#endif

#ifndef SELF_KERNEL_PHYMEM
#	define SELF_KERNEL_PHYMEM 0
#	warning "SELF_KERNEL_PHYMEM not defined"
#endif

extern char* gBootArgs;
extern char** gKernelPhyMem;
extern int(*kernel_load)(void* input, int max_size, char** output);

int kernel_init();
int kernel_cmd(int argc, CmdArg* argv);
int kernel_patch(void* address);

#endif /* KERNEL_H */
