/*
 *
 *  greenpois0n - payload/src/filesystem.c
 *  (c) 2010 Chronic-Dev Team
 *
 */

#include <stdio.h>

#include "filesystem.h"

int(*fs_load_file)(const char *path, void* address, unsigned int* size) = SELF_FS_LOAD_FILE;
void(*fs_mount)(const char *partition, const char *type, const char *path) = SELF_FS_MOUNT;
void(*fs_unmount)(const char *path) = SELF_FS_UNMOUNT;

int fs_init() {
	cmd_add("fs", &fs_cmd, "perform operations on the filesystem");
	return 0;
}

int fs_cmd(int argc, CmdArg* argv) {
	char* path = NULL;
	char* action = NULL;
	char* device = NULL;
	void* address = NULL;
	unsigned int* size = 0;

	if(argc < 3) {
		puts("usage: fs <mount/unmount/load> [options]\n");
		puts("  mount <device> <path> \tmount device to path\n");
		puts("  unmount <path>        \tunmount specified path\n");
		puts("  load <path> <address> \tload file from path to address\n\n");
		return 0;
	}

	action = argv[1].string;
	if(argc == 3) {
		if(strcmp(action, "umount")) {
				path = argv[2].string;
				fs_unmount(path);
		}

	} else if(argc == 4) {
		if(!strcmp(action, "mount")) {
			path = argv[3].string;
			device = argv[2].string;
			fs_mount(device, "hfs", path);

		} else if(!strcmp(action, "load")) {
			path = argv[2].string;
			address = (void*) argv[3].uinteger;
			fs_load_file(path, address, size);
		}
	}

	return 0;
}
