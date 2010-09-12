/*
 *
 *  greenpois0n - payload/include/nvram.h
 *  (c) 2010 Chronic-Dev Team
 *
 */

#ifndef NVRAM_H
#define NVRAM_H

#include "common.h"
#include "device.h"
#include "offsets.h"
#include "commands.h"

#ifdef TARGET_NVRAM_LIST
#	define SELF_NVRAM_LIST ((void*)(TARGET_BASEADDR + TARGET_NVRAM_LIST))
#endif

#ifndef SELF_NVRAM_LIST
#	define SELF_NVRAM_LIST 0
#	error "SELF_NVRAM_LIST not defined"
#endif

typedef struct NvramVar {
	struct NvramVar* prev;
	struct NvramVar* next;
	unsigned char* string;
	unsigned int integer;
	unsigned int save;
	char name[0x40];
} NvramVar;

extern LinkedList* gNvramList;

int nvram_init();
int nvram_cmd(int argc, CmdArg* argv);
void nvram_display_list();
int nvram_get_var(const char* name);
int nvram_remove_var(const char* name);
NvramVar* nvram_find_var(const char* name);
int nvram_set_var(const char* name, const char* value);


#endif /* NVRAM_H */
