/*
 *
 *  greenpois0n - payload/patch.c
 *  (c) 2010 Chronic-Dev Team
 *
 */

#include <stdio.h>
#include <string.h>

#include "lock.h"
#include "patch.h"
#include "common.h"
#include "commands.h"

const unsigned char* patch_cert_seq = "\x4F\xF0\xFF\x30\xDD\xF8\x40\x24";
const unsigned char* patch_cert = "\x00\x20\x00\x20";

const unsigned char patch_perm_seq[] = "\xf3\xdf\x90\xb5\x07\x4b\x1b\x68";
const unsigned char patch_perm[] = "\x4f\xf0\xff\x33";

const unsigned char patch_ecid_seq[] = "\x02\x94\x03\x94\x01\x90\x28\x46";
const unsigned char patch_ecid[] = "\x00\x20\x00\x20";

const unsigned char patch_bgcolor_seq[] = "\x80\xb5\x00\xaf\x04\x28\x0b\x46";
const unsigned char patch_bgcolor[] = "\x00\x4b\x18\x47\x00\x00\x00\x41";


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


int patch_kernel(void* address, int size) {
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

	unsigned char* getenv_string = patch_find(address, size, "getenv", 6);
	if(getenv_string == NULL) {
		printf("Unable to find getenv string\n");
		return -1;
	}
	printf("Found getenv string at %p\n", getenv_string);

	unsigned char* getenv_xref = patch_find(address, size, (unsigned char*)&getenv_string, 4);
	if(getenv_string == NULL) {
		printf("Unable to find getenv xref\n");
		return -1;
	}
	printf("Found getenv xref at %p\n", getenv_xref);

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
