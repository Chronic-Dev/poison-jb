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

int kernel_init();
int kernel_cmd(int argc, CmdArg* argv);

int kernel_load(void* image, void* dest);
int kernel_patch(void* address);

#endif /* KERNEL_H */
