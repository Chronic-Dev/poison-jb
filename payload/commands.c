/**
  * GreenPois0n Cynanide - commands.c
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

#include "lock.h"
#include "patch.h"
#include "common.h"
#include "commands.h"
#include "functions.h"
#include "coprocessor.h"

int gCmdCount = 0;
Bool gCmdHasInit = FALSE;

CmdInfo** gCmdCommands = NULL;

void* gCmdListEnd = SELF_CMD_LIST_END;
void* gCmdListBegin = SELF_CMD_LIST_BEGIN;
int(*fsboot)(void) = NULL;
int(*jump_to)(int flags, void* addr, int phymem) = SELF_JUMP_TO;
int(*load_ramdisk)(int argc) = SELF_CMD_RAMDISK;

void hooked(int flags, void* addr, int phymem);

/*
 * Private Functions
 */
int cmd_init() {
	if(gCmdHasInit) return 0;
	printf("Initializing commands\n");

	int i = 0;
	gCmdCount = 0;
	gCmdHasInit = TRUE;
	gCmdCommands = (CmdInfo**) (LOADADDR + 0x02000000);

	// add all built in commands to our private commands
	CmdInfo** current = (CmdInfo**) gCmdListBegin;
	for (i = 0; &current[i] < (CmdInfo**) gCmdListEnd; i++) {
		cmd_add(current[i]->name, current[i]->handler, current[i]->description);
	}

	// add our essential commands
	cmd_add("help", &cmd_help, "display all available commands");
	cmd_add("echo", &cmd_echo, "write characters back to screen");
	cmd_add("hexdump", &cmd_hexdump, "dump section of memory to screen");
	cmd_add("jump", &cmd_jump, "shutdown current image and jump into another");
	cmd_add("mw", &cmd_mw, "write value to specified address");
	cmd_add("md", &cmd_md, "display value at specified address");
	cmd_add("call", &cmd_call, "calls a subroutine passing args to it");
	cmd_add("fsboot", &cmd_fsboot, "patch and boot kernel from filesystem");
	cmd_add("test", &cmd_test, "test finding functions offsets");

#ifndef TARGET_CMD_RAMDISK
	load_ramdisk = find_function("cmd_ramdisk", TARGET_BASEADDR, TARGET_BASEADDR);
#endif

	if(load_ramdisk) {
		cmd_add("ramdisk", &cmd_ramdisk, "create a ramdisk from the specified address");
	}

	return 0;
}

void cmd_add(char* name, CmdFunction handler, char* description) {
	CmdInfo* command = NULL;
	if(gCmdCount >= MAX_COMMANDS) {
		puts("Maximum Commands Reached\n");
		return;
	}

	//command = (CmdInfo*) malloc(sizeof(CmdInfo));
	command = (CmdInfo*) (LOADADDR + 0x02000000) + (gCmdCount * sizeof(CmdInfo));
	command->name = name;
	command->handler = handler;
	command->description = description;
	gCmdCommands[gCmdCount] = command;
	gCmdCount++;
}

int cmd_get_arg(CmdArg arg) {
	if (arg.type==CMDARG_TYPE_INTEGER) {
		return arg.uinteger;
	} else {
		char* ret = arg.string;
		int len = strlen(ret);
		// strip leading and trailing quotes if any
		if (len > 0 && (ret[len-1]=='"' || ret[len-1]=='\'')) ret[len-1]=0;
		if (ret[0]=='"' || ret[0]=='\'') ret = &ret[1];
		return (int) ret;
	}
}

void cmd_start() {
	printf_begin();
}

/*
 * Public Functions
 */
int cmd_help(int argc, CmdArg* argv) {
	int i = 0;
	enter_critical_section();
	printf("Commands:\n");
	for(i = 0; i < gCmdCount; i++) {
		printf("  %s \t", gCmdCommands[i]->name);
		if(strlen(gCmdCommands[i]->name) < 5) printf(" \t");
		printf("%s\n", gCmdCommands[i]->description);
	}
	printf("\n");
	exit_critical_section();

	return 0;
}

int cmd_echo(int argc, CmdArg* argv) {
	int i = 0;
	if(argc >= 2) {
		enter_critical_section();
		for(i = 1; i < argc; i++) {
			printf("%s ", argv[i].string);
		}
		printf("\n");
		exit_critical_section();
		return 0;
	}
	
	puts("usage: echo <message>\n");
	return 0;
}

int cmd_hexdump(int argc, CmdArg* argv) {
	int i = 0;
	unsigned int len = 0;
	unsigned char* buf = NULL;
	if(argc != 3) {
		puts("usage: hexdump <address> <length>\n");
		return 0;
	}

	len = argv[2].uinteger;
	buf = (unsigned char*) argv[1].uinteger;
	hexdump(buf, len);

	return 0;
}

int cmd_mw(int argc, CmdArg* argv) {
	cmd_start();

	if(argc != 3) {
		puts("usage: mw <address> <value>\n");
		return 0;
	}

	unsigned int address = argv[1].uinteger;
	unsigned int value = argv[2].uinteger;
	SETREG32(address, value);
	//clear_cpu_caches();
	return 0;
}

int cmd_md(int argc, CmdArg* argv) {
	cmd_start();

	if(argc != 2) {
		puts("usage: md <address>\n");
		return 0;
	}

	unsigned int value = GETREG32(argv[1].uinteger);
	enter_critical_section();
	printf("%p\n", value);
	exit_critical_section();

	return 0;
}

