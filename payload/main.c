/**
  * GreenPois0n Cynanide - main.c
  * Copyright (C) 2010 Chronic-Dev Team
  * Copyright (C) 2010 Joshua Hill
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aes.h"
#include "bdev.h"
#include "nvram.h"
#include "image.h"
#include "patch.h"
#include "memory.h"
#include "kernel.h"
#include "common.h"
#include "commands.h"
#include "filesystem.h"

Bool gGpHasInit = FALSE;

int gp_init() {
	if(cmd_init()) return -1;
	if(patch_init()) return -1;
	if(memory_init()) return -1;

#if TARGET_AES_CRYPTO_CMD
	if(aes_init()) return -1;
#endif

#if TARGET_BDEV_LIST
	if(bdev_init()) return -1;
#endif

#if TARGET_IMAGE_LIST && TARGET_BDEV_LIST && TARGET_AES_CRYPTO_CMD
	if(image_init()) return -1;
#endif

#if TARGET_NVRAM_LIST
	if(nvram_init()) return -1;
#endif

#if TARGET_FS_MOUNT && TARGET_FS_UNMOUNT && TARGET_FS_LOAD_FILE
	if(fs_init()) return -1;
#endif

//#if TARGET_KERNEL_LOAD && TARGET_KERNEL_PHYMEM
	if(kernel_init()) return -1;
//#endif

	gGpHasInit = TRUE;
	return 0;
}

int main(int argc, CmdArg* argv) {
	if(!gGpHasInit) {
		puts("Attempting to initialize greenpois0n\n");
		if(gp_init()) {
			puts("Unable to initialize greenpois0n!!\n");
			return -1;
		}
		printf("Greenpois0n initialized\n");
		return 0;
	}

#if TARGET_NVRAM_LIST
	int i = 0;
	for(i = 1; i < argc; i++) {
		if(!strcmp(argv[i].string, "$_")) {
			NvramVar* retval = nvram_find_var("?");
			printf("substituting $_ with %s\n", retval->string);
			argv[i].string = retval->string;
			continue;
		}
		if(argv[i].string[0] == '$') {
			NvramVar* var = nvram_find_var(&(argv[i].string[1]));
			if(var == NULL) {
				printf("Unable to find nvram var for %s\n", &(argv[i].string[1]));
			} else {
				printf("substituting %s with %s\n", argv[i].string, var->string);
				argv[i].string = var->string;
			}
		}
	}
#endif

	if(argc > 1) {
		int i = 0;
		char* command = argv[1].string;
		for(i = 0; i < gCmdCount; i++) {
			if(!strcmp(gCmdCommands[i]->name, command)) {
				return gCmdCommands[i]->handler(argc-1, &argv[1]);
			}
		}

		printf("Command Not Found\n\n");
		return -1;
	}

	return 0;
}

