/*
 *
 *  greenpois0n - payload/patch.c
 *  (c) 2010 Chronic-Dev Team
 *
 */

#include <stdio.h>
#include <string.h>

#include "task.h"
#include "lock.h"
#include "patch.h"
#include "common.h"
#include "commands.h"

const unsigned char* patch_cert_seq = "\x4F\xF0\xFF\x30\xDD\xF8\x40\x24";
const unsigned char* patch_cert = "\x00\x20\x00\x20";

//const unsigned char patch_perm_seq[] = "\xf3\xdf\x90\xb5\x07\x4b\x1b\x68";
//const unsigned char patch_perm[] = "\x4f\xf0\xff\x33";

const unsigned char patch_ecid_seq[] = "\x02\x94\x03\x94\x01\x90\x28\x46";
const unsigned char patch_ecid[] = "\x00\x20\x00\x20";

const unsigned char patch_command_seq1[] = "\x80\xb5\x00\xaf\x82\xb0\x4f\xf0";
const unsigned char patch_command_seq2[] = "\x90\xB5\x01\xAF\x84\xB0";

const unsigned char patch_command[] = "\x00\x4b\x18\x47\x00\x00\x00\x41";

int patch_init() {
	cmd_add("patch", &patch_cmd, "patch firmware in memory");
	return 0;
}

int patch_cmd(int argc, CmdArg* argv) {
	unsigned int size = 0;
	unsigned char* address = NULL;
	if(argc != 3) {
		puts("usage: patch <address> <size>\n");
		puts("  address           \t\taddress to the image to patch\n");
		puts("  size              \t\tsize of image to search\n");
		return 0;
	}

	size = argv[2].uinteger;
	address = (unsigned char*) argv[1].uinteger;
	return patch_firmware(address, size);
}


int patch_kernel(unsigned char* address, unsigned int size) {
	unsigned int target = 0;
	/*
	CSED: 00 00 00 00 01 00 00 00  80 00 00 00 00 00 00 00 => 01 00 00 00 01 00 00 00  80 00 00 00 00 00 00 00
	AMFI: 00 B1 00 24 20 46 90 BD => 00 B1 01 24 20 46 90 BD
	PROD: 00 23 00 94 01 95 02 95 + 10 => 00 20 00 20
	ECID: 02 90 03 90 1D 49 50 46 + 12 => 00 20 00 20
	SHSH: D3 80 04 98 02 21 7C 4B + 8 => 00 20
	      98 47 50 B9 00 98 02 21 + 8 => 00 20
	????: 00 28 40 F0 CC 80 04 98 + 8 => 00 20 00 20
	      28 B9 00 98 FF F7 03 FF + 8 => 00 20 00 20
	????: 1F 4C 1E E0 28 46 51 46 + 8 => 01 20 01 20
	SHA1: A0 47 08 B1 28 46 30 E0 + 8 => 00 20 00 20
	*/
	unsigned int i = 0;
	enter_critical_section();
	for(i = 0; i < size; i++) {
		if(!memcmp(&address[i], "\x00\x00\x00\x00\x01\x00\x00\x00\x80\x00\x00\x00\x00\x00\x00\x00", 16)) {
			target = i + 0;
			printf("Found kernel patch 1 at %p\n", &address[target]);
			memcpy(&address[target], "\x01\x00\x00\x00\x01\x00\x00\x00\x80\x00\x00\x00\x00\x00\x00\x00", 16);
			continue;
		}
		if(!memcmp(&address[i], "\x00\xB1\x00\x24\x20\x46\x90\xBD", 8)) {
			target = i + 0;
			printf("Found kernel patch 2 at %p\n", &address[target]);
			memcpy((char*) &address[target], "\x00\xB1\x01\x24\x20\x46\x90\xBD", 8);
			continue;
		}
		if(!memcmp(&address[i], "\x00\x23\x00\x94\x01\x95\x02\x95", 8)) {
			target = i + 10;
			printf("Found kernel patch 3 at %p\n", &address[target]);
			memcpy(&address[target], "\x00\x20\x00\xD3\x80\x04\x98\x02\x21\x7C\x4B\x20", 4);
			continue;
		}
		if(!memcmp(&address[i], "\x02\x90\x03\x90\x1D\x49\x50\x46", 8)) {
			target = i + 12;
			printf("Found kernel patch 4 at %p\n", &address[target]);
			memcpy(&address[target], "\x00\x20\x00\x20", 4);
			continue;
		}
		if(!memcmp(&address[i], "\xD3\x80\x04\x98\x02\x21\x7C\x4B", 8)
				|| !memcmp(&address[i], "\x98\x47\x50\xB9\x00\x98\x02\x21", 8)) {
			target = i + 8;
			printf("Found kernel patch 5 at %p\n", &address[target]);
			memcpy(&address[target], "\x00\x20", 2);
			continue;
		}
		if(!memcmp(&address[i], "\x00\x28\x40\xF0\xCC\x80\x04\x98", 8)
				|| !memcmp(&address[i], "\x28\xB9\x00\x98\xFF\xF7\x03\xFF", 8)) {
			target = i + 8;
			printf("Found kernel patch 6 at %p\n", &address[target]);
			memcpy(&address[target], "\x00\x20\x00\x20", 4);
			continue;
		}
		if(!memcmp(&address[i], "\x1F\x4C\x1E\xE0\x28\x46\x51\x46", 8)) {
			target = i + 8;
			printf("Found kernel patch 7 at %p\n", &address[target]);
			memcpy(&address[target], "\x01\x20\x01\x20", 4);
			continue;
		}
		if(!memcmp(&address[i], "\xA0\x47\x08\xB1\x28\x46\x30\xE0", 8)) {
			target = i + 8;
			printf("Found kernel patch 8 at %p\n", &address[target]);
			memcpy(&address[target], "\x00\x20\x00\x20", 4);
			continue;
		}
		//task_yield();
	}
	exit_critical_section();
	return 0;
}

int patch_firmware(unsigned char* address, int size) {
	unsigned char* cert_offset = patch_find(address, size, patch_cert_seq, 8);
	if(cert_offset == NULL) {
		printf("Unable to find RSA patch offset\n");
		return -1;
	}
	printf("Found RSA patch offset at %p\n", cert_offset);
	memcpy(cert_offset, patch_cert, 4);

	unsigned char* ecid_offset = patch_find(address, size, patch_ecid_seq, 8);
	if(ecid_offset == NULL) {
		printf("Unable to find ECID patch offset\n");
		return -1;
	}
	ecid_offset += 8;
	printf("Found ECID patch offset at %p\n", ecid_offset);
	memcpy(ecid_offset, patch_ecid, 4);

	unsigned char* command_offset = patch_find(address, size, patch_command_seq1, 8);
	if(command_offset == NULL) {
		command_offset = patch_find(address, size, patch_command_seq2, 6);
		if(command_offset == NULL) {
			printf("Unable to find command patch offset\n");
			return -1;
		}
	}
	printf("Found command patch offset at %p\n", command_offset);
	memcpy(command_offset, patch_command, 8);

	return 0;
}

unsigned char* patch_find(unsigned char* start, int length, unsigned char* find, int size) {
	int i = 0;
	for(i = 0; i < length; i++) {
		if(!memcmp(&start[i], find, size)) {
			return &start[i];
		}
	}
	return NULL;
}

unsigned char* patch_rfind(unsigned char* start, int length, unsigned char* find, int size) {
	int i = 0;
	for(i = length; i < length; i--) {
		if(!memcmp(&start[i], find, size)) {
			return &start[i];
		}
	}
	return NULL;
}