int cmd_jump(int argc, CmdArg* argv) {
	int i = 0;
	void* address = NULL;
	if(argc < 2) {
		puts("usage: jump <address>\n");
		return 0;
	}
	if(argc == 2) {
		address = (void*) argv[1].uinteger;
		jump_to(0, address, 0);
	}
	if(argc == 4) {
		address = (void*) argv[1].uinteger;
		jump_to(argv[2].uinteger, argv[2].uinteger, argv[2].uinteger);
	}

	return 0;
}

int cmd_call(int argc, CmdArg* argv) {
	cmd_start();

	if (argc < 2) {
		puts("usage: call <address> <args>\n");
		puts("  <address> is subroutine address (pair=ARM, odd=THUMB)\n");
		puts("  <args> are subroutine arguments (integers), if any.\n");
		puts("  [!] limited to 4 args max subroutines at the moment.\n");
		return 0;
	} else if (argc > 6) {
		puts("[!] Sorry, call is limited to 4 args max subroutines ATM.\n");
		return -1;
	} else if (argv[1].type!=CMDARG_TYPE_INTEGER) {
		puts("[!] <address> needs to be an integer !\n");
		return -1;
	}

	int addr = argv[1].uinteger;
	int r0, r1, r2, r3;

	if (argc >= 3) r0 = cmd_get_arg(argv[2]);
	if (argc >= 4) r1 = cmd_get_arg(argv[3]);
	if (argc >= 5) r2 = cmd_get_arg(argv[4]);
	if (argc >= 6) r3 = cmd_get_arg(argv[5]);

	int (*pSub)(int, int, int, int) = (int (*)(int, int, int, int)) addr;
	int ret = pSub(r0, r1, r2, r3);

	printf("returned: %08x\n", ret);

	return 0;
}

int cmd_fsboot(int argc, CmdArg* argv) {
	int i = 0;
	void* address = NULL;
	void(*hooker)(int flags, void* addr, void* phymem) = &hooked;
	if(argc != 1) {
		puts("usage: fsboot\n");
		return 0;
	}

	// search for jump_to function
	if(strstr((char*) (IBOOT_BASEADDR + 0x200), "n72ap")) {
		jump_to = patch_find(IBOOT_BASEADDR, 0x30000, "\xf0\xb5\x03\xaf\x04\x1c\x15\x1c", 8);
	} else {
		// 80  B5  00  AF  04  46  15  46
		jump_to = patch_find(IBOOT_BASEADDR, 0x30000, "\x80\xb5\x00\xaf\x04\x46\x15\x46", 8);
	}
	printf("Found jump_to function at %p\n", jump_to);

	memcpy(jump_to, "\x00\x4b\x98\x47", 4);
	memcpy(jump_to+4, &hooker, 4);

	printf("Hooked jump_to function to call 0x%08x\n", hooker);
	if(strstr((char*) (IBOOT_BASEADDR + 0x200), "n72ap")) {
		fsboot = patch_find(IBOOT_BASEADDR, 0x30000, "\xf0\xb5\x03\xaf\x11\x48", 6);
	} else
	if(strstr((char*) (IBOOT_BASEADDR + 0x200), "k66ap")) {
		fsboot = patch_find(IBOOT_BASEADDR, 0x30000, "\xf0\xb5\x03\xaf\x81\xb0", 6);
	} else {
		fsboot = patch_find(IBOOT_BASEADDR, 0x30000, "\xb0\xb5\x02\xaf\x11\x48", 6);
	}
	printf("Found fsboot function at %p\n", fsboot);

	//call address
	fsboot++;
	printf("Calling %p\n", fsboot);
	fsboot();

	return 0;
}

int cmd_test(int argc, CmdArg* argv) {
	printf("aes_crypto_cmd: 0x%08x\n", find_function("aes_crypto_cmd", TARGET_BASEADDR, TARGET_BASEADDR));
	printf("free: 0x%08x\n", find_function("free", TARGET_BASEADDR, TARGET_BASEADDR));
	printf("cmd_ramdisk: 0x%08x\n", find_function("cmd_ramdisk", TARGET_BASEADDR, TARGET_BASEADDR));
	printf("fs_mount: 0x%08x\n", find_function("fs_mount", TARGET_BASEADDR, TARGET_BASEADDR));
	return 0;
}

int cmd_ramdisk(int argc, CmdArg* argv) {
	printf("%d\n", load_ramdisk(3));
	return 0;
}

void clear_icache() {
    __asm__("mov r0, #0");
    __asm__("mcr p15, 0, r0, c7, c5, 0");
    __asm__("mcr p15, 0, r0, c7, c5, 4");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
};

void hooked(int flags, void* addr, int phymem) {
	// patch kernel
	//printf("Entered hooked jump_to function!!!\n");
	//printf("Patching kernel\n");
	patch_kernel((void*)(LOADADDR - 0x1000000), 0xA00000);

	printf("Replace hooking code with original\n");
	if(strstr((char*) (IBOOT_BASEADDR + 0x200), "n72ap")) {
		memcpy(jump_to, "\xf0\xb5\x03\xaf\x04\x1c\x15\x1c", 8);
	} else {
		memcpy(jump_to, "\x80\xb5\x00\xaf\x04\x46\x15\x46", 8);
	}
	clear_icache();

	jump_to++;
	printf("Calling %p\n", jump_to);
	//jump_to(flags, addr, phymem);
}
