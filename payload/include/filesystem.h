/*
 *
 *  greenpois0n - payload/include/filesystem.h
 *  (c) 2010 Chronic-Dev Team
 *
 */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "common.h"
#include "device.h"
#include "offsets.h"
#include "commands.h"

#ifdef TARGET_FS_MOUNT
#	define SELF_FS_MOUNT ((void*)(TARGET_BASEADDR + TARGET_FS_MOUNT))
#endif
#ifdef TARGET_FS_UNMOUNT
#	define SELF_FS_UNMOUNT ((void*)(TARGET_BASEADDR + TARGET_FS_UNMOUNT))
#endif
#ifdef TARGET_FS_LOAD_FILE
#	define SELF_FS_LOAD_FILE ((void*)(TARGET_BASEADDR + TARGET_FS_LOAD_FILE))
#endif

#ifndef SELF_FS_MOUNT
#	define SELF_FS_MOUNT 0
//#	error "SELF_FS_MOUNT not defined"
#endif
#ifndef SELF_FS_UNMOUNT
#	define SELF_FS_UNMOUNT 0
//#	error "SELF_FS_UNMOUNT not defined"
#endif
#ifndef SELF_FS_LOAD_FILE
#	define SELF_FS_LOAD_FILE 0
//#	error "SELF_FS_LOAD_FILE not defined"
#endif

extern void(*fs_unmount)(const char *path);
extern int(*fs_load_file)(const char *path, void* address, unsigned int* size);
extern void(*fs_mount)(const char *partition, const char *type, const char *path);

int fs_init();
int fs_cmd(int argc, CmdArg* argv);

#endif /* FILESYSTEM_H */
