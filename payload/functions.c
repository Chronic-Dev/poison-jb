/**
  * GreenPois0n Cynanide - functions.c
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

#include "task.h"
#include "lock.h"
#include "common.h"
#include "commands.h"

static unsigned char push = 0xB5;
static unsigned char push_r7_lr[] = { 0x80, 0xB5 };
static unsigned char push_r4_r7_lr[] = { 0x90, 0xB5 };
static unsigned char push_r4_to_r7_lr[] = { 0xF0, 0xB5 };
static unsigned char push_r4_r5_r7_lr[] = { 0xB0, 0xB5 };

static unsigned char* functions[][3] = {
	{ "aes_crypto_cmd", "aes_crypto_cmd", push_r4_r7_lr },
	{ "free", "heap_panic",  push_r4_to_r7_lr },
	{ "fs_mount", "fs_mount", push_r4_to_r7_lr },
	{ "cmd_ramdisk", "Ramdisk too large", push_r4_r5_r7_lr },
	{ "cmd_go", "jumping into image", push_r7_lr },
	{ "image_load", "image validation failed but untrusted images are permitted", push_r4_to_r7_lr },
	{ NULL, NULL, NULL }
};

unsigned int find_offset(unsigned char* data, unsigned int base, unsigned int size, unsigned char** what) {
	unsigned int i = 0;
	unsigned char* top = what[2];
	unsigned char* name = what[0];
	unsigned char* signature = what[1];
	unsigned int dbase = (unsigned int) data;

	// First find the string
	unsigned int address = 0;
	for(i = 0; i < size; i++) {
		if(!memcmp(&data[i], signature, strlen(signature))) {
			address = base | i;
			printf("Found %s string at 0x%x\n", name, address);
			break;
		}
	}
	if(address == 0) return -1;

	// Next find where that string is referenced
	unsigned int reference = 0;
	for(i = 0; i < size; i++) {
		if(!memcmp(&data[i], &address, 4)) {
			reference = base | i;
			printf("Found %s reference at 0x%x\n", name, reference);
			break;
		}
	}
	if(reference == 0) return -2;

	// Finally find the top of that function
	unsigned int function = 0;
	while(i > 0) {
		i--;
		if(data[i] == push) {
			function = dbase | i;
			printf("Found %s function at 0x%x\n", name, function);
			break;
		}
	}
	if(function == 0) return -3;

	return function;
}

void* find_string(const char* name) {
	// First find the string
	int i = 0;
	unsigned int address = 0;
	unsigned int size = 0x38000;
	unsigned char* data = TARGET_BASEADDR;
	for(i = 0; i < size; i++) {
		if(!memcmp(&data[i], name, strlen(name))) {
			address = TARGET_BASEADDR | i;
			printf("Found %s string at 0x%x\n", name, address);
			break;
		}
	}
	if(address == 0) return -1;
	return (void*) address;
}

void* find_function(const char* name, unsigned char* target, unsigned char* base) {
	int i = 0;
	unsigned int found = 0;
	for(i = 0; i < sizeof(functions); i++) {
		if(!strcmp(functions[i][0], name)) {
			printf("Searching for %s\n", functions[i][0]);
			found = find_offset(target, base, 0x35000, functions[i]);
			if(found < 0) {
				printf("Unable to find %s, error %d\n", functions[i][0], found);
				return NULL;
			}
			break;
		}
	}

	return (void*) found;
}
