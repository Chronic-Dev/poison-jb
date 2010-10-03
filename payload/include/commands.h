/**
  * GreenPois0n Cynanide - commands.h
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

#ifndef COMMANDS_H
#define COMMANDS_H

#include "device.h"
#include "offsets.h"

#ifdef TARGET_CMD_LIST_BEGIN
#	define SELF_CMD_LIST_BEGIN ((void*)(TARGET_BASEADDR + TARGET_CMD_LIST_BEGIN))
#endif
#ifdef TARGET_CMD_LIST_END
#	define SELF_CMD_LIST_END ((void*)(TARGET_BASEADDR + TARGET_CMD_LIST_END))
#endif
#ifdef TARGET_JUMP_TO
#	define SELF_JUMP_TO ((void*)(TARGET_BASEADDR + TARGET_JUMP_TO))
#endif
#ifdef TARGET_CMD_RAMDISK
#	define SELF_CMD_RAMDISK ((void*)(TARGET_BASEADDR + TARGET_CMD_RAMDISK))
#endif

#ifndef SELF_CMD_LIST_BEGIN
#	define SELF_CMD_LIST_BEGIN 0
#	error "SELF_CMD_LIST_BEGIN not defined"
#endif

#ifndef SELF_CMD_LIST_END
#	define SELF_CMD_LIST_END 0
#	error "SELF_CMD_LIST_END not defined"
#endif

#ifndef SELF_JUMP_TO
#	define SELF_JUMP_TO 0
#	error "SELF_JUMP_TO not defined"
#endif

#ifndef SELF_CMD_RAMDISK
#	define SELF_CMD_RAMDISK 0
#	warning "SELF_CMD_RAMDISK not defined"
#endif

#define MAX_COMMANDS 30

#define CMDARG_TYPE_STRING 0
#define CMDARG_TYPE_INTEGER 1

typedef struct CmdArg {
	signed int unk1;
	unsigned int uinteger;
	signed int integer;
	unsigned int type;
	unsigned char* string;
} CmdArg;

typedef int(*CmdFunction)(int argc, CmdArg* argv);

typedef struct CmdInfo {
	char* name;
	CmdFunction handler;
	char* description;
} CmdInfo;

extern void* gCmdListBegin;
extern void* gCmdListEnd;

extern int gCmdCount;
extern Bool gCmdHasInit;
extern CmdInfo** gCmdCommands;

extern int(*jump_to)(int flags, void* addr, int phymem);
extern int(*cmd_ramdisk)(int argc, CmdArg* argv);

int cmd_init();
void cmd_add(char* name, CmdFunction handler, char* description);

int cmd_help(int argc, CmdArg* argv);
int cmd_echo(int argc, CmdArg* argv);
int cmd_hexdump(int argc, CmdArg* argv);
int cmd_jump(int argc, CmdArg* argv);
int cmd_mw(int argc, CmdArg* argv);
int cmd_md(int argc, CmdArg* argv);
int cmd_call(int argc, CmdArg* argv);
int cmd_fsboot(int argc, CmdArg* argv);

#endif
