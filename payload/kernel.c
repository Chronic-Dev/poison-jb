/*
 *
 *  greenpois0n - payload/kernel.c
 *  (c) 2009 Chronic-Dev Team
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lzss.h"
#include "lock.h"
#include "image.h"
#include "patch.h"
#include "nvram.h"
#include "kernel.h"
#include "common.h"
#include "commands.h"
#include "filesystem.h"

static char* gKernelAddr = NULL;
char* gBootArgs = SELF_KERNEL_BOOTARGS;
char** gKernelPhyMem = SELF_KERNEL_PHYMEM;

int(*kernel_load)(void* input, int max_size, char** output) = SELF_KERNEL_LOAD;

int kernel_init() {
	//cmd_add("kernel", &kernel_cmd, "operations for filesystem kernel");
	cmd_add("kernel", &kernel_cmd, "Nothing");
	return 0;
}

int kernel_cmd(int argc, CmdArg* argv) {
	char* action = NULL;
	unsigned int size = 0;
	unsigned int* compressed = 0;
	unsigned char* address = NULL;
	unsigned char* decompressed = NULL;
	if(argc < 2) {
		puts("usage: kernel <load/patch> [options]\n");
		puts("  load <address> <size>         \t\tload filesystem kernel to address\n");
		puts("  patch <address> <size>        \t\tpatches kernel at address in memory\n");
		puts("  boot                          \t\tboot a loaded kernel\n");
		return 0;
	}

	action = argv[1].string;
	size = argv[3].uinteger;
	address = (unsigned char*) argv[2].uinteger;
	if(!strcmp(action, "load")) {
		void* image = 0;

#if TARGET_FS_MOUNT && TARGET_FS_LOAD_FILE
		fs_mount("nand0a", "hfs", "/boot");
		fs_load_file(KERNEL_PATH, (void*) address, compressed);
#endif

		printf("Load kernelcache image at %p with %u bytes\n", address, *compressed);
#if TARGET_KERNEL_BOOTARGS
		NvramVar* bootargs = nvram_find_var("boot-args");
		printf("boot-args set to %s\n", bootargs->string);
		strcpy(gBootArgs, "-v serial=1 debug=10");//bootargs->string);
#endif
		kernel_load((void*) address, size, &gKernelAddr);
		printf("Kernelcache prepped at %p with %p and phymem %p\n", address, gKernelAddr, *gKernelPhyMem);
		patch_kernel(0x40000000, 0xF00000);
		printf("Booting kernelcache...\n");
		jump_to(3, gKernelAddr, *gKernelPhyMem);
	}
	else if(!strcmp(action, "patch")) {
		printf("patching kernel..\n");
		patch_kernel(address, size);
	}
	else if(!strcmp(action, "boot")) {
		if(gKernelAddr) {
			printf("booting kernel...\n");
			jump_to(3, gKernelAddr, *gKernelPhyMem);
		}
	}
	return 0;
}

/*
int kernel_load(void* dest, int max_size, int* size) {
	unsigned int* file_size = 0;
	void* image = (void*) 0x43000000;
    ImageHeader* header = (ImageHeader*) image;

	puts("Loading kernelcache image\n");

    // Mount disk and load kernelcache
	fs_mount("nand0a", "hfs", "/boot");
	fs_load_file(KERNEL_PATH, image, file_size);

    // Decrypt image
    image_decrypt(image);

    // Find start of (actual) data
	unsigned char* data = image_find_tag(image, IMAGE_DATA, header->fullSize);
    data += sizeof(ImageTagHeader);

    // Assume data starts with a complzss header
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
	*size = len;
	return 0;
}
*/

int kernel_patch(void* address) {
	printf("Not implemented yet\n");
	return 0;
}

