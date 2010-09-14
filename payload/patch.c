/*
 *
 *  greenpois0n - payload/patch.c
 *  (c) 2010 Chronic-Dev Team
 *
 */

#include <stdio.h>
#include <string.h>

#include "lock.h"
#include "common.h"

LinkedList* gNvramList = SELF_NVRAM_LIST;

int patch_init() {
	cmd_add("nvram", &nvram_cmd, "view and modify nvram variables");
	return 0;
}

int patch_cmd(int argc, CmdArg* argv) {
	char* var = NULL;
	char* value = NULL;
	char* action = NULL;
	if(argc < 2) {
		puts("usage: nvram <action> [options]\n");
		puts("  list              \t\tdisplay list of nvram variables\n");
		puts("  remove <var>      \t\tdeletes entry from nvram variables\n");
		puts("  get <var>         \t\tshow contents of nvram variable\n");
		puts("  set <var> <value> \t\tset contents of nvram variable\n\n");
		return 0;
	}

	action = argv[1].string;
	if(argc == 2) {
		if(!strcmp(action, "list")) {
			nvram_display_list();
			return 0;
		}

	} else if(argc == 3) {
		if(!strcmp(action, "get")) {
			var = argv[2].string;
			return nvram_get_var(var);

		} else if(!strcmp(action, "remove")) {
			var = argv[2].string;
			return nvram_remove_var(var);
		}

	} else if(argc == 4) {
		if(!strcmp(action, "set")) {
			var = argv[2].string;
			value = argv[3].string;
			return nvram_set_var(var, value);
		}
	}

	return 0;
}


