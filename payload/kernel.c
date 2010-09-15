/*
 *
 *  greenpois0n - payload/kernel.c
 *  (c) 2009 Chronic-Dev Team
 *
 */

#include "lzss.h"
#include "lock.h"
#include "image.h"
#include "kernel.h"
#include "common.h"
#include "commands.h"
#include "filesystem.h"

int kernel_init() {
	//cmd_add("kernel", &kernel_cmd, "operations for filesystem kernel");
	cmd_add("kernel", &kernel_cmd, "Nothing");
	return 0;
}

int kernel_cmd(int argc, CmdArg* argv) {
	char* action = NULL;
	unsigned char* address = NULL;
	if(argc != 3) {
		puts("usage: kernel <load/patch> [options]\n");
		puts("  load <address>         \t\tload filesystem kernel to address\n");
		puts("  patch <address>        \t\tpatches kernel at address in memory\n");
		return 0;
	}

	action = argv[1].string;
	address = argv[2].uinteger;
	if(!strcmp(action, "load")) {
		kernel_load(0x42000000, 0x43000000);
	}
	else if(!strcmp(action, "patch")) {
		kernel_patch(0x43000000);
	}
	return 0;
}

int kernel_load(void* image, void* dest) {
	unsigned int* size = 0;
    ImageHeader* header = (ImageHeader*) image;

	puts("Loading kernelcache image\n");

    /* Mount disk and load kernelcache */
	fs_mount("nand0a", "hfs", "/boot");
	fs_load_file(KERNEL_PATH, image, size);

    /* Decrypt image */
    image_decrypt(image);

    /* Find start of (actual) data */
	unsigned char* data = image_find_tag(image, IMAGE_DATA, header->fullSize);
    data += sizeof(ImageTagHeader);

    /* Assume data starts with a complzss header */
    ImageComp* comp = (ImageComp*)data;
    FLIPENDIAN(comp->signature);
    FLIPENDIAN(comp->compressionType);
    FLIPENDIAN(comp->compressedSize);
    FLIPENDIAN(comp->uncompressedSize);
    
    if(comp->signature != IMAGE_COMP || comp->compressionType != IMAGE_LZSS) {
        puts("Didn't find expected COMPLZSS header\n");
        fs_unmount("/boot");
        return;
    }

    enter_critical_section();
    printf("LZSS compressed: %d uncompressed: %d\n", comp->compressedSize, comp->uncompressedSize);
    exit_critical_section();

    char *lzss_data = ((char *)(comp) + sizeof(ImageComp));
    int len = lzss_decompress(dest, comp->uncompressedSize, lzss_data, comp->compressedSize);

    enter_critical_section();
    printf("Actually uncompressed: %d\n", len);
	exit_critical_section();

	fs_unmount("/boot");

	puts("Finished loading kernelcache\n");
}

int kernel_patch(void* address) {
	printf("Not implemented yet\n");
	return 0;
}
