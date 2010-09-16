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
#include "common.h"
#include "commands.h"

int gCmdCount = 0;
Bool gCmdHasInit = FALSE;

CmdInfo** gCmdCommands = NULL;

void* gCmdListEnd = SELF_CMD_LIST_END;
void* gCmdListBegin = SELF_CMD_LIST_BEGIN;
int(*jump_to)(int flags, void* addr, int unk) = SELF_JUMP_TO;
int(*cmd_ramdisk)(int argc, CmdArg* argv) = SELF_CMD_RAMDISK;

/*
 * Private Functions
 */
int cmd_init() {
	if(gCmdHasInit) return 0;

	int i = 0;
	gCmdCount = 0;
	gCmdHasInit = TRUE;
	gCmdCommands = (CmdInfo**) malloc(sizeof(CmdInfo*) * MAX_COMMANDS);

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

#ifdef TARGET_CMD_RAMDISK
	cmd_add("ramdisk", &cmd_ramdisk, "create a ramdisk from the specified address");
#endif

	return 0;
}

void cmd_add(char* name, CmdFunction handler, char* description) {
	CmdInfo* command = NULL;
	if(gCmdCount >= MAX_COMMANDS) {
		puts("Maximum Commands Reached\n");
		return;
	}

	command = (CmdInfo*) malloc(sizeof(CmdInfo));
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
	//cmd_start();

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
	//cmd_start();
	puts("Entered cmd_echo\n");
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
