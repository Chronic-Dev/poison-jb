/*
 *
 *  greenpois0n - kernel.c
 *  (c) 2009 Chronic-Dev Team
 *
 */

#include "lzss.h"
#include "lock.h"
#include "image.h"
#include "kernel.h"
#include "common.h"
#include "filesystem.h"

void kernel_load(void* image, void* dest) {
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
