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

const unsigned char patch_cert_seq[] = "\x1a\x78\xff\x2a\x4f\xf0\xff\x30";
const unsigned char patch_cert[] = "\x00\x20\x00\x20";

const unsigned char patch_perm_seq[] = "\xf3\xdf\x90\xb5\x07\x4b\x1b\x68";
const unsigned char patch_perm[] = "\x4f\xf0\xff\x33";

const unsigned char patch_tags_seq[] = "\x11\x9a\xd3\xf1\x18\xbf\x01\x20";
const unsigned char patch_tags[] = "\x00\x20\x00\x20";

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
	unsigned char* cert_offset = patch_find(address, size, patch_cert_seq, sizeof(patch_cert_seq));
	if(cert_offset == NULL) {
		printf("Unable to find RSA patch offset\n");
		return -1;
	}
	cert_offset += 4;
	printf("Found RSA patch offset at %p\n", cert_offset);
	SETREG32(cert_offset, patch_cert);
	return 0;
}

unsigned char* patch_find(unsigned char* start, int length, const unsigned char find[], int size) {
	int i = 0;
	for(i = 0; i < length; i++) {
		if(!memcmp(&start[i], find, size)) {
			return &start[i];
		}
	}
	return NULL;
}
